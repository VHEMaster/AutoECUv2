/*
 * pulsedadc.h
 *
 *  Created on: Apr 28, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_LL_PULSEDADC_INC_PULSEDADC_H_
#define DRIVERS_LL_PULSEDADC_INC_PULSEDADC_H_

#include "main.h"
#include "errors.h"
#include "bool.h"

typedef enum {
  PULSEDADC_STATUS_IDLE = 0,
  PULSEDADC_STATUS_PREPARED,
  PULSEDADC_STATUS_RUNNING,
  PULSEDADC_STATUS_OVERFLOW,
}pulsedadc_status_t;

typedef struct {
    uint32_t samples_buffer_size;
    uint16_t *samples_buffer;

    ADC_HandleTypeDef *hadc;
    uint32_t adc_channel;

    HRTIM_HandleTypeDef *hhrtim;
    uint32_t hrtim_index;

    uint32_t base_frequency;
    uint32_t sampling_frequency_default;
}pulsedadc_init_ctx_t;

typedef struct {
    pulsedadc_init_ctx_t init;
    bool ready;
    pulsedadc_status_t status;

    uint32_t sampling_frequency;
    uint32_t current_samples;
}pulsedadc_ctx_t;

error_t pulsedadc_init(pulsedadc_ctx_t *ctx, const pulsedadc_init_ctx_t *init_ctx);

error_t pulsedadc_set_sampling_frequency(pulsedadc_ctx_t *ctx, uint32_t sampling_frequency);

error_t pulsedadc_prepare(pulsedadc_ctx_t *ctx);
error_t pulsedadc_start(pulsedadc_ctx_t *ctx);
error_t pulsedadc_stop(pulsedadc_ctx_t *ctx);
error_t pulsedadc_get_samples(pulsedadc_ctx_t *ctx, uint16_t **buffer, uint32_t *samples);
error_t pulsedadc_get_status(pulsedadc_ctx_t *ctx, pulsedadc_status_t *status);

void pulsedadc_adc_dma_error(pulsedadc_ctx_t *ctx);

#endif /* DRIVERS_LL_PULSEDADC_INC_PULSEDADC_H_ */
