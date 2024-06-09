/*
 * config_motor.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include "config_motor.h"
#include "config_extern.h"
#include "config_rcc.h"
#include "middlelayer_spi.h"
#include "compiler.h"
#include <string.h>
#include <math.h>
#include <limits.h>

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    l9960_config_t config_default;
    l9960_init_ctx_t init;
    gpio_t dir_pin;
    TIM_HandleTypeDef *htim_pwm;
    uint32_t channel_pwm;
    __IO uint32_t *channel_ccr;
    bool tim_started;
    l9960_ctx_t *ctx;
    float pwm_frequency;

    bool pulse_dir;
    uint32_t pulse_pwm;
}ecu_devices_motor_ctx_t;

static const l9960_config_t ecu_devices_motor_config_default = {
    .comm_check = L9960_CONFIG_CC_ENABLED,
    .diag_clr_en = L9960_CONFIG_DCE_DISABLED,
    .tsw_low_current = L9960_CONFIG_TLC_GATEFB_ONLY,
    .tdiag1 = L9960_CONFIG_TD1_45US,
    .voltage_slew_rate = L9960_CONFIG_VSR_FAST,
    .current_slow_rate = L9960_CONFIG_ISR_FAST,
    .no_slew_rate = L9960_CONFIG_NOSR_NOT_ALLOWED,
    .temp_depent_slew_rate = L9960_CONFIG_TDSR_NOT_ALLOWED,
    .current_limit = L9960_CONFIG_CL_RANGE1,
    .undervoltage_window = L9960_CONFIG_UVW_20US,
    .spread_spectrum = L9960_CONFIG_SS_ENABLED,
    .undervoltage_protection = L9960_CONFIG_UVP_DISABLED,
    .otwarn_thr_var = L9960_CONFIG_OTW_0,
    .otsd_thr_var = L9960_CONFIG_OTSD_0,
    .in1_in2_if = L9960_CONFIG_IIF_PWM_DIR,
    .otwarn_tsec_en = L9960_CONFIG_OTWTSE_DISABLED,
    .tvvl = L9960_CONFIG_TVVL_400US,
    .vvl_mode = L9960_CONFIG_VVL_ENABLED,
    .ol_on = L9960_CONFIG_OLON_ENABLED,
};

static ecu_devices_motor_ctx_t ecu_devices_motor_ctx[ECU_DEVICE_MOTOR_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_OUTSM1,
      .init = {
          .dis_pin = { .port = MOTOR1_DIS_GPIO_Port, .pin = MOTOR1_DIS_Pin },
      },
      .dir_pin = { .port = MOTOR1_DIR_GPIO_Port, .pin = MOTOR1_DIR_Pin },
      .htim_pwm = &htim8,
      .channel_pwm = TIM_CHANNEL_4,
      .config_default = ecu_devices_motor_config_default,
    },
    {
      .slave_index = ECU_SPI_SLAVE_OUTSM2,
      .init = {
          .dis_pin = { .port = MOTOR2_DIS_GPIO_Port, .pin = MOTOR2_DIS_Pin },
      },
      .dir_pin = { .port = MOTOR2_DIR_GPIO_Port, .pin = MOTOR2_DIR_Pin },
      .htim_pwm = &htim8,
      .channel_pwm = TIM_CHANNEL_3,
      .config_default = ecu_devices_motor_config_default,
    },
};

error_t ecu_devices_motor_init(ecu_device_motor_t instance, l9960_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX || ctx == NULL, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];
    motor_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&motor_ctx->init.spi_slave, motor_ctx->slave_index);
    BREAK_IF(err != E_OK);

    if(gpio_valid(&motor_ctx->dir_pin)) {
      gpio_reset(&motor_ctx->dir_pin);
    }

    err = l9960_init(motor_ctx->ctx, &motor_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&motor_ctx->ctx->config, &motor_ctx->config_default, sizeof(l9960_config_t));

  } while(0);

  return err;
}

error_t ecu_devices_motor_get_default_config(ecu_device_motor_t instance, l9960_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX || config == NULL, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    memcpy(config, &motor_ctx->config_default, sizeof(l9960_config_t));

  } while(0);

  return err;
}

static void ecu_devices_motor_pulse_cplt(TIM_HandleTypeDef *htim)
{
  ecu_devices_motor_ctx_t *motor_ctx;
  uint32_t usrdata_index = POSITION_VAL(htim->Channel);

  do {
    BREAK_IF(usrdata_index >= ITEMSOF(htim->usrdata));

    motor_ctx = htim->usrdata[usrdata_index];

    BREAK_IF(motor_ctx == NULL);

    if(motor_ctx->channel_ccr != NULL) {
      *motor_ctx->channel_ccr = motor_ctx->pulse_pwm;
    }

    if(gpio_valid(&motor_ctx->dir_pin)) {
      gpio_write(&motor_ctx->dir_pin, motor_ctx->pulse_dir);
    }

  } while(0);
}

error_t ecu_devices_motor_configure(ecu_device_motor_t instance, const l9960_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX || config == NULL, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    err = l9960_configure(motor_ctx->ctx, config);
    if(err == E_OK) {
      status = HAL_TIM_RegisterCallback(motor_ctx->htim_pwm, HAL_TIM_PWM_PULSE_FINISHED_CB_ID, ecu_devices_motor_pulse_cplt);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

      motor_ctx->channel_ccr = NULL;
      switch(motor_ctx->channel_pwm) {
        case TIM_CHANNEL_1:
          motor_ctx->channel_ccr = &motor_ctx->htim_pwm->Instance->CCR1;
          motor_ctx->htim_pwm->usrdata[0] = motor_ctx;
          break;
        case TIM_CHANNEL_2:
          motor_ctx->channel_ccr = &motor_ctx->htim_pwm->Instance->CCR2;
          motor_ctx->htim_pwm->usrdata[1] = motor_ctx;
          break;
        case TIM_CHANNEL_3:
          motor_ctx->channel_ccr = &motor_ctx->htim_pwm->Instance->CCR3;
          motor_ctx->htim_pwm->usrdata[2] = motor_ctx;
          break;
        case TIM_CHANNEL_4:
          motor_ctx->channel_ccr = &motor_ctx->htim_pwm->Instance->CCR4;
          motor_ctx->htim_pwm->usrdata[3] = motor_ctx;
          break;
        case TIM_CHANNEL_5:
          motor_ctx->channel_ccr = &motor_ctx->htim_pwm->Instance->CCR5;
          motor_ctx->htim_pwm->usrdata[4] = motor_ctx;
          break;
        case TIM_CHANNEL_6:
          motor_ctx->channel_ccr = &motor_ctx->htim_pwm->Instance->CCR6;
          motor_ctx->htim_pwm->usrdata[5] = motor_ctx;
          break;
        default:
          err = E_PARAM;
          break;
      }
      BREAK_IF(err != E_OK);
    }

  } while(0);

  return err;
}

error_t ecu_devices_motor_reconfigure(ecu_device_motor_t instance)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;
  //TODO: this is workaround function because L9960, for some reason, resets the config after HWSC/DIAGOFF

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    err = l9960_configure(motor_ctx->ctx, &motor_ctx->ctx->config);

  } while(0);

  return err;
}

error_t ecu_devices_motor_reset(ecu_device_motor_t instance)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    err = l9960_reset(motor_ctx->ctx);
    if(err != E_AGAIN) {
      if(motor_ctx->tim_started) {
        HAL_TIM_PWM_Stop_IT(motor_ctx->htim_pwm, motor_ctx->channel_pwm);
        motor_ctx->tim_started = false;
      }
    }

  } while(0);

  return err;
}

error_t ecu_devices_motor_hwsc(ecu_device_motor_t instance)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    err = l9960_hwsc(motor_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_devices_motor_diagoff(ecu_device_motor_t instance)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    err = l9960_diagoff(motor_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_devices_motor_get_diag(ecu_device_motor_t instance, l9960_diag_t *diag)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    err = l9960_get_diagnostic(motor_ctx->ctx, diag);

  } while(0);

  return err;
}

error_t ecu_devices_motor_set_enabled(ecu_device_motor_t instance, bool enabled)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    if(motor_ctx->tim_started != enabled) {
      err = l9960_set_enabled(motor_ctx->ctx, enabled);
      BREAK_IF(err != E_OK);

      if(enabled == false) {
        status = HAL_TIM_PWM_Stop_IT(motor_ctx->htim_pwm, motor_ctx->channel_pwm);
      } else {
        status = HAL_TIM_PWM_Start_IT(motor_ctx->htim_pwm, motor_ctx->channel_pwm);
      }
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

      motor_ctx->tim_started = enabled;
    }

  } while(0);

  return err;
}

error_t ecu_devices_motor_set_dutycycle(ecu_device_motor_t instance, float dutycycle)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;
  bool dir;
  float value;
  uint32_t arr, ccr;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    dir = dutycycle >= 0 ? true : false;

    arr = motor_ctx->htim_pwm->Instance->ARR;

    value = fabsf(dutycycle);
    value *= motor_ctx->htim_pwm->Instance->ARR;
    value = roundf(value);
    ccr = MIN(value, arr);

    motor_ctx->pulse_pwm = ccr;
    motor_ctx->pulse_dir = dir;

  } while(0);

  return err;
}

error_t ecu_devices_motor_set_frequency(ecu_device_motor_t instance, float frequency)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;
  uint32_t base_freq, arr, psc;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_MOTOR_MAX, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];

    if(motor_ctx->pwm_frequency != frequency) {

      err = ecu_config_get_tim_base_frequency(motor_ctx->htim_pwm, &base_freq);
      BREAK_IF(err != E_OK);

      psc = base_freq / 1000000;
      arr = roundf((float)1000000.0f / frequency);

      while((arr < ECU_CONFIG_MOTOR_PWM_DEPTH_MIN) && (psc > 1) && (arr < (USHRT_MAX / 2))) {
        arr *= 2;
        psc /= 2;
      }

      motor_ctx->htim_pwm->Instance->PSC = psc - 1;
      motor_ctx->htim_pwm->Instance->ARR = arr - 1;

      motor_ctx->pwm_frequency = frequency;
    }
  } while(0);

  return err;
}
