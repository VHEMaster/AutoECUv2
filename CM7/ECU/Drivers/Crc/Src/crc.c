/*
 * crc.c
 *
 *  Created on: May 5, 2024
 *      Author: VHEMaster
 */

#include "crc.h"
#include "config_extern.h"
#include "compiler.h"
#include <string.h>

typedef struct {
    CRC_HandleTypeDef *hcrc;
    DMA_HandleTypeDef *hdma;
    bool ready;
    bool locked;
    bool busy;
}crc_ctx_t;

static crc_ctx_t crc_ctx = {0};

error_t crc_init(void)
{
  crc_ctx_t *ctx = &crc_ctx;
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  do {
    memset(ctx, 0u, sizeof(crc_ctx_t));

    ctx->hcrc = &hcrc;
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
    if(ctx->locked == false || ctx->busy != false) {
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

}
