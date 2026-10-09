/*
 * cmp_signal_patterned.c
 *
 * Sliding window CMP pattern matching against both possible CKP revolutions.
 * All angular units are crankshaft degrees, covering a 720-degree cycle.
 */
#include "cmp_signal_patterned.h"
#include "compiler.h"
#include "errors.h"
#include "time.h"
#include <string.h>
#include <math.h>

#define CMP_PATTERNED_HISTORY_MAX       CMP_CONFIG_PATTERNED_EDGES_MAX
#define CMP_PATTERNED_MIN_EDGES         2

typedef struct {
    float position;
    bool odd_rev;
    bool rising;
    time_us_t timestamp;
}cmp_signal_patterned_event_t;

typedef struct {
    cmp_signal_patterned_event_t history[CMP_PATTERNED_HISTORY_MAX];
    uint8_t count;
    uint8_t next;
    bool level_known;
    bool last_rising;
    bool ckp_synced;
    bool matched;
    bool match_phase;
    uint8_t match_index;
}cmp_signal_patterned_ctx_t;

static cmp_signal_patterned_ctx_t cmp_signal_patterned_ctx[CMP_INSTANCE_MAX];

static float cmp_patterned_wrap(float angle, float period)
{
  while(angle < 0.0f) {
    angle += period;
  }
  while(angle >= period) {
    angle -= period;
  }
  return angle;
}

static float cmp_patterned_delta(float angle)
{
  return cmp_patterned_wrap(angle + 360.0f, 720.0f) - 360.0f;
}

static bool cmp_patterned_cfg_valid(const cmp_config_signal_ref_type_patterned_t *cfg)
{
  if(cfg->edges_count < CMP_PATTERNED_MIN_EDGES ||
      cfg->edges_count > CMP_CONFIG_PATTERNED_EDGES_MAX ||
      !isfinite(cfg->reference_offset) ||
      !isfinite(cfg->vvt_min) ||
      !isfinite(cfg->vvt_max) ||
      !isfinite(cfg->angle_tolerance) ||
      !isfinite(cfg->interval_tolerance) ||
      !isfinite(cfg->vvt_slew_rate) ||
      cfg->vvt_min > cfg->vvt_max ||
      cfg->angle_tolerance <= 0.0f ||
      cfg->angle_tolerance >= 90.0f ||
      cfg->interval_tolerance <= 0.0f ||
      cfg->interval_tolerance >= 90.0f ||
      cfg->vvt_slew_rate < 0.0f) {
    return false;
  }

  for(uint8_t i = 0; i < cfg->edges_count; i++) {
    if(!isfinite(cfg->edges[i].angle) ||
        cfg->edges[i].angle < 0.0f ||
        cfg->edges[i].angle >= 720.0f) {
      return false;
    }
    if(i && (cfg->edges[i].angle <= cfg->edges[i - 1].angle ||
        cfg->edges[i].rising == cfg->edges[i - 1].rising)) {
      return false;
    }
  }

  return cfg->edges[0].rising != cfg->edges[cfg->edges_count - 1].rising;
}

static cmp_signal_patterned_event_t *cmp_patterned_history(
    cmp_signal_patterned_ctx_t *state, uint8_t from_newest)
{
  uint8_t index = (uint8_t)((state->next +
      CMP_PATTERNED_HISTORY_MAX - 1u - from_newest) % CMP_PATTERNED_HISTORY_MAX);
  return &state->history[index];
}

static float cmp_patterned_event_angle(const cmp_signal_patterned_event_t *event)
{
  /* CKP uses [-180, +180) and flips odd_rev at the -180 boundary.
   * Shift this coordinate by 180 before combining it with the revolution bit.
   * A simple wrap(position, 360) silently assigns negative CKP angles to
   * the wrong 360-degree half of the 720-degree engine cycle.
   */
  return cmp_patterned_wrap(event->position + 180.0f, 360.0f) +
      (event->odd_rev ? 360.0f : 0.0f);
}

