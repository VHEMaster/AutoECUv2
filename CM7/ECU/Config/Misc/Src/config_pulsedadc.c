/*
 * config_pulsedadc.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_rcc.h"
#include "config_pulsedadc.h"
#include "config_extern.h"
#include "compiler.h"

#define ECU_PULSEDADC_SAMPLING_BUFFER_SIZE    2048

static uint16_t ecu_devices_pulsedadc_sambling_buffer[ECU_DEVICE_PULSEDADC_MAX][ECU_PULSEDADC_SAMPLING_BUFFER_SIZE];

typedef struct {
    pulsedadc_init_ctx_t init;
    pulsedadc_ctx_t *ctx;

}ecu_devices_pulsedadc_ctx_t;

static ecu_devices_pulsedadc_ctx_t ecu_devices_pulsedadc_ctx[ECU_DEVICE_PULSEDADC_MAX] = {
    {
        .init = {
            .samples_buffer_size = ECU_PULSEDADC_SAMPLING_BUFFER_SIZE,
            .samples_buffer = ecu_devices_pulsedadc_sambling_buffer[ECU_DEVICE_PULSEDADC_1],

            .hadc = &hadc1,
            .adc_channel = ADC_CHANNEL_10,

            .hhrtim = &hhrtim,
            .hrtim_index = HRTIM_TIMERINDEX_TIMER_A,

            .base_frequency = 0,
            .sampling_frequency_default = 44100,
        },
    },
    {
        .init = {
            .samples_buffer_size = ECU_PULSEDADC_SAMPLING_BUFFER_SIZE,
            .samples_buffer = ecu_devices_pulsedadc_sambling_buffer[ECU_DEVICE_PULSEDADC_2],

            .hadc = &hadc2,
            .adc_channel = ADC_CHANNEL_11,

            .hhrtim = &hhrtim,
            .hrtim_index = HRTIM_TIMERINDEX_TIMER_B,

            .base_frequency = 0,
            .sampling_frequency_default = 44100,
        },
    }
};

static void ecu_devices_pulsedadc_error_cb(ADC_HandleTypeDef *hadc)
{
  ecu_devices_pulsedadc_ctx_t *pulsedadc_ctx;

  for(int i = 0; i < ITEMSOF(ecu_devices_pulsedadc_ctx); i++) {
    pulsedadc_ctx = &ecu_devices_pulsedadc_ctx[i];

    if(pulsedadc_ctx->init.hadc == hadc) {
      pulsedadc_adc_dma_error(pulsedadc_ctx->ctx);
    }
  }
}

error_t ecu_devices_pulsedadc_init(ecu_device_pulsedadc_t instance, pulsedadc_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  ecu_devices_pulsedadc_ctx_t *pulsedadc_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_PULSEDADC_MAX || ctx == NULL, err = E_PARAM);

    pulsedadc_ctx = &ecu_devices_pulsedadc_ctx[instance];
    pulsedadc_ctx->ctx = ctx;

    err = ecu_config_get_tim_base_frequency(&htim2, &pulsedadc_ctx->init.base_frequency);
    BREAK_IF(err != E_OK);

    status = HAL_ADC_RegisterCallback(pulsedadc_ctx->init.hadc, HAL_ADC_CONVERSION_COMPLETE_CB_ID, ecu_devices_pulsedadc_error_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    status = HAL_ADC_RegisterCallback(pulsedadc_ctx->init.hadc, HAL_ADC_ERROR_CB_ID, ecu_devices_pulsedadc_error_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    err = pulsedadc_init(pulsedadc_ctx->ctx, &pulsedadc_ctx->init);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
