/*
 * cj125_fsm.c
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#include "cj125_internal.h"
#include "cj125_fsm.h"
#include "cj125_reg.h"
#include "compiler.h"
#include "errors.h"
#include "time.h"

error_t cj125_fsm(cj125_ctx_t *ctx)
{
  error_t err;

  do {
    err = E_OK;

    switch(ctx->process_fsm) {
      case CJ125_PROCESS_RESET:
        //err = cj125_fsm_reset(ctx);
        break;
      case CJ125_PROCESS_DIAG:
        //err = cj125_fsm_diagoff(ctx);
        break;
      case CJ125_PROCESS_CONFIGURE:
        //err = cj125_fsm_configure(ctx);
        break;
      default:
        break;
    }

    if(err == E_OK) {
      if(++ctx->process_fsm >= CJ125_PROCESS_MAX) {
        ctx->process_fsm = 0;
        break;
      }
    }
  } while(0);

  return err;
}
