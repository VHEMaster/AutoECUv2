/*
 * config_common_private.c
 *
 *  Created on: 14 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "config_common_private.h"
#include "config_extern.h"

typedef struct {
    MDMA_HandleTypeDef *hdma;
    error_t dma_errcode;
    bool initialized;
}ecu_config_common_mdma_ctx_t;

static ecu_config_common_mdma_ctx_t ecu_config_common_mdma_ctx = {
  .hdma = &hmdma_memtomem_stream3,
  .initialized = false,
};

static void ecu_config_common_mdma_cplt_cb(MDMA_HandleTypeDef *hdma);
static void ecu_config_common_mdma_err_cb(MDMA_HandleTypeDef *hdma);

error_t ecu_config_common_mdma_init(void)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  do {
    ecu_config_common_mdma_ctx.hdma->Init.Request = MDMA_REQUEST_SW;
    ecu_config_common_mdma_ctx.hdma->Init.TransferTriggerMode = MDMA_FULL_TRANSFER;
    ecu_config_common_mdma_ctx.hdma->Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
    ecu_config_common_mdma_ctx.hdma->Init.SourceInc = MDMA_SRC_INC_DOUBLEWORD;
    ecu_config_common_mdma_ctx.hdma->Init.DestinationInc = MDMA_DEST_INC_DOUBLEWORD;
    ecu_config_common_mdma_ctx.hdma->Init.SourceDataSize = MDMA_SRC_DATASIZE_DOUBLEWORD;
    ecu_config_common_mdma_ctx.hdma->Init.DestDataSize = MDMA_DEST_DATASIZE_DOUBLEWORD;

    status = HAL_MDMA_Init(ecu_config_common_mdma_ctx.hdma);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_MDMA_RegisterCallback(ecu_config_common_mdma_ctx.hdma, HAL_MDMA_XFER_CPLT_CB_ID, ecu_config_common_mdma_cplt_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    status = HAL_MDMA_RegisterCallback(ecu_config_common_mdma_ctx.hdma, HAL_MDMA_XFER_ERROR_CB_ID, ecu_config_common_mdma_err_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    ecu_config_common_mdma_ctx.initialized = true;
    ATOMIC_STORE(ecu_config_common_mdma_ctx.dma_errcode, E_OK);

  } while(0);

  return err;
}

error_t ecu_config_common_mdma_copy(void *dst, const void *src, uint32_t bytes)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(ecu_config_common_mdma_ctx.initialized != true, err = E_NOTRDY);
    BREAK_IF_ACTION(ATOMIC_LOAD(ecu_config_common_mdma_ctx.dma_errcode) == E_AGAIN, err = E_AGAIN);

    ATOMIC_STORE(ecu_config_common_mdma_ctx.dma_errcode, E_AGAIN);
    status = HAL_MDMA_Start_IT(ecu_config_common_mdma_ctx.hdma, (uint32_t)src, (uint32_t)dst, bytes, 1);
    if(status != HAL_OK) {
      ATOMIC_STORE(ecu_config_common_mdma_ctx.dma_errcode, E_OK);
      err = E_HAL;
      break;
    }

  } while(0);

  return err;
}

error_t ecu_config_common_mdma_poll(void)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ecu_config_common_mdma_ctx.initialized != true, err = E_NOTRDY);
    err = ATOMIC_LOAD(ecu_config_common_mdma_ctx.dma_errcode);

  } while(0);

  return err;
}

static void ecu_config_common_mdma_cplt_cb(MDMA_HandleTypeDef *hdma)
{
  ATOMIC_STORE(ecu_config_common_mdma_ctx.dma_errcode, E_OK);
}

static void ecu_config_common_mdma_err_cb(MDMA_HandleTypeDef *hdma)
{
  ATOMIC_STORE(ecu_config_common_mdma_ctx.dma_errcode, E_IO);
}