static bool cmp_patterned_match(const cmp_config_signal_ref_type_patterned_t *cfg,
    cmp_signal_patterned_ctx_t *state, uint8_t newest_index,
    uint8_t phase, float *vvt)
{
  cmp_signal_patterned_event_t *latest = cmp_patterned_history(state, 0);
  float latest_expected = cfg->edges[newest_index].angle +
      cfg->reference_offset + 180.0f + (phase ? 360.0f : 0.0f);
  float latest_position = cmp_patterned_event_angle(latest);
  float fitted_vvt = cmp_patterned_delta(latest_position - latest_expected);
  float previous_actual = latest_position;
  float previous_expected = cfg->edges[newest_index].angle;
  time_us_t previous_time = latest->timestamp;

  /* The configured VVT limits are diagnostic, not matching limits.
   * Only +/-180 crank degrees is needed to disambiguate the two
   * CKP-revolution hypotheses. A shift outside that range cannot be
   * distinguished from the opposite revolution without another reference.
   */
  if(fitted_vvt <= -180.0f || fitted_vvt >= 180.0f) {
    return false;
  }

  for(uint8_t n = 0; n < state->count; n++) {
    cmp_signal_patterned_event_t *event = cmp_patterned_history(state, n);
    uint8_t index = (uint8_t)((newest_index + cfg->edges_count -
        (n % cfg->edges_count)) % cfg->edges_count);
    float expected = cfg->edges[index].angle +
        cfg->reference_offset + 180.0f + (phase ? 360.0f : 0.0f);
    float actual = cmp_patterned_event_angle(event);
    float residual = cmp_patterned_delta(actual - expected);
    float elapsed = (float)time_diff(latest->timestamp, event->timestamp) * 0.000001f;
    float allowance = cfg->angle_tolerance + cfg->vvt_slew_rate * elapsed;

    if(event->rising != cfg->edges[index].rising ||
        fabsf(cmp_patterned_delta(residual - fitted_vvt)) > allowance) {
      return false;
    }

    if(n != 0) {
      float observed_interval = cmp_patterned_wrap(previous_actual - actual, 720.0f);
      float expected_interval = cmp_patterned_wrap(previous_expected -
          cfg->edges[index].angle, 720.0f);
      float interval_elapsed = (float)time_diff(previous_time, event->timestamp) * 0.000001f;
      if(fabsf(observed_interval - expected_interval) >
          cfg->interval_tolerance + cfg->vvt_slew_rate * interval_elapsed) {
        return false;
      }
    }

    previous_actual = actual;
    previous_expected = cfg->edges[index].angle;
    previous_time = event->timestamp;
  }

  *vvt = fitted_vvt;
  return true;
}

error_t cmp_signal_patterned_init(cmp_ctx_t *ctx, cmp_instance_t instance_index, void **usrdata)
{
  cmp_signal_patterned_ctx_t *state;

  if(ctx == NULL || usrdata == NULL || instance_index >= CMP_INSTANCE_MAX) {
    return E_PARAM;
  }
  if(!cmp_patterned_cfg_valid(&ctx->config.signal_ref_types_config.patterned)) {
    return E_PARAM;
  }

  state = &cmp_signal_patterned_ctx[instance_index];
  memset(state, 0, sizeof(*state));
  *usrdata = state;
  return E_OK;
}

