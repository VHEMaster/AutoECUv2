/*
 * config_maf.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_maf.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    maf_config_t config_default;
    maf_init_ctx_t init;
    maf_ctx_t *ctx;
}ecu_sensors_maf_ctx_t;

static const maf_config_t ecu_sensors_maf_config_default = {
    .signal_voltage_to_value = { // Parameters for sensor Siemens VDO 5WK9701 :
        .table = {
            .items = 256,
            .output = {
                -38.4f, -37.3f, -36.2f, -35.1f, -34.0f, -33.0f, -31.9f, -30.9f,
                -29.9f, -28.9f, -27.9f, -27.0f, -26.1f, -25.1f, -24.3f, -23.4f,
                -22.5f, -21.7f, -20.8f, -20.0f, -19.3f, -18.5f, -17.7f, -17.0f,
                -16.3f, -15.6f, -14.9f, -14.2f, -13.6f, -12.9f, -12.3f, -11.7f,
                -11.1f, -10.5f, -10.0f, -9.4f, -8.9f, -8.3f, -7.8f, -7.3f,
                -6.7f, -6.2f, -5.7f, -5.1f, -4.5f, -3.9f, -3.3f, -2.7f,
                -2.0f, -1.4f, -0.7f, 0.0f, 0.6f, 1.2f, 1.9f, 2.5f,
                3.1f, 3.7f, 4.4f, 5.0f, 5.6f, 6.2f, 6.8f, 7.3f,
                7.8f, 8.3f, 8.9f, 9.4f, 10.0f, 10.5f, 11.1f, 11.7f,
                12.3f, 12.9f, 13.6f, 14.2f, 14.9f, 15.6f, 16.3f, 17.0f,
                17.7f, 18.5f, 19.2f, 20.0f, 20.8f, 21.7f, 22.5f, 23.4f,
                24.2f, 25.1f, 26.1f, 27.0f, 27.9f, 28.9f, 29.9f, 30.9f,
                31.9f, 32.9f, 33.9f, 34.9f, 35.9f, 37.0f, 38.1f, 39.1f,
                40.2f, 41.4f, 42.5f, 43.6f, 44.7f, 46.0f, 47.2f, 48.5f,
                49.9f, 51.2f, 52.6f, 54.0f, 55.4f, 56.9f, 58.4f, 59.9f,
                61.5f, 63.1f, 64.7f, 66.4f, 68.0f, 69.7f, 71.5f, 73.3f,
                75.1f, 76.9f, 78.8f, 80.7f, 82.6f, 84.6f, 86.6f, 88.6f,
                90.6f, 92.7f, 94.9f, 97.0f, 99.2f, 101.5f, 103.7f, 106.0f,
                108.4f, 110.7f, 113.1f, 115.5f, 117.9f, 120.4f, 122.9f, 125.5f,
                128.2f, 130.9f, 133.6f, 136.4f, 139.2f, 142.1f, 145.1f, 148.1f,
                151.2f, 154.4f, 157.6f, 160.8f, 164.1f, 167.5f, 170.9f, 174.4f,
                177.9f, 181.5f, 185.2f, 188.9f, 192.6f, 196.5f, 200.4f, 204.3f,
                208.3f, 212.4f, 216.5f, 220.7f, 225.0f, 229.4f, 233.8f, 238.3f,
                242.9f, 247.5f, 252.2f, 257.0f, 261.9f, 266.9f, 271.9f, 277.0f,
                282.2f, 287.5f, 292.8f, 298.2f, 303.7f, 309.2f, 314.9f, 320.6f,
                326.4f, 332.3f, 338.3f, 344.4f, 350.5f, 356.7f, 363.0f, 369.4f,
                375.9f, 382.5f, 389.2f, 396.0f, 402.9f, 409.8f, 416.8f, 423.9f,
                431.1f, 438.3f, 445.7f, 453.3f, 461.0f, 468.8f, 476.7f, 484.7f,
                492.8f, 501.0f, 509.3f, 517.8f, 526.3f, 535.0f, 543.8f, 552.7f,
                561.6f, 570.7f, 580.0f, 589.3f, 598.7f, 608.3f, 617.9f, 627.7f,
                637.6f, 647.5f, 657.6f, 667.8f, 678.0f, 688.4f, 698.9f, 709.5f,
                720.2f, 731.1f, 742.2f, 753.4f, 764.8f, 776.3f, 787.9f, 799.7f,
            },
        },
        .input_low = 0.90f,
        .input_high = 4.35f,
        .input_step = 5.0f / 256,
    },
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_mode = MAF_SIGNAL_MODE_ANALOG,
    .calc_mode = MAF_CALC_MODE_TABLE_VALUE,
};

static const bool ecu_sensors_maf_enabled_default[ECU_SENSOR_MAF_MAX] = {
    true,
    false
};

static const ecu_gpio_input_pin_t ecu_sensors_maf_input_pin_default[ECU_SENSOR_MAF_MAX] = {
    ECU_IN_PORT1_PIN7,
    ECU_IN_NONE
};

static RAM_SECTION ecu_sensors_maf_ctx_t ecu_sensors_maf_ctx[ECU_SENSOR_MAF_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_maf_config_default,
    },
    {
      .init = {

      },
      .config_default = ecu_sensors_maf_config_default,
    },
};

error_t ecu_sensors_maf_init(ecu_sensor_maf_t instance, maf_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX || ctx == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];
    maf_ctx->ctx = ctx;

    maf_ctx->config_default.enabled = ecu_sensors_maf_enabled_default[instance];
    maf_ctx->config_default.input_pin = ecu_sensors_maf_input_pin_default[instance];

    err = maf_init(maf_ctx->ctx, &maf_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&maf_ctx->ctx->config, &maf_ctx->config_default, sizeof(maf_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_maf_get_default_config(ecu_sensor_maf_t instance, maf_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX || config == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    memcpy(config, &maf_ctx->config_default, sizeof(maf_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_maf_configure(ecu_sensor_maf_t instance, const maf_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX || config == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    err = maf_configure(maf_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_maf_reset(ecu_sensor_maf_t instance)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    err = maf_reset(maf_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_maf_get_value(ecu_sensor_maf_t instance, maf_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    err = maf_get_value(maf_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_maf_get_diag(ecu_sensor_maf_t instance, maf_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    err = maf_get_diag(maf_ctx->ctx, diag);

  } while(0);

  return err;
}
