/*
 * timing_internal.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include "timing_internal.h"
#include "compiler.h"
#include "math_fast.h"
#include <math.h>

error_t timing_internal_process(timing_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);


  } while(0);

  return err;
}
