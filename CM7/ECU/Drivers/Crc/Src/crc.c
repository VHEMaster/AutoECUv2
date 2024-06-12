/*
 * crc.c
 *
 *  Created on: May 5, 2024
 *      Author: VHEMaster
 */

#include "crc.h"
#include "crc_fsm.h"
#include "config_loop.h"
#include "compiler.h"
#include <string.h>

static crc_ctx_t crc_ctx = {0};

ITCM_FUNC static void crc_loop_fast(crc_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    err = crc_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set DTC here?
    }

  } while(0);
}

static void crc_dma_clpt_cb(DMA_HandleTypeDef *hdma)
{
  crc_ctx_t *ctx = &crc_ctx;

  if(ctx->hdma == hdma) {
    ctx->dma_errcode = E_OK;
  }
}

static void crc_dma_err_cb(DMA_HandleTypeDef *hdma)
{
  crc_ctx_t *ctx = &crc_ctx;

  if(ctx->hdma == hdma) {
    ctx->dma_errcode = E_IO;
  }
}

error_t crc_init(void)
{
  crc_ctx_t *ctx = &crc_ctx;
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  do {
    memset(ctx, 0u, sizeof(crc_ctx_t));

    ctx->hcrc = &hcrc;
    ctx->hdma = &hdma_memtomem_dma2_stream1;

    ctx->hcrc->Instance = CRC;
    ctx->hcrc->Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
    ctx->hcrc->Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
    ctx->hcrc->Init.GeneratingPolynomial = CRC_POLY;
    ctx->hcrc->Init.CRCLength = CRC_POLYLENGTH_16B;
    ctx->hcrc->Init.InitValue = 0xFFFF;
    ctx->hcrc->Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
    ctx->hcrc->Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
    ctx->hcrc->InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

    status = HAL_CRC_Init(ctx->hcrc);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    ctx->hdma->Init.Request = DMA_REQUEST_MEM2MEM;
    ctx->hdma->Init.Direction = DMA_MEMORY_TO_MEMORY;
    ctx->hdma->Init.PeriphInc = DMA_PINC_ENABLE;
    ctx->hdma->Init.MemInc = DMA_MINC_DISABLE;
    ctx->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ctx->hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    status = HAL_DMA_Init(ctx->hdma);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_DMA_RegisterCallback(ctx->hdma, HAL_DMA_XFER_CPLT_CB_ID, crc_dma_clpt_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    status = HAL_DMA_RegisterCallback(ctx->hdma, HAL_DMA_XFER_ERROR_CB_ID, crc_dma_err_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    err = ecu_loop_register_fast((ecu_loop_cb_t)crc_loop_fast, ctx, 0);
    BREAK_IF(err != E_OK);

    ctx->ready = true;

  } while(0);

  return err;
}

error_t crc_lock(void)
{
  crc_ctx_t *ctx = &crc_ctx;
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    if(ctx->locked == true) {
      err = E_AGAIN;
    } else {
      ctx->locked = true;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t crc_unlock(void)
{
  crc_ctx_t *ctx = &crc_ctx;
  error_t err = E_OK;
  uint32_t prim;

  do {
    prim = EnterCritical();
    if(ctx->locked == false || ctx->cmd_ready != false) {
      err = E_INVALACT;
    } else {
      ctx->locked = false;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t crc_calculate(crc_checksum_t *crc, const void *payload, uint32_t length, bool reset)
{
  crc_ctx_t *ctx = &crc_ctx;
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->locked == false, err = E_INVALACT);

    if(ctx->cmd_ready == false) {
      ctx->cmd_address = (uint32_t)payload;
      ctx->cmd_length = length;
      ctx->cmd_reset = reset;

      ctx->cmd_errcode = E_AGAIN;
      ctx->cmd_ready = true;
    }
    if(ctx->cmd_errcode != E_AGAIN) {
      *crc = ctx->hcrc->Instance->DR;
      err = ctx->cmd_errcode;
      ctx->cmd_ready = false;
    } else {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}