OPTIMIZE_FAST
ITCM_FUNC void cmp_signal_patterned_signal(cmp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata)
{
  cmp_signal_patterned_ctx_t *state = (cmp_signal_patterned_ctx_t *)usrdata;
  const cmp_config_signal_ref_type_patterned_t *cfg;
  ckp_data_t ckp;
  cmp_data_t result;
  cmp_signal_patterned_event_t *event;
  uint8_t found = 0;
  bool found_phase = false;
  uint8_t found_index = 0;
  float found_vvt = 0.0f;
  uint32_t prim;

  if(ctx == NULL || state == NULL ||
      (level != ECU_IN_LEVEL_HIGH && level != ECU_IN_LEVEL_LOW)) {
    return;
  }

  cfg = &ctx->config.signal_ref_types_config.patterned;
  bool rising = level == ECU_IN_LEVEL_HIGH;

  if(state->level_known && state->last_rising == rising) {
    ctx->diag.bits.signal_sequence = true;
    if(ctx->config.desync_on_error) {
      state->count = 0;
      state->matched = false;
      ctx->data.validity = CMP_DATA_NONE;
    }
    return;
  }
  state->level_known = true;
  state->last_rising = rising;

  if(ctx->init.ckp_update_req_cb == NULL ||
      ctx->init.ckp_update_req_cb(ctx->init.ckp_update_usrdata, NULL, &ckp) != E_OK ||
      ckp.validity < CKP_DATA_VALID) {
    state->count = 0;
    state->matched = false;
    ctx->data.validity = CMP_DATA_NONE;
    ctx->diag.bits.position_out_of_range = false;
    return;
  }

  event = &state->history[state->next];
  event->position = ckp.current_position;
  event->odd_rev = ckp.odd_rev;
  event->rising = rising;
  event->timestamp = time_now_us();
  state->next = (uint8_t)((state->next + 1u) % CMP_PATTERNED_HISTORY_MAX);
  if(state->count < cfg->edges_count) {
    state->count++;
  }

  if(state->count < CMP_PATTERNED_MIN_EDGES) {
    ctx->data.validity = CMP_DATA_DETECTED;
    return;
  }

  /* Search all circular template windows and both CKP-revolution hypotheses. */
  for(uint8_t phase = 0; phase < 2; phase++) {
    for(uint8_t index = 0; index < cfg->edges_count; index++) {
      float vvt;
      if(cmp_patterned_match(cfg, state, index, phase, &vvt)) {
        found++;
        found_phase = phase != 0;
        found_index = index;
        found_vvt = vvt;
      }
    }
  }

  prim = EnterCritical();
  result = ctx->data;
  if(found == 1) {
    if(state->matched && state->match_phase != found_phase) {
      ctx->diag.bits.wrong_signal = true;
      if(ctx->config.desync_on_error) {
        result.validity = CMP_DATA_DETECTED;
        state->matched = false;
        state->count = 0;
      }
    } else {
      result.validity = CMP_DATA_VALID;
      result.sync_at_odd_rev = found_phase;
      result.position = cmp_patterned_wrap(cfg->reference_offset + found_vvt + 180.0f, 360.0f) - 180.0f;
      /* Preserve the measured phase and synchronization even when the
       * camshaft is outside the configured permitted operating range. */
      ctx->diag.bits.position_out_of_range =
          (found_vvt < cfg->vvt_min || found_vvt > cfg->vvt_max);
      state->matched = true;
      state->match_phase = found_phase;
      state->match_index = found_index;
    }
  } else {
    if(found == 0) {
      ctx->diag.bits.wrong_signal = true;
    }
    /* No unique full-cycle match: never retain an obsolete VALID result. */
    result.validity = CMP_DATA_DETECTED;
    ctx->diag.bits.position_out_of_range = false;
    state->matched = false;
    if(found == 0) {
      state->count = 0;
    }
  }
  ctx->data = result;
  ExitCritical(prim);
}

void cmp_signal_patterned_loop_main(cmp_ctx_t *ctx, void *usrdata)
{
  (void)ctx;
  (void)usrdata;
}

void cmp_signal_patterned_loop_slow(cmp_ctx_t *ctx, void *usrdata)
{
  (void)ctx;
  (void)usrdata;
}

ITCM_FUNC void cmp_signal_patterned_loop_fast(cmp_ctx_t *ctx, void *usrdata)
{
  (void)ctx;
  (void)usrdata;
}

OPTIMIZE_FAST
ITCM_FUNC void cmp_signal_patterned_ckp_update(cmp_ctx_t *ctx, void *usrdata,
    const ckp_data_t *data, const ckp_diag_t *diag)
{
  cmp_signal_patterned_ctx_t *state = (cmp_signal_patterned_ctx_t *)usrdata;
  (void)diag;

  if(ctx == NULL || state == NULL || data == NULL) {
    return;
  }

  if(data->validity < CKP_DATA_VALID) {
    state->count = 0;
    state->next = 0;
    state->level_known = false;
    state->matched = false;
    ctx->data.validity = CMP_DATA_NONE;
    ctx->diag.bits.position_out_of_range = false;
  }
}
