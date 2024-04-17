/*
 * config_motor.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_motor.h"
#include "config_extern.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    l9960_config_t config;
    l9960_init_ctx_t init;
    gpio_t dir_pin;
    TIM_HandleTypeDef *htim_pwm;
    uint32_t channel_pwm;
    l9960_ctx_t *ctx;
}ecu_devices_motor_ctx_t;

static const l9960_config_t ecu_devices_motor_config_default = {

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
      .config = ecu_devices_motor_config_default,
    },
    {
      .slave_index = ECU_SPI_SLAVE_OUTSM2,
      .init = {
          .dis_pin = { .port = MOTOR2_DIS_GPIO_Port, .pin = MOTOR2_DIS_Pin },
      },
      .dir_pin = { .port = MOTOR2_DIR_GPIO_Port, .pin = MOTOR2_DIR_Pin },
      .htim_pwm = &htim8,
      .channel_pwm = TIM_CHANNEL_3,
      .config = ecu_devices_motor_config_default,
    },
};

error_t ecu_devices_motor_init(ecu_device_motor_t instance, l9960_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_motor_ctx_t *motor_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_MOTOR_MAX || ctx == NULL, err = E_PARAM);

    motor_ctx = &ecu_devices_motor_ctx[instance];
    motor_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&motor_ctx->init.spi_slave, motor_ctx->slave_index);
    BREAK_IF(err != E_OK);

    if(gpio_valid(&motor_ctx->dir_pin)) {
      gpio_reset(&motor_ctx->dir_pin);
    }

    err = l9960_init(motor_ctx->ctx, &motor_ctx->init);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
