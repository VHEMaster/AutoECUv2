/*
 * pulsedadc.h
 *
 *  Created on: Apr 28, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_PULSEDADC_INC_PULSEDADC_H_
#define DRIVERS_PULSEDADC_INC_PULSEDADC_H_

#include "common.h"
#include "time.h"

#define PULSEDADC_SAMPLES_ALL   (0u)

typedef struct pulsedadc_sampling_cplt_ctx_tag pulsedadc_sampling_cplt_ctx_t;

typedef void (*pulsedadc_sampling_cplt_cb_t)(void *usrdata, const pulsedadc_sampling_cplt_ctx_t *cplt_ctx);
typedef void (*pulsedadc_sampling_error_cb_t)(void *usrdata);

typedef enum {
  PULSEDADC_STATUS_NONE = 0,
  PULSEDADC_STATUS_IDLE = 1,
  PULSEDADC_STATUS_PREPARED = 2,
  PULSEDADC_STATUS_RUNNING = 4,
  PULSEDADC_STATUS_CPLT = 8,
  PULSEDADC_STATUS_ERROR = 16,
}pulsedadc_status_t;

typedef struct pulsedadc_sampling_cplt_ctx_tag {
    const uint16_t *samples_buffer;
    uint16_t samples_count;
    time_us_t time_start;
    time_us_t time_cplt;
}pulsedadc_sampling_cplt_ctx_t;

typedef struct {
    uint32_t samples_buffer_size;
    uint16_t *samples_buffer;
    bool ring_buffer;

    ADC_HandleTypeDef *hadc;
    uint32_t adc_channel;

    HRTIM_HandleTypeDef *hhrtim;
    uint32_t hrtim_index;

    uint32_t base_frequency;
    uint32_t sampling_frequency_default;

    pulsedadc_sampling_cplt_cb_t sampling_cplt_cb;
    pulsedadc_sampling_error_cb_t sampling_error_cb;
    void *callback_usrdata;
}pulsedadc_init_ctx_t;

typedef struct {
    pulsedadc_init_ctx_t init;
    bool ready;
    pulsedadc_status_t status;

    uint32_t sampling_frequency;
    uint32_t target_samples;
    uint32_t current_samples;
    uint16_t *samples_buffer;

    pulsedadc_sampling_cplt_ctx_t sampling_cplt_ctx;
}pulsedadc_ctx_t;

error_t pulsedadc_init(pulsedadc_ctx_t *ctx, const pulsedadc_init_ctx_t *init_ctx);

error_t pulsedadc_set_sampling_frequency(pulsedadc_ctx_t *ctx, uint32_t sampling_frequency);

error_t pulsedadc_prepare(pulsedadc_ctx_t *ctx, uint16_t samples);
error_t pulsedadc_start(pulsedadc_ctx_t *ctx);
error_t pulsedadc_stop(pulsedadc_ctx_t *ctx);
error_t pulsedadc_get_samples(pulsedadc_ctx_t *ctx, const uint16_t **buffer, uint16_t *samples);
error_t pulsedadc_get_status(pulsedadc_ctx_t *ctx, pulsedadc_status_t *status);

void pulsedadc_adc_dma_cplt(pulsedadc_ctx_t *ctx);
void pulsedadc_adc_dma_error(pulsedadc_ctx_t *ctx);

#endif /* DRIVERS_PULSEDADC_INC_PULSEDADC_H_ */
