/*
 * tle6240.c
 *
 *  Created on: Apr 6, 2024
 *      Author: VHEMaster
 */

#include "tle6240.h"

error_t tle6240_init(tle6240_ctx_t *ctx, const tle6240_init_ctx_t *init_ctx)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

void tle6240_loop_main(tle6240_ctx_t *ctx)
{

}

void tle6240_loop_slow(tle6240_ctx_t *ctx)
{

}

void tle6240_loop_fast(tle6240_ctx_t *ctx)
{

}


error_t tle6240_write_all_masked(tle6240_ctx_t *ctx, uint16_t mask, uint16_t value)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle6240_write_all(tle6240_ctx_t *ctx, uint16_t value)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle6240_update(tle6240_ctx_t *ctx)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle6240_diagnostics(tle6240_ctx_t *ctx, tle6240_diag_t *diag)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle6240_set_poll_period(tle6240_ctx_t *ctx, time_delta_us_t period)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle6240_ch_write(tle6240_ctx_t *ctx, uint8_t channel, bool value)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

error_t tle6240_ch_read(tle6240_ctx_t *ctx, uint8_t channel, bool *value)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

bool tle6240_ch_is_serial(tle6240_ctx_t *ctx, uint8_t channel)
{
  error_t err = E_NOTSUPPORT;

  return err;
}

bool tle6240_ch_is_direct(tle6240_ctx_t *ctx, uint8_t channel)
{
  error_t err = E_NOTSUPPORT;

  return err;
}
