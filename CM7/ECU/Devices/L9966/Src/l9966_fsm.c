/*
 * l9966_fsm.c
 *
 *  Created on: Apr 8: 2024
 *      Author: VHEMaster
 */

#include "l9966_internal.h"
#include "l9966_fsm.h"
#include "l9966_reg.h"
#include "l9966_cfg_reg.h"
#include "compiler.h"
#include "errors.h"
#include "time.h"

static error_t l9966_fsm_reset(l9966_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  l9966_reg_soft_rst_cmd_t soft_rst_cmd = {0};
  l9966_reg_version_t version_payload = {0};

  while(true) {
    err = E_AGAIN;
    now = time_get_current_us();

    switch(ctx->reset_fsm_state) {
      case L9966_RESET_CONDITION:
        if(ctx->reset_request == true && ctx->reset_errcode == E_AGAIN) {
          ctx->sqncr_cmd_ready_mask = 0;
          if(gpio_valid(&ctx->init.nrst_pin)) {
            ctx->reset_fsm_state = L9966_RESET_HARD_RESET;
          } else {
            ctx->reset_fsm_state = L9966_RESET_SOFT_STEP_PREPARE;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9966_RESET_HARD_RESET:
        ctx->reset_action_timestamp = now;
        gpio_reset(&ctx->init.nrst_pin);
        ctx->reset_fsm_state = L9966_RESET_HARD_WAIT_RESET;
        break;
      case L9966_RESET_HARD_WAIT_RESET:
        if(time_diff(now, ctx->reset_action_timestamp) >= L9966_RESET_HARD_WAIT_RESET_US) {
          ctx->reset_fsm_state = L9966_RESET_HARD_SET;
          continue;
        }
        break;
      case L9966_RESET_HARD_SET:
        ctx->reset_action_timestamp = now;
        gpio_set(&ctx->init.nrst_pin);
        ctx->reset_fsm_state = L9966_RESET_HARD_WAIT_SET;
        break;
      case L9966_RESET_HARD_WAIT_SET:
        if(time_diff(now, ctx->reset_action_timestamp) >= L9966_RESET_HARD_WAIT_SET_US) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_STEP_PREPARE;
          continue;
        }
        break;
      case L9966_RESET_SOFT_STEP_PREPARE:
        ctx->reset_fsm_state = L9966_RESET_SOFT_STEP_1_REQ;
        soft_rst_cmd.bits.AC = L9966_CTRL_SOFT_RESET_CMD_FIRST;
        ctx->fsm_tx_payload = soft_rst_cmd.data;
        continue;
      case L9966_RESET_SOFT_STEP_1_REQ:
        err = l9966_reg_write(ctx, L9966_REG_SOFT_RST_CMD, ctx->fsm_tx_payload);
        if(err == E_OK) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_STEP_2_REQ;
          soft_rst_cmd.bits.AC = L9966_CTRL_SOFT_RESET_CMD_SECOND;
          ctx->fsm_tx_payload = soft_rst_cmd.data;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9966_RESET_CONDITION;
        }
        break;
      case L9966_RESET_SOFT_STEP_2_REQ:
        err = l9966_reg_write(ctx, L9966_REG_SOFT_RST_CMD, ctx->fsm_tx_payload);
        if(err == E_OK) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_STEP_3_REQ;
          soft_rst_cmd.bits.AC = L9966_CTRL_SOFT_RESET_CMD_THIRD;
          ctx->fsm_tx_payload = soft_rst_cmd.data;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9966_RESET_CONDITION;
        }
        break;
      case L9966_RESET_SOFT_STEP_3_REQ:
        err = l9966_reg_write(ctx, L9966_REG_SOFT_RST_CMD, ctx->fsm_tx_payload);
        if(err == E_OK) {
          ctx->reset_action_timestamp = now;
          ctx->reset_fsm_state = L9966_RESET_SOFT_WAIT;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9966_RESET_CONDITION;
        }
        break;
      case L9966_RESET_SOFT_WAIT:
        if(time_diff(now, ctx->reset_action_timestamp) >= L9966_RESET_SOFT_WAIT_US) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_VERSION_DEV_V_REQ;
          continue;
        }
        break;
      case L9966_RESET_SOFT_VERSION_DEV_V_REQ:
        err = l9966_reg_read(ctx, L9966_REG_DEV_V, &ctx->fsm_rx_payload);
        if(err == E_OK) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_VERSION_HW_REV_REQ;
          version_payload.data = ctx->fsm_rx_payload;
          ctx->version.dev_ver = version_payload.bits.Payload;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9966_RESET_CONDITION;
        }
        break;
      case L9966_RESET_SOFT_VERSION_HW_REV_REQ:
        err = l9966_reg_read(ctx, L9966_REG_HW_REV, &ctx->fsm_rx_payload);
        if(err == E_OK) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_VERSION_DEV_ID_REQ;
          version_payload.data = ctx->fsm_rx_payload;
          ctx->version.hw_rev = version_payload.bits.Payload;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9966_RESET_CONDITION;
        }
        break;
      case L9966_RESET_SOFT_VERSION_DEV_ID_REQ:
        err = l9966_reg_read(ctx, L9966_REG_DEV_ID, &ctx->fsm_rx_payload);
        if(err == E_OK) {
          ctx->reset_fsm_state = L9966_RESET_SOFT_VERSION_VERIFY;
          version_payload.data = ctx->fsm_rx_payload;
          ctx->version.dev_id = version_payload.bits.Payload;
          continue;
        } else if(err != E_AGAIN) {
          ctx->reset_errcode = err;
          ctx->reset_fsm_state = L9966_RESET_CONDITION;
        }
        break;
      case L9966_RESET_SOFT_VERSION_VERIFY:
        if(ctx->version.dev_id != L9966_DEV_ID) {
          ctx->reset_errcode = E_BADRESP;
        } else {
          ctx->reset_errcode = E_OK;
        }
        ctx->reset_fsm_state = L9966_RESET_CONDITION;
        ctx->sqncr_cmd_ready_mask = 0;
        ctx->version_valid = true;
        ctx->initialized = true;
        err = E_OK;
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_check_status(l9966_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  l9966_reg_gen_status_t status;

  while(true) {
    err = E_OK;

    switch(ctx->check_status_fsm_state) {
      case L9966_CHECK_STATUS_CONDITION:
        if(ctx->initialized) {
          now = time_get_current_us();
          if(ctx->status_valid == false || time_diff(now, ctx->status_check_last) >= L9966_STATUS_POLL_PERIOD) {
            ctx->check_status_fsm_state = L9966_CHECK_STATUS_SPI_REQ;
            err = E_AGAIN;
            continue;
          }
        }
        break;
      case L9966_CHECK_STATUS_SPI_REQ:
        err = l9966_reg_read(ctx, L9966_REG_GEN_STATUS, &status.data);
        if(err == E_OK) {
          now = time_get_current_us();
          ctx->status_check_last = now;
          ctx->status.config_check = status.bits.CONFIG_CHECK;
          ctx->status.calib_sel = status.bits.CALIB_SEL;
          ctx->status.calib_fault = status.bits.CALIB_FLT;
          ctx->status.trim_fault = status.bits.TRIM_FLT;
          ctx->status.overtemperature_mask = status.bits.OT_MASK;
          ctx->status.wakeup_event = status.bits.WAK_UP;
          ctx->status.supply_3v3_fault = status.bits.V33_FLT;
          ctx->status.overtemperature_fault = status.bits.OT_FLT;
          ctx->status_valid = true;
          ctx->check_status_fsm_state = L9966_CHECK_STATUS_CONDITION;
        } else if(err != E_OK) {
          ctx->status_valid = false;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_read_dig_in(l9966_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  l9966_reg_dig_in_stat_t status;

  while(true) {
    err = E_OK;

    switch(ctx->read_dig_in_fsm_state) {
      case L9966_READ_DIG_IN_CONDITION:
        if(ctx->initialized && ctx->configured) {
          now = time_get_current_us();
          if(ctx->digital_inputs_valid == false || time_diff(now, ctx->read_dig_in_last) >= ctx->config.digital_poll_period) {
            ctx->read_dig_in_fsm_state = L9966_READ_DIG_IN_SPI_REQ;
            err = E_AGAIN;
            continue;
          }
        }
        break;
      case L9966_READ_DIG_IN_SPI_REQ:
        err = l9966_reg_read(ctx, L9966_REG_DIN_IN_STAT, &status.data);
        if(err == E_OK) {
          now = time_get_current_us();
          ctx->read_dig_in_last = now;
          ctx->digital_inputs = status.bits.DIN_INx;
          ctx->digital_inputs_valid = true;
          ctx->read_dig_in_fsm_state = L9966_READ_DIG_IN_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_check_irq_flags(l9966_ctx_t *ctx)
{
  error_t err;
  bool int_pin_triggered = false;
  l9966_reg_sqncr_int_msk_flg_t status;

  while(true) {
    err = E_OK;

    switch(ctx->check_irq_flags_fsm_state) {
      case L9966_CHECK_IRQ_FLAGS_CONDITION:
        if(ctx->initialized && ctx->configured) {
          err = l9966_is_int_pin_triggered(ctx, &int_pin_triggered);
          if(err == E_OK) {
            if(int_pin_triggered == true) {
              ctx->check_irq_flags_fsm_state = L9966_CHECK_IRQ_FLAGS_SPI_REQ;
              err = E_AGAIN;
              continue;
            }
          }
        }
        break;
      case L9966_CHECK_IRQ_FLAGS_SPI_REQ:
        err = l9966_reg_read(ctx, L9966_REG_SQNCR_INT_MSK_FLG, &status.data);
        if(err == E_OK) {
          ctx->sc_int |= (status.bits.INTx & 1) != 0;
          for(int i = 0; i < L9966_EU_COUNT; i++) {
            ctx->eu_int[i] |= (status.bits.INTx & (1 << (i + 1))) != 0;
          }
          ctx->check_irq_flags_fsm_state = L9966_CHECK_IRQ_FLAGS_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_read_sqncr(l9966_ctx_t *ctx)
{
  error_t err;
  time_us_t now;
  l9966_reg_sqncr_rslt_copy_cmd_t status;
  l9966_reg_sqncr_result_voltage_t result_voltage;
  l9966_reg_sqncr_result_resistor_t result_resistor;
  float result_float, result_old, resistor, lpf;
  l9966_config_sqncr_cmd_pd_t rr_index;
  bool eu_enabled = false;
  bool data_available;
  bool copy_cplt = false;

  while(true) {
    err = E_OK;

    switch(ctx->read_sqncr_fsm_state) {
      case L9966_READ_SQNCR_CONDITION:
        if(ctx->initialized && ctx->configured) {
          for(int i = 0; i < L9966_EU_COUNT; i++) {
            if(ctx->eu_enabled[i] == true) {
              eu_enabled = true;
              break;
            }
          }
          if(eu_enabled == true) {
            for(int i = 0; i < L9966_EU_COUNT; i++) {
              if(ctx->eu_int[i] == true) {
                ctx->eu_int[i] = false;
                ctx->read_sqncr_fsm_state = L9966_READ_SQNCR_SPI_COPY_REQ;
              }
            }
            now = time_get_current_us();
            if(time_diff(now, ctx->sqncr_result_poll_last) >= L9966_SQNCR_RESULT_POLL_PERIOD) {
              ctx->read_sqncr_fsm_state = L9966_READ_SQNCR_SPI_COPY_REQ;
            }
            if(ctx->read_sqncr_fsm_state != L9966_READ_SQNCR_CONDITION) {
              if(ctx->config.config_data.sequencer_config.control.sync_copy_cmd_en) {
                ctx->sync_timestamp = now;
                if(gpio_valid(&ctx->init.sync_pin)) {
                  gpio_set(&ctx->init.sync_pin);
                }
              }
              err = E_AGAIN;
              continue;
            }
          }
        }
        break;
      case L9966_READ_SQNCR_SPI_COPY_REQ:
        copy_cplt = false;
        if(ctx->config.config_data.sequencer_config.control.sync_copy_cmd_en) {
          err = E_AGAIN;
          now = time_get_current_us();
          if(time_diff(now, ctx->sync_timestamp) >= L9966_SYNC_WAIT_US) {
            if(gpio_valid(&ctx->init.sync_pin)) {
              gpio_reset(&ctx->init.sync_pin);
            }
            copy_cplt = true;
          }
        } else {
          err = l9966_reg_read(ctx, L9966_REG_SQNCR_RSLT_COPY_CMD, &status.data);
          if(err == E_OK) {
            now = time_get_current_us();
            copy_cplt = true;
          }
        }
        if(copy_cplt) {
          ctx->sqncr_result_poll_last = now;

          ctx->read_sqncr_fsm_state = L9966_READ_SQNCR_SPI_READ_REQ;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9966_READ_SQNCR_SPI_READ_REQ:
        err = l9966_burst_reg_read(ctx, L9966_REG_SQNCR_RESULT_x, ctx->fsm_rx_burst_payload, L9966_CHANNELS);
        if(err == E_OK) {
          now = time_get_current_us();

          ctx->sqncr_diff = time_diff(now, ctx->sqncr_last);
          ctx->sqncr_last = now;

          for(int i = 0; i < L9966_CHANNELS; i++) {
            data_available = false;
            if(ctx->config.config_data.sequencer_config.cmd_config[i].r_volt_sel == L9966_CFG_SQNCR_CMD_RVM_RESISTANCE) {
              result_resistor.data = ctx->fsm_rx_burst_payload[i];
              if(result_resistor.bits.ADC_RESULT != 0x0000) {
                rr_index = ctx->config.config_data.sequencer_config.cmd_config[i].pu_div_sel;
                if(rr_index == L9966_CFG_SQNCR_CMD_PU_DISABLED) {
                  resistor = 1000000.0f;
                } else {
                  resistor = ctx->config.rrx[rr_index - L9966_CFG_SQNCR_CMD_PU_RR1];
                }
                result_float = (float)result_resistor.bits.ADC_RESULT * 0.00048828125f * resistor;
                data_available = true;
              }
            } else if(ctx->config.config_data.sequencer_config.cmd_config[i].r_volt_sel == L9966_CFG_SQNCR_CMD_RVM_VOLTAGE) {
              result_voltage.data = ctx->fsm_rx_burst_payload[i];
              if(result_voltage.bits.NEW_RESULT_FLAG) {
                result_float = result_voltage.bits.ADC_RESULT * 0.000244140625f;
                switch(i + 1) {
                  case L9966_CFG_SQNCR_PC_UBSW:
                    result_float *= L9966_DIV_INTERNAL_UBSW * 1.25f;
                    break;
                  case L9966_CFG_SQNCR_PC_VI5V:
                    result_float *= L9966_DIV_INTERNAL_VI5V * 1.25f;
                    break;
                  case L9966_CFG_SQNCR_PC_VIX:
                    result_float *= L9966_DIV_INTERNAL_VIX * 1.25f;
                    break;
                  default:
                    switch(ctx->config.config_data.sequencer_config.cmd_config[i].pu_div_sel) {
                      case L9966_CFG_SQNCR_CMD_DIV_5V:
                        result_float *= 5.0f;
                        break;
                      case L9966_CFG_SQNCR_CMD_DIV_20V:
                        result_float *= 20.0f;
                        break;
                      case L9966_CFG_SQNCR_CMD_DIV_40V:
                        result_float *= 40.0f;
                        break;
                      case L9966_CFG_SQNCR_CMD_DIV_1V25:
                        result_float *= 1.25f;
                      default:
                        break;
                    }
                    break;
                }
                data_available = true;
              }
            }

            if(data_available) {
              result_old = ctx->sqncr_cmd_results[i];
              lpf = ctx->config.config_data.sequencer_config.cmd_config[i].lpf;
              if(lpf < 1.0f) {
                result_float = result_float * lpf + result_old * (1.0f - lpf);
              } else {
                lpf = (ctx->sqncr_diff / (float)TIME_US_IN_MS) * lpf;
                if(lpf < 1.0f) {
                  result_float = result_float * lpf + result_old * (1.0f - lpf);
                }
              }
              ctx->sqncr_cmd_results[i] = result_float;
              ctx->sqncr_cmd_ready_mask |= 1 << i;
            }
          }
          ctx->read_sqncr_fsm_state = L9966_READ_SQNCR_CONDITION;
          err = E_AGAIN;
          continue;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_read_sc(l9966_ctx_t *ctx)
{
  error_t err;
  l9966_reg_sc_result_voltage_t result_voltage;
  l9966_reg_sc_result_resistor_t result_resistor;
  float result_float, resistor;
  l9966_ctrl_sc_pd_t rr_index;

  while(true) {
    err = E_OK;

    switch(ctx->read_sc_fsm_state) {
      case L9966_READ_SC_CONDITION:
        if(ctx->initialized && ctx->configured) {
          if(ctx->sc_int == true) {
            ctx->sc_int = false;
            ctx->read_sc_fsm_state = L9966_READ_SC_SPI_READ_REQ;
            err = E_AGAIN;
            continue;
          }
        }
        break;
      case L9966_READ_SC_SPI_READ_REQ:
        err = l9966_reg_read(ctx, L9966_REG_SC_RESULT, &ctx->fsm_rx_payload);
        if(err == E_OK) {
          result_float = 0.0f;
          if(ctx->sc_control.r_volt_sel == L9966_CTRL_SC_RVM_RESISTANCE) {
            result_resistor.data = ctx->fsm_rx_payload;
            rr_index = ctx->sc_control.pu_div_sel;
            if(rr_index == L9966_CTRL_SC_PU_DISABLED) {
              resistor = 1000000.0f;
            } else {
              resistor = ctx->config.rrx[rr_index - L9966_CTRL_SC_PU_RR1];
            }
            result_float = (float)result_resistor.bits.ADC_RESULT * 0.00048828125f * resistor;
          } else if(ctx->sc_control.r_volt_sel == L9966_CTRL_SC_RVM_VOLTAGE) {
            result_voltage.data = ctx->fsm_rx_payload;
            result_float = result_voltage.bits.ADC_RESULT * 0.000244140625f;
            switch(ctx->sc_control.adc_mux + 1) {
              case L9966_CFG_SQNCR_PC_UBSW:
                result_float *= L9966_DIV_INTERNAL_UBSW * 1.25f;
                break;
              case L9966_CFG_SQNCR_PC_VI5V:
                result_float *= L9966_DIV_INTERNAL_VI5V * 1.25f;
                break;
              case L9966_CFG_SQNCR_PC_VIX:
                result_float *= L9966_DIV_INTERNAL_VIX * 1.25f;
                break;
              default:
                switch(ctx->sc_control.pu_div_sel) {
                  case L9966_CFG_SQNCR_CMD_DIV_5V:
                    result_float *= 5.0f;
                    break;
                  case L9966_CFG_SQNCR_CMD_DIV_20V:
                    result_float *= 20.0f;
                    break;
                  case L9966_CFG_SQNCR_CMD_DIV_40V:
                    result_float *= 40.0f;
                    break;
                  case L9966_CFG_SQNCR_CMD_DIV_1V25:
                    result_float *= 1.25f;
                  default:
                    break;
                }
                break;
            }
          }

          ctx->sc_result = result_float;
          ctx->sc_ready = true;
          ctx->sc_enabled_accept = false;
          ctx->sc_enabled = false;
          ctx->read_sc_fsm_state = L9966_READ_SC_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_sc_maintain(l9966_ctx_t *ctx)
{
  error_t err;
  l9966_reg_sc_conf_t data;

  while(true) {
    err = E_OK;

    switch(ctx->sc_maintain_fsm_state) {
      case L9966_MAINTAIN_SC_CONDITION:
        if(ctx->initialized && ctx->configured) {
          if(ctx->sc_enabled == true && ctx->sc_enabled_accept == false) {
            ctx->sc_enabled = ctx->sc_enabled_accept;
            ctx->sc_int = false;
            ctx->sc_maintain_fsm_state = L9966_MAINTAIN_SC_SPI_WRITE_REQ;

            data.data = 0;
            data.bits.ADC_RUN = ctx->sc_enabled_accept == true;
            data.bits.ADC_MUX = ctx->sc_control.adc_mux;
            data.bits.PUP_DIV = ctx->sc_control.pu_div_sel;
            data.bits.R_VOLT_MEAS_SELECT = ctx->sc_control.r_volt_sel;
            ctx->fsm_tx_payload = data.data;

            err = E_AGAIN;
            continue;
          }
        }
        break;
      case L9966_MAINTAIN_SC_SPI_WRITE_REQ:
        err = l9966_reg_write(ctx, L9966_REG_SC_CONF, ctx->fsm_tx_payload);
        if(err == E_OK) {
          ctx->sc_maintain_fsm_state = L9966_MAINTAIN_SC_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_eu_maintain(l9966_ctx_t *ctx)
{
  error_t err;
  l9966_reg_sqncr_ctrl_t *data = &ctx->register_map.data.sqncr_ctrl;

  while(true) {
    err = E_OK;

    switch(ctx->eu_maintain_fsm_state) {
      case L9966_MAINTAIN_EU_CONDITION:
        if(ctx->initialized && ctx->configured) {
          for(int i = 0; i < L9966_EU_COUNT; i++) {
            if(ctx->eu_enabled[i] == true && ctx->eu_enabled_accept[i] == false) {
              ctx->eu_enabled_accept[i] = ctx->eu_enabled[i];
              ctx->eu_int[i] = false;
              ctx->eu_maintain_fsm_state = L9966_MAINTAIN_EU_SPI_WRITE_REQ;
            } else if(ctx->eu_enabled[i] == false && ctx->eu_enabled_accept[i] == true) {
              ctx->eu_enabled_accept[i] = ctx->eu_enabled[i];
              ctx->eu_maintain_fsm_state = L9966_MAINTAIN_EU_SPI_WRITE_REQ;
            }
          }
        }
        if(ctx->eu_maintain_fsm_state != L9966_MAINTAIN_EU_CONDITION) {
          data->bits.EU1_EN = ctx->eu_enabled_accept[0] && ctx->config.config_data.sequencer_config.control.sqncr_cfg[0].sync_pin_enabled == L9966_CFG_SQNCR_CONFIG_SYNC_DISABLED;
          data->bits.EU2_EN = ctx->eu_enabled_accept[1] && ctx->config.config_data.sequencer_config.control.sqncr_cfg[1].sync_pin_enabled == L9966_CFG_SQNCR_CONFIG_SYNC_DISABLED;

          data->bits.EU1_SYNC_EN = ctx->eu_enabled_accept[0] && ctx->config.config_data.sequencer_config.control.sqncr_cfg[0].sync_pin_enabled == L9966_CFG_SQNCR_CONFIG_SYNC_ENABLED;
          data->bits.EU2_SYNC_EN = ctx->eu_enabled_accept[1] && ctx->config.config_data.sequencer_config.control.sqncr_cfg[1].sync_pin_enabled == L9966_CFG_SQNCR_CONFIG_SYNC_ENABLED;

          data->bits.INIT_PC_EU1 = ctx->config.config_data.sequencer_config.control.sqncr_cfg[0].init_pc;
          data->bits.INIT_PC_EU2 = ctx->config.config_data.sequencer_config.control.sqncr_cfg[1].init_pc;

          data->bits.SYNC_COPY_CMD_EN = ctx->config.config_data.sequencer_config.control.sync_copy_cmd_en;
          ctx->fsm_tx_payload = data->data;

          err = E_AGAIN;
          continue;
        }
        break;
      case L9966_MAINTAIN_EU_SPI_WRITE_REQ:
        err = l9966_reg_write(ctx, L9966_REG_SQNCR_CTRL, ctx->fsm_tx_payload);
        if(err == E_OK) {
          ctx->eu_maintain_fsm_state = L9966_MAINTAIN_EU_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t l9966_fsm_configure(l9966_ctx_t *ctx)
{
  error_t err;
  bool eu_disabled;
  bool config_lost_check = (uint8_t)ctx->status.config_check != (uint8_t)ctx->config.config_data.status.config_check;

  while(true) {
    err = E_OK;

    switch(ctx->configure_fsm_state) {
      case L9966_CONFIGURE_CONDITION:
        if(ctx->initialized) {
          if((ctx->configure_request == true && ctx->configure_errcode == E_AGAIN) ||
              (ctx->configured == true && ((ctx->status_valid == true && ctx->status.config_check == L9966_CTRL_GS_CC_LOST) ||
              config_lost_check))) {
            if(config_lost_check) {
              ctx->configuration_lost_cnt++;
            }
            ctx->configure_fsm_state = L9966_CONFIGURE_EU_DISABLE;
            ctx->configure_errcode = E_AGAIN;
            err = E_AGAIN;
            continue;
          }
        }
        break;
      case L9966_CONFIGURE_EU_DISABLE:
        eu_disabled = true;
        for(int i = 0; i < L9966_EU_COUNT; i++) {
          if(ctx->eu_enabled[i] == true || ctx->eu_enabled_accept[i] == true) {
            ctx->eu_enabled[i] = false;
            eu_disabled = false;
          }
        }
        if(eu_disabled == true) {
          ctx->configure_fsm_state = L9966_CONFIGURE_SC_WAIT;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9966_CONFIGURE_SC_WAIT:
        if(ctx->sc_enabled == false && ctx->sc_enabled_accept == false) {
          ctx->configure_fsm_state = L9966_CONFIGURE_REG_TRANSLATE;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case L9966_CONFIGURE_REG_TRANSLATE:
        err = l9966_cfg_reg_translate(ctx);
        if(err == E_OK) {
          ctx->configure_fsm_state = L9966_CONFIGURE_CMD_DEFINE;
          ctx->configure_cmd_index = 0;
          err = E_AGAIN;
        } else if(err != E_AGAIN) {
          ctx->configure_errcode = err;
          err = E_OK;
        }
        break;
      case L9966_CONFIGURE_CMD_DEFINE:
        if(ctx->configure_cmd_index < ctx->register_map.raw_count) {
          ctx->fsm_tx_addr = ctx->register_map.raw[ctx->configure_cmd_index].addr;
          ctx->fsm_tx_payload = *ctx->register_map.raw[ctx->configure_cmd_index].value;
          ctx->configure_cmd_index++;
          ctx->configure_fsm_state = L9966_CONFIGURE_SPI_WRITE_REQ;
          err = E_AGAIN;
          continue;
        } else {
          ctx->configure_fsm_state = L9966_CONFIGURE_EU_RESTORE;
          err = E_AGAIN;
          continue;
        }
        break;
      case L9966_CONFIGURE_SPI_WRITE_REQ:
        err = l9966_reg_write(ctx, ctx->fsm_tx_addr, ctx->fsm_tx_payload);
        if(err == E_OK) {
          ctx->configure_fsm_state = L9966_CONFIGURE_CMD_DEFINE;
          err = E_AGAIN;
          continue;
        } else if(err != E_AGAIN) {
          ctx->configure_errcode = err;
          ctx->configure_fsm_state = L9966_CONFIGURE_CONDITION;
          err = E_OK;
        }
        break;
      case L9966_CONFIGURE_EU_RESTORE:
        for(int i = 0; i < L9966_EU_COUNT; i++) {
          ctx->eu_enabled[i] = ctx->config.eu_used[i];
        }
        ctx->configured = true;
        ctx->configure_errcode = E_OK;
        ctx->configure_fsm_state = L9966_CONFIGURE_CONDITION;
        err = E_OK;
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

error_t l9966_fsm(l9966_ctx_t *ctx)
{
  error_t err;

  while(true) {
    err = E_OK;

    switch(ctx->process_fsm) {
      case L9966_PROCESS_RESET:
        err = l9966_fsm_reset(ctx);
        break;
      case L9966_PROCESS_CHECK_STATUS:
        err = l9966_fsm_check_status(ctx);
        break;
      case L9966_PROCESS_CONFIGURE:
        err = l9966_fsm_configure(ctx);
        break;
      case L9966_PROCESS_CHECK_IRQ_FLAGS:
        err = l9966_fsm_check_irq_flags(ctx);
        break;
      case L9966_PROCESS_READ_SC:
        err = l9966_fsm_read_sc(ctx);
        break;
      case L9966_PROCESS_READ_SQNCR:
        err = l9966_fsm_read_sqncr(ctx);
        break;
      case L9966_PROCESS_READ_DIG_IN:
        err = l9966_fsm_read_dig_in(ctx);
        break;
      case L9966_PROCESS_SC_MAINTAIN:
        err = l9966_fsm_sc_maintain(ctx);
        break;
      case L9966_PROCESS_EU_MAINTAIN:
        err = l9966_fsm_eu_maintain(ctx);
        break;
      default:
        break;
    }

    if(err == E_OK) {
      if(++ctx->process_fsm >= L9966_PROCESS_MAX) {
        ctx->process_fsm = 0;
        break;
      }
    } else {
      break;
    }
  }

  return err;
}
