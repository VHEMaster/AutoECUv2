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
    spi_slave_t spi_slave;
    ecu_spi_slave_cfg_t cfg;
}middlelayer_spi_slave_t;

typedef struct {
    spi_t spi_if;
    spi_cfg_t spi_cfg;
    middlelayer_spi_slave_t slaves[ECU_SPI_IFS_SLAVE_MAX];
}middlelayer_spi_if_t;

typedef struct {
    middlelayer_spi_if_t ifs[ECU_SPI_IF_MAX];
    middlelayer_spi_slave_t *slaves[ECU_SPI_SLAVE_MAX];
}middlelayer_spi_ctx_t;

static middlelayer_spi_ctx_t middlelayer_spi_ctx = {0};

static error_t middlelayer_spi_if_init(ecu_spi_if_enum_t interface)
{
  error_t err = E_OK;
  middlelayer_spi_if_t *if_ctx = NULL;

  do {
    if(interface >= ECU_SPI_IF_MAX) {
      err = E_PARAM;
      break;
    }

    if_ctx = &middlelayer_spi_ctx.ifs[interface];

    err = ecu_config_spi_init();
    if(err != E_OK) {
      break;
    }

    err = ecu_config_spi_get_if_cfg(&if_ctx->spi_cfg, interface);
    if(err != E_OK) {
      break;
    }

    err = ecu_config_spi_register_callbacks(&if_ctx->spi_cfg, interface);
    if(err != E_OK) {
      break;
    }

    err = spi_init(&if_ctx->spi_if, &if_ctx->spi_cfg);
    if(err != E_OK) {
      break;
    }

  } while(0);

  return err;
}

static error_t middlelayer_spi_slaves_init(ecu_spi_if_enum_t interface)
{
  error_t err = E_OK;
  ecu_spi_slave_enum_t slave_enum;
  middlelayer_spi_if_t *if_ctx = NULL;

  do {
    if(interface >= ECU_SPI_IF_MAX) {
      err = E_PARAM;
      break;
    }

    if_ctx = &middlelayer_spi_ctx.ifs[interface];

    for(int i = 0; i < ECU_SPI_IFS_SLAVE_MAX; i++) {
      err = ecu_config_spi_get_slave_cfg(&if_ctx->slaves[i].cfg, interface, i);
      if(err == E_NOTSUPPORT) {
        err = E_OK;
        break;
      } else if(err != E_OK) {
        break;
      }

      err = spi_slave_init(&if_ctx->spi_if, &if_ctx->slaves[i].spi_slave,
          &if_ctx->slaves[i].cfg.nss_pin);
      if(err != E_OK) {
        break;
      }

      slave_enum = if_ctx->slaves[i].cfg.slave_enum;
      middlelayer_spi_ctx.slaves[slave_enum] = &if_ctx->slaves[i];
    }

    if(err != E_OK) {
      break;
    }

  } while(0);

  return err;
}

void middlelayer_spi_init(void)
{
  error_t err = E_OK;

  //TODO: Create DTC of SPI init
  (void)err;

  memset(&middlelayer_spi_ctx, 0u, sizeof(middlelayer_spi_ctx));

  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {
    err = middlelayer_spi_if_init(i);
    if(err == E_OK) {
      err = middlelayer_spi_slaves_init(i);
    }
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

ITCM_FUNC void middlelayer_spi_loop_fast(void)
{
  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {
    if(middlelayer_spi_ctx.ifs[i].spi_if.cfg.hspi != NULL) {
      spi_loop_fast(&middlelayer_spi_ctx.ifs[i].spi_if);
    }
  }
}

error_t middlelayer_spi_get_if(spi_t **spi_if, ecu_spi_if_enum_t interface)
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

error_t middlelayer_spi_get_slave(spi_slave_t **spi_slave, ecu_spi_slave_enum_t slave)
{
  error_t err = E_OK;

  do {
    if(slave >= ECU_SPI_SLAVE_MAX || spi_slave == NULL) {
      err = E_PARAM;
      break;
    }

    if(middlelayer_spi_ctx.slaves[slave] == NULL) {
      err = E_NOTSUPPORT;
    } else {
      *spi_slave = &middlelayer_spi_ctx.slaves[slave]->spi_slave;
    }

  } while(0);

  return err;
}
