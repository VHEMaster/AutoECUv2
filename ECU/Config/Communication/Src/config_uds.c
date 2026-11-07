/*
 * config_uds.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include <string.h>
#include "config_uds.h"
#include "compiler.h"

typedef struct ecu_comm_uds_ctx_tag ecu_comm_uds_ctx_t;

#ifdef CORE_CM4
static void ecu_comm_uds_error_callback(uds_ctx_t *ctx, uds_error_code_t code, void *userdata);
#endif /* CORE_CM4 */

typedef struct ecu_comm_uds_ctx_tag {
#ifdef CORE_CM7
    const uds_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    uds_init_ctx_t init;
    uds_ctx_t *ctx;
#endif /* CORE_CM4 */
}ecu_comm_uds_ctx_t;

#ifdef CORE_CM7
static const uds_config_t ecu_comm_uds_config_default = {

};

static const bool ecu_comm_uds_enabled_default[ECU_COMM_UDS_MAX] = {
    true,
    true,
    true
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_uds_ctx_t ecu_comm_uds_ctx[ECU_COMM_UDS_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = ecu_comm_uds_error_callback,
          .callback_userdata = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_uds_config_default,
#endif /* CORE_CM7 */
    },
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = ecu_comm_uds_error_callback,
          .callback_userdata = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_uds_config_default,
#endif /* CORE_CM7 */
    },
    {
#ifdef CORE_CM4
      .init = {
          .error_callback = ecu_comm_uds_error_callback,
          .callback_userdata = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_uds_config_default,
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_uds_init(ecu_comm_uds_t instance, uds_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX || ctx == NULL, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];
    uds_ctx->ctx = ctx;

    uds_ctx->init.callback_userdata = (void *)uds_ctx;

    err = uds_init(uds_ctx->ctx, &uds_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_UDS, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_uds_get_default_config(ecu_comm_uds_t instance, uds_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX || config == NULL, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];

    memcpy(config, uds_ctx->config_default, sizeof(uds_config_t));
    config->enabled = ecu_comm_uds_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_uds_configure(ecu_comm_uds_t instance, const uds_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX || config == NULL, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];

#ifdef CORE_CM7
    (void)uds_ctx;
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = uds_configure(uds_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_UDS, instance, uds_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_uds_reset(ecu_comm_uds_t instance)
{
  error_t err = E_OK;
  ecu_comm_uds_ctx_t *uds_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_UDS_MAX, err = E_PARAM);

    uds_ctx = &ecu_comm_uds_ctx[instance];

#ifdef CORE_CM7
    (void)uds_ctx;
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = uds_reset(uds_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM4
static void ecu_comm_uds_error_callback(uds_ctx_t *ctx, uds_error_code_t code, void *userdata)
{
  // TODO: IMPLEMENT
}
#endif /* CORE_CM4 */
