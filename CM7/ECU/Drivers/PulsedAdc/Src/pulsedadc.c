/*
 * pulsedadc.c
 *
 *  Created on: Apr 28, 2024
 *      Author: VHEMaster
 */

#include "pulsedadc.h"
#include "compiler.h"
#include <string.h>

error_t pulsedadc_init(pulsedadc_ctx_t *ctx, const pulsedadc_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL || init_ctx->hadc == NULL || init_ctx->hhrtim == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(pulsedadc_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(pulsedadc_init_ctx_t));

    ctx->sampling_frequency = ctx->init.sampling_frequency_default;

    ctx->ready = true;

  } while(0);

  return err;
}

error_t pulsedadc_prepare(pulsedadc_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  HRTIM_TimeBaseCfgTypeDef time_base_cfg = {0};

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready != true, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->status != PULSEDADC_STATUS_IDLE, err = E_INVALACT);

    time_base_cfg.Period = ctx->init.base_frequency / ctx->sampling_frequency;
    time_base_cfg.RepetitionCounter = 0x00;
    time_base_cfg.PrescalerRatio = HRTIM_PRESCALERRATIO_DIV1;
    time_base_cfg.Mode = HRTIM_MODE_CONTINUOUS;
    status = HAL_HRTIM_TimeBaseConfig(ctx->init.hhrtim, ctx->init.hrtim_index, &time_base_cfg);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_ADC_Start_DMA(ctx->init.hadc, (uint32_t *)ctx->init.samples_buffer, ctx->init.samples_buffer_size);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    ctx->status = PULSEDADC_STATUS_PREPARED;

  } while(0);

  return err;
}

error_t pulsedadc_start(pulsedadc_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready != true, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->status != PULSEDADC_STATUS_IDLE && ctx->status != PULSEDADC_STATUS_PREPARED, err = E_INVALACT);

    if(ctx->status == PULSEDADC_STATUS_IDLE) {
      err = pulsedadc_prepare(ctx);
      BREAK_IF(err != E_OK);
    }

    status = HAL_HRTIM_SimpleBaseStart(ctx->init.hhrtim, ctx->init.hrtim_index);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    ctx->status = PULSEDADC_STATUS_RUNNING;

  } while(0);

  return err;
}

error_t pulsedadc_stop(pulsedadc_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready != true, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->status != PULSEDADC_STATUS_RUNNING && ctx->status != PULSEDADC_STATUS_OVERFLOW, err = E_INVALACT);

    status = HAL_HRTIM_SimpleBaseStop(ctx->init.hhrtim, ctx->init.hrtim_index);

    ctx->current_samples = ctx->init.samples_buffer_size -
        ((DMA_Stream_TypeDef *)ctx->init.hadc->DMA_Handle->Instance)->NDTR;

    status = HAL_ADC_Stop_DMA(ctx->init.hadc);

    ctx->status = PULSEDADC_STATUS_IDLE;

    (void)status;

  } while(0);

  return err;
}

error_t pulsedadc_get_samples(pulsedadc_ctx_t *ctx, uint16_t **buffer, uint32_t *samples)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || buffer == NULL || samples == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready != true, err = E_NOTRDY);

    if(ctx->status == PULSEDADC_STATUS_RUNNING) {
      ctx->current_samples = ctx->init.samples_buffer_size -
          (((DMA_Stream_TypeDef *)ctx->init.hadc->DMA_Handle->Instance)->NDTR / sizeof(*ctx->init.samples_buffer));
    }

    *buffer = ctx->init.samples_buffer;
    *samples = ctx->current_samples;

  } while(0);

  return err;
}

error_t pulsedadc_get_status(pulsedadc_ctx_t *ctx, pulsedadc_status_t *status)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || status == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready != true, err = E_NOTRDY);

    *status = ctx->status;

  } while(0);

  return err;
}

void pulsedadc_adc_dma_error(pulsedadc_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL || ctx->status != PULSEDADC_STATUS_RUNNING);

    HAL_HRTIM_SimpleBaseStop(ctx->init.hhrtim, ctx->init.hrtim_index);
    HAL_ADC_Stop_DMA(ctx->init.hadc);

    ctx->status = PULSEDADC_STATUS_OVERFLOW;
    ctx->current_samples = ctx->init.samples_buffer_size;

  } while(0);
}
