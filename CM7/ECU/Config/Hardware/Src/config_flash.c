/*
 * config_flash.c
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_flash.h"
#include "config_extern.h"
#include "config_loop.h"
#include "compiler.h"

#include "flash_memory_layout.h"

typedef struct {
    qspi_ctx_t *ctx;
    qspi_init_t init;
}ecu_devices_flash_ctx_t;

static ecu_devices_flash_ctx_t ecu_devices_flash_ctx[ECU_DEVICE_FLASH_MAX] = {
    {
        .init = {
            .hqspi = &hqspi,
            .memory_base_address = QSPI_BASE,

            .flash_size = ECU_FLASH_SIZE,
            .flash_dies_count = ECU_FLASH_DIES_COUNT,
            .erase_sector_size = ECU_FLASH_SECTOR_SIZE,
            .erase_sectors_number = ECU_FLASH_SIZE / ECU_FLASH_SECTOR_SIZE,
            .prog_page_size = ECU_FLASH_PAGE_SIZE,
            .prog_pages_number = ECU_FLASH_SIZE / ECU_FLASH_PAGE_SIZE,
            .otp_size = 2048,
            .prot_size = 18,

            .delay_program_time = 1.5 * TIME_US_IN_MS,
            .delay_sector_erase = 25 * TIME_US_IN_MS,
            .delay_chip_erase = 50 * TIME_US_IN_MS,

            .timeout_program_time = 1 * TIME_US_IN_MS,
            .timeout_sector_erase = 5 * TIME_US_IN_MS,
            .timeout_chip_erase = 5 * TIME_US_IN_MS,

            .expected_jedec = {
                .mfg_id = 0xBF,
                .device_type = 0x26,
                .device_id = 0x42,
            },

            .cmd_rsten = {
                .Instruction = 0x66,
                .InstructionMode = QSPI_INSTRUCTION_1_LINE,
            },
            .cmd_rst = {
                .Instruction = 0x99,
                .InstructionMode = QSPI_INSTRUCTION_1_LINE,
            },
            .cmd_eqio = {
                .Instruction = 0x38,
                .InstructionMode = QSPI_INSTRUCTION_1_LINE,
            },
            .cmd_rstqios = {
                .Instruction = 0xFF,
                .InstructionMode = QSPI_INSTRUCTION_1_LINE,
            },
            .cmd_rstqioq = {
                .Instruction = 0xFF,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },
            .cmd_rdsr = {
                .Instruction = 0x05,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .DummyCycles = 2,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 1 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_wrsr = {
                .Instruction = 0x01,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .DummyCycles = 0,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 2 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_rdcr = {
                .Instruction = 0x35,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .DummyCycles = 2,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 1 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_hsread = {
                .Instruction = 0x0B,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .Address = 0x00,
                .AddressSize = 3,
                .AddressMode = QSPI_ADDRESS_4_LINES,
                .DummyCycles = 8,
                .AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 0 * ECU_FLASH_DIES_COUNT,
            },

            .cmd_jid = {
                .Instruction = 0x9F,
                .InstructionMode = QSPI_INSTRUCTION_1_LINE,
                .DataMode = QSPI_DATA_1_LINE,
                .NbData = 3 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_qjid = {
                .Instruction = 0xAF,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .DummyCycles = 2,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 3 * ECU_FLASH_DIES_COUNT,
            },

            .cmd_wren = {
                .Instruction = 0x06,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },
            .cmd_wrdi = {
                .Instruction = 0x04,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },
            .cmd_se = {
                .Instruction = 0x20,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .Address = 0x00,
                .AddressSize = 3,
                .AddressMode = QSPI_ADDRESS_4_LINES,
            },
            .cmd_be = {
                .Instruction = 0xD8,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .Address = 0x00,
                .AddressSize = 3,
                .AddressMode = QSPI_ADDRESS_4_LINES,
            },
            .cmd_ce = {
                .Instruction = 0xC7,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },
            .cmd_prog = {
                .Instruction = 0x02,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .Address = 0x00,
                .AddressSize = 3,
                .AddressMode = QSPI_ADDRESS_4_LINES,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 0 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_wrsu = {
                .Instruction = 0xB0,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },
            .cmd_wrre = {
                .Instruction = 0x30,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },

            .cmd_rbpr = {
                .Instruction = 0x72,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .DummyCycles = 2,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 18 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_wbpr = {
                .Instruction = 0x42,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 18 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_lbpr = {
                .Instruction = 0x8D,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },

            .cmd_rsid = {
                .Instruction = 0x88,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .Address = 0x00,
                .AddressSize = 2,
                .AddressMode = QSPI_ADDRESS_4_LINES,
                .DummyCycles = 6,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 0 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_psid = {
                .Instruction = 0xA5,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
                .Address = 0x00,
                .AddressSize = 2,
                .AddressMode = QSPI_ADDRESS_4_LINES,
                .DataMode = QSPI_DATA_4_LINES,
                .NbData = 0 * ECU_FLASH_DIES_COUNT,
            },
            .cmd_lsid = {
                .Instruction = 0x85,
                .InstructionMode = QSPI_INSTRUCTION_4_LINES,
            },
        },
    },
};


static void ecu_devices_flash_cplt_cb(QSPI_HandleTypeDef *hqspi)
{
  ecu_devices_flash_ctx_t *flash_ctx;

  for(int i = 0; i < ECU_DEVICE_FLASH_MAX; i++) {
    flash_ctx = &ecu_devices_flash_ctx[i];

    if(flash_ctx->init.hqspi == hqspi) {
      qspi_cplt_cb(flash_ctx->ctx);
      break;
    }
  }
}

static void ecu_devices_flash_err_cb(QSPI_HandleTypeDef *hqspi)
{
  ecu_devices_flash_ctx_t *flash_ctx;

  for(int i = 0; i < ECU_DEVICE_FLASH_MAX; i++) {
    flash_ctx = &ecu_devices_flash_ctx[i];

    if(flash_ctx->init.hqspi == hqspi) {
      qspi_err_cb(flash_ctx->ctx);
      break;
    }
  }
}

static void ecu_devices_flash_timeout_cb(QSPI_HandleTypeDef *hqspi)
{
  ecu_devices_flash_ctx_t *flash_ctx;

  for(int i = 0; i < ECU_DEVICE_FLASH_MAX; i++) {
    flash_ctx = &ecu_devices_flash_ctx[i];

    if(flash_ctx->init.hqspi == hqspi) {
      qspi_timeout_cb(flash_ctx->ctx);
      break;
    }
  }
}

error_t ecu_devices_flash_init(ecu_device_flash_t instance, qspi_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_flash_ctx_t *flash_ctx;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_FLASH_MAX || ctx == NULL, err = E_PARAM);

    flash_ctx = &ecu_devices_flash_ctx[instance];
    flash_ctx->ctx = ctx;

    status = HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_CMD_CPLT_CB_ID, ecu_devices_flash_cplt_cb);
    status |= HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_TX_CPLT_CB_ID, ecu_devices_flash_cplt_cb);
    status |= HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_RX_CPLT_CB_ID, ecu_devices_flash_cplt_cb);
    status |= HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_STATUS_MATCH_CB_ID, ecu_devices_flash_cplt_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_ERROR_CB_ID, ecu_devices_flash_err_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_ABORT_CB_ID, ecu_devices_flash_timeout_cb);
    status |= HAL_QSPI_RegisterCallback(flash_ctx->init.hqspi, HAL_QSPI_TIMEOUT_CB_ID, ecu_devices_flash_timeout_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    err = qspi_init(flash_ctx->ctx, &flash_ctx->init);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_flash_reset(ecu_device_flash_t instance)
{
  error_t err = E_OK;
  ecu_devices_flash_ctx_t *flash_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_FLASH_MAX, err = E_PARAM);

    flash_ctx = &ecu_devices_flash_ctx[instance];

    err = qspi_reset(flash_ctx->ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
