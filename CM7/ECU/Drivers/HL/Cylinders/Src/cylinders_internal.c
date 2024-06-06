/*
 * cylinders_internal.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include "cylinders_internal.h"
#include "compiler.h"
#include "math_fast.h"
#include <math.h>

error_t cylinders_internal_process(cylinders_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);


  } while(0);

  return err;
}
