/*
 * middlelayer_spi.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "middlelayer_spi.h"
#include "compiler.h"
#include "config_spi.h"

typedef struct {
    spi_t spi_if;
    spi_cfg_t spi_cfg;
}middlelayer_spi_if_t;

typedef struct {
    middlelayer_spi_if_t ifs[ECU_SPI_IF_MAX];
}middlelayer_spi_ctx_t;

static middlelayer_spi_ctx_t middlelayer_spi_ctx = {0};

static error_t middlelayer_spi_if_init(ecu_spi_if_enum_t interface)
{
  error_t err = E_OK;

  do {
    if(interface >= ECU_SPI_IF_MAX) {
      err = E_PARAM;
      break;
    }

    err = ecu_config_spi_get_if_cfg(&middlelayer_spi_ctx.ifs[interface].spi_cfg, interface);
    if(err != E_OK) {
      break;
    }

    err = ecu_config_spi_register_callbacks(&middlelayer_spi_ctx.ifs[interface].spi_cfg, interface);
    if(err != E_OK) {
      break;
    }

    err = spi_init(&middlelayer_spi_ctx.ifs[interface].spi_if, &middlelayer_spi_ctx.ifs[interface].spi_cfg);
    if(err != E_OK) {
      break;
    }

  } while(0);

  return err;
}

void middlelayer_spi_init(void)
{
  error_t err = E_OK;

  (void)err;

  memset(&middlelayer_spi_ctx, 0u, sizeof(middlelayer_spi_ctx));

  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {
    err = middlelayer_spi_if_init(i);
  }
}

void middlelayer_spi_loop_main(void)
{
  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {
    if(middlelayer_spi_ctx.ifs[i].spi_if.cfg.hspi != NULL) {
      spi_loop_main(&middlelayer_spi_ctx.ifs[i].spi_if);
    }
  }
}

void middlelayer_spi_loop_slow(void)
{
  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {
    if(middlelayer_spi_ctx.ifs[i].spi_if.cfg.hspi != NULL) {
      spi_loop_slow(&middlelayer_spi_ctx.ifs[i].spi_if);
    }
  }
}

void middlelayer_spi_loop_fast(void)
{
  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {
    if(middlelayer_spi_ctx.ifs[i].spi_if.cfg.hspi != NULL) {
      spi_loop_fast(&middlelayer_spi_ctx.ifs[i].spi_if);
    }
  }
}

ITCM_FUNC error_t middlelayer_spi_get_if(spi_t **spi_if, ecu_spi_if_enum_t interface)
{
  error_t err = E_OK;

  do {
    if(interface >= ECU_SPI_IF_MAX || spi_if == NULL) {
      err = E_PARAM;
      break;
    }

    *spi_if = &middlelayer_spi_ctx.ifs[interface].spi_if;

  } while(0);

  return err;
}
