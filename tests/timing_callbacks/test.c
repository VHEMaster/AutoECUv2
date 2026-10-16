#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "timing_common.h"
#include "config_timings.h"
#include "config_hw.h"
#include "interpolation.h"
#include "defaults.h"

void reference_timing_position_clamp(float, bool, float *);
void reference_timing_base_signal_update_cb(void *, const timing_base_data_t *, const timing_base_diag_t *);
error_t reference_timing_calculate_offset_position(timing_base_ctx_t *, float, bool, timing_base_req_t *, timing_base_data_crankshaft_t *);
error_t reference_timing_calculate_current_position(timing_base_ctx_t *, float, bool, timing_base_req_t *, timing_base_data_crankshaft_t *);
sMathInterpolateInput reference_math_interpolate_input(float, const float *, uint32_t);
float reference_math_interpolate_1d(sMathInterpolateInput, const float *);
float reference_math_interpolate_1d_direct(float, const float *, const float *, uint32_t);

typedef struct {
  timing_base_ctx_t base;
  ignition_ctx_t ign;
  injection_ctx_t inj;
  cybalance_ctx_t balance;
  knockwindow_ctx_t knock;
} fixture_t;
static fixture_t fixtures[2], *active;
static ecu_config_engine_calibration_t calibration;
static pulsedadc_ctx_t adc[2];
static uint16_t samples[4096];
static volatile time_us_t clock_us = 1;
static unsigned critical_count, event_count, case_id, step_id;
static uint32_t rng = 1;
static unsigned next_random(void) { rng = rng * 1664525u + 1013904223u; return rng; }
static struct event { unsigned type, pin, start, end; } events[128], reference_events[128];
static void event(unsigned type, unsigned pin, unsigned start, unsigned end)
{
  assert(event_count < ITEMSOF(events));
  events[event_count++] = (struct event){type, pin, start, end};
}
static void equal(const void *a, const void *b, size_t size, const char *what)
{
  if(memcmp(a, b, size)) {
    fprintf(stderr, "Mismatch: %s, scenario %u, step %u size=%zu\n", what, case_id, step_id, size);
    unsigned differences = 0;
    for(size_t i = 0; i < size && differences < 16; i++) if(((const unsigned char *)a)[i] != ((const unsigned char *)b)[i]) {
      fprintf(stderr, " byte %zu: %02x/%02x\n", i, ((const unsigned char *)a)[i], ((const unsigned char *)b)[i]); differences++;
    }
    fprintf(stderr, "ign groups=%zu stride=%zu req_mean=%zu mean=%zu advance_cy=%zu\n", offsetof(ignition_runtime_ctx_t,groups), sizeof(ignition_runtime_group_ctx_t), offsetof(ignition_runtime_group_ctx_t,advance_requested_mean), offsetof(ignition_runtime_group_ctx_t,advance_mean), offsetof(ignition_runtime_group_ctx_t,advance_cy));
    abort();
  }
}
uint32_t EnterCritical(void) { critical_count++; return 0; }
void ExitCritical(uint32_t prim) { (void)prim; }
void ecu_config_set_ignition_enabled(bool enabled) { event(0, enabled, 0, 0); }
error_t ecu_config_gpio_output_write(ecu_gpio_output_pin_t pin, output_value_t value)
{ event(1, pin, value, 0); return E_OK; }
error_t timing_pulse_schedule(ecu_gpio_output_pin_t pin, time_us_t start, time_us_t end, timing_late_policy_t policy)
{ event(2 + policy, pin, start, end); return E_OK; }
error_t ecu_config_gpio_input_get_id(ecu_gpio_input_pin_t pin, input_id_t *id)
{ *id = 0; return E_OK; }
error_t input_get_value(input_id_t id, input_value_t *value, time_delta_us_t *time)
{ *value = (unsigned)(13.8f / INPUTS_ANALOG_MULTIPLIER_R); return E_OK; }
error_t ecu_timings_base_get_data_ptr(ecu_timing_base_t instance, const timing_base_data_t **data)
{ *data = &active->base.data; return E_OK; }
error_t ecu_timings_ignition_get_runtime_data_ptr(ecu_timing_ignition_t instance, ignition_runtime_ctx_t **data)
{ *data = &active->ign.runtime; return E_OK; }
error_t ecu_timings_get_ignition_ctx(ecu_timing_base_t instance, ignition_ctx_t **ctx)
{ *ctx = &active->ign; return E_OK; }
error_t ecu_timings_get_injection_ctx(ecu_timing_base_t instance, injection_ctx_t **ctx)
{ *ctx = &active->inj; return E_OK; }
error_t ecu_timings_get_cybalance_ctx(ecu_timing_base_t instance, cybalance_ctx_t **ctx)
{ *ctx = &active->balance; return E_OK; }
error_t ecu_timings_get_knockwindow_ctx(ecu_timing_base_t instance, knockwindow_ctx_t **ctx)
{ *ctx = &active->knock; return E_OK; }
error_t ecu_timings_get_timing_enabled(ecu_timing_type_t type, ecu_timing_instance_t instance, bool *enabled)
{
  switch(type) {
    case ECU_TIMING_TYPE_IGNITION: *enabled = active->ign.config.enabled; break;
    case ECU_TIMING_TYPE_INJECTION: *enabled = active->inj.config.enabled; break;
    case ECU_TIMING_TYPE_CYBALANCE: *enabled = active->balance.config.enabled; break;
    case ECU_TIMING_TYPE_KNOCKWINDOW: *enabled = active->knock.config.enabled; break;
    default: return E_PARAM;
  }
  return E_OK;
}
error_t ecu_sensors_map_get_value(ecu_sensor_map_t instance, map_data_t *data)
{
  memset(data, 0, sizeof(*data));
  if(case_id < 48) {
    data->valid = true;
    data->manifold_air_pressure = 0.7f;
    return E_OK;
  }
  event(8, instance, 0, 0);
  if((step_id + instance) % 113 == 0) return E_FAULT;
  data->valid = step_id % 97 != 0;
  data->manifold_air_pressure = instance < 2 ?
      0.2f + (step_id % 121) * 0.02f : 2.0f + (step_id % 131) * 0.03f;
  if(step_id % 7 != 0) data->manifold_air_pressure += (instance & 1) * 0.17f;
  return E_OK;
}
error_t pulsedadc_prepare(pulsedadc_ctx_t *ctx, uint16_t count)
{ event(4, ctx - adc, count, 0); return count > 4000 ? E_OVERFLOW : E_OK; }
error_t pulsedadc_start(pulsedadc_ctx_t *ctx)
{ event(5, ctx - adc, 0, 0); return E_OK; }
error_t pulsedadc_stop(pulsedadc_ctx_t *ctx)
{ event(6, ctx - adc, 0, 0); return E_OK; }
error_t pulsedadc_get_samples(pulsedadc_ctx_t *ctx, const uint16_t **buffer, uint16_t *count)
{ *buffer = samples; *count = 400; return E_OK; }
static void sampling_done(void *usrdata, const knockwindow_sampling_cplt_ctx_t *data)
{ event(7, data->cylinder, data->samples_count, data->setup_index); }

static void test_lookup(void)
{
  float x[32], y[32];
  unsigned count = 0;
  for(unsigned size = 1; size <= 32; size++) {
    for(unsigned pattern = 0; pattern < 4; pattern++) {
      x[0] = -10.0f;
      for(unsigned i = 0; i < size; i++) {
        if(i) x[i] = x[i-1] + (pattern == 1 ? 0 : (next_random() % 3));
        y[i] = ((int)(next_random() % 100000) - 50000) * 0.01f;
      }
      for(unsigned i = 0; i < 1000; i++) {
        float value = (int)(next_random() % 20000) * 0.01f - 60.0f;
        if(i < size) value = x[i];
        float old = reference_math_interpolate_1d_direct(value, x, y, size);
#ifdef HAVE_DIRECT_LOOKUP
        float new = math_interpolate_1d_direct(value, x, y, size);
#else
        float new = math_interpolate_1d(math_interpolate_input(value, x, size), y);
#endif
        if(memcmp(&old, &new, sizeof(old))) {
          fprintf(stderr, "lookup size=%u pattern=%u sample=%u input=%g old=%a new=%a\n", size, pattern, i, value, old, new);
        }
        equal(&old, &new, sizeof(old), "scalar lookup");
        count++;
      }
    }
  }
  printf("Scalar lookup: %u bit-identical cases (extrapolation, duplicate axes; finite inputs).\n", count);
}

static void test_angle_clamp(void)
{
  const float boundaries[] = {0.0f, -0.0f, 180.0f, -180.0f, 360.0f, -360.0f,
      720.0f, -720.0f, 360000.0f, -360000.0f};
  for(unsigned phased = 0; phased < 2; phased++) {
    for(unsigned i = 0; i < 100000; i++) {
      float value = i < ITEMSOF(boundaries) ? boundaries[i] :
          ((int)(next_random() % 2880000) - 1440000) * 0.001f;
      if(i < ITEMSOF(boundaries)) {
        for(int direction = -1; direction <= 1; direction++) {
          float adjacent = direction == 0 ? value : nextafterf(value, direction < 0 ? -INFINITY : INFINITY);
          float old, current;
          reference_timing_position_clamp(adjacent, phased, &old);
          timing_position_clamp(adjacent, phased, &current);
          equal(&old, &current, sizeof(old), "angle clamp boundary");
        }
      }
      float old, current;
      reference_timing_position_clamp(value, phased, &old);
      timing_position_clamp(value, phased, &current);
      equal(&old, &current, sizeof(old), "angle clamp");
    }
    timing_position_clamp(0.0f, phased, NULL);
  }
  puts("Angle clamp: 200000 finite cases plus adjacent boundaries, identical results.");
}

static void test_positions(void)
{
  timing_base_ctx_t old = {0}, current = {0};
  timing_base_req_t a = {0}, b = {0};
  timing_base_data_crankshaft_t da, db;
  old.init.calibration_config = current.init.calibration_config = &calibration;
  calibration.cylinders.cylinders_count = 8;
  for(unsigned cy = 0; cy < 8; cy++) calibration.cylinders.cylinders[cy].pos_offset = 90.0f * cy;
  for(step_id = 0; step_id < 20000; step_id++) {
    timing_base_data_crankshaft_t *source = &old.data.crankshaft;
    source->mode = (step_id / 317) % TIMING_CRANKSHAFT_MODE_MAX;
    source->valid = source->mode >= TIMING_CRANKSHAFT_MODE_VALID;
    source->sensor_data.current.position = ((int)(step_id % 120) - 60) * 6.0f;
    source->sensor_data.previous.position = source->sensor_data.current.position - 6.0f;
    source->sensor_data.current.timestamp = step_id * 100;
    source->sensor_data.previous.timestamp = source->sensor_data.current.timestamp - 100;
    old.config.phased_only = current.config.phased_only = (step_id / 997) & 1;
    current.data.crankshaft = *source;
    clock_us = source->sensor_data.current.timestamp + step_id % 50;
    float offset = ((int)(next_random() % 240) - 120) * 6.0f;
    bool phased = step_id & 1;
    if(step_id & 2) {
      reference_timing_calculate_current_position(&old, offset, phased, &a, &da);
      timing_calculate_current_position(&current, offset, phased, &b, &db);
    } else {
      reference_timing_calculate_offset_position(&old, offset, phased, &a, &da);
      timing_calculate_offset_position(&current, offset, phased, &b, &db);
    }
    equal(&a, &b, sizeof(a), "request history");
    equal(&da, &db, sizeof(da), "position data");
    for(unsigned mode = 1; mode <= 2; mode++) {
      for(unsigned cy = 0; cy < 8; cy++) {
        timing_base_cylinder_sequentialed_cylinder_ctx_t *dst = &old.data.sequentialed[mode].cylinders[cy];
        reference_timing_calculate_offset_position(&old, calibration.cylinders.cylinders[cy].pos_offset,
            mode == 1, &dst->timing_req, &dst->crankshaft_data);
      }
    }
    critical_count = 0;
#ifdef HAVE_BATCH_POSITIONS
    timing_calculate_cylinders_positions(&current, true, true);
    assert(critical_count == 1);
#else
    for(unsigned mode = 1; mode <= 2; mode++) {
      for(unsigned cy = 0; cy < 8; cy++) {
        timing_base_cylinder_sequentialed_cylinder_ctx_t *dst = &current.data.sequentialed[mode].cylinders[cy];
        timing_calculate_offset_position(&current, calibration.cylinders.cylinders[cy].pos_offset,
            mode == 1, &dst->timing_req, &dst->crankshaft_data);
      }
    }
    assert(critical_count == 16);
#endif
    equal(old.data.sequentialed, current.data.sequentialed, sizeof(old.data.sequentialed), "batch positions");
  }
#ifdef HAVE_BATCH_POSITIONS
  puts("Positions: 20000 steps; 8-cylinder batch uses one critical section.");
#else
  puts("Positions: 20000 steps; original 8-cylinder passes use 16 critical sections.");
#endif
}

static void init_fixture(unsigned cylinders, unsigned banks, unsigned scenario)
{
  memset(fixtures, 0, sizeof(fixtures));
  memset(&calibration, 0, sizeof(calibration));
  calibration.cylinders.cylinders_count = cylinders;
  calibration.cylinders.banks_count = banks;
  if(scenario >= 48) {
    for(unsigned type = 0; type < ECU_CONFIG_IO_MAP_MAX; type++) {
      unsigned first = type == ECU_CONFIG_IO_MAP_MANIFOLD ? 0 : 2;
      calibration.io.banked.global.common.sensor_map[type] = scenario & 4 ? ECU_SENSOR_MAP_MAX : first;
      for(unsigned b = 0; b < banks; b++)
        calibration.io.banked.banks[b].common.sensor_map[type] = first + b;
    }
  }
  for(unsigned cy = 0; cy < cylinders; cy++) {
    calibration.cylinders.cylinders[cy].pos_offset = 720.0f / cylinders * cy;
    calibration.cylinders.cylinders[cy].bank = cy % banks;
    calibration.cylinders.cylinders[cy].cy_opposed = (cy + cylinders / 2) % cylinders;
  }
  for(unsigned k = 0; k < 2; k++) {
    fixture_t *f = &fixtures[k];
    f->base.init.calibration_config = &calibration;
    f->base.ready = f->base.configured = f->base.config.enabled = true;
    f->base.config.phased_only = scenario & 1;
    f->ign.init.calibration_config = f->inj.init.calibration_config = f->balance.init.calibration_config = f->knock.init.calibration_config = &calibration;
    f->ign.config = ecu_timings_ignition_config_default;
    f->inj.config = ecu_timings_injection_config_default;
    f->balance.config = ecu_timings_cybalance_config_default;
    f->knock.config = ecu_timings_knockwindow_config_default;
    f->ign.ready = f->ign.configured = f->ign.config.enabled = true;
    f->inj.ready = f->inj.configured = f->inj.config.enabled = true;
    f->balance.ready = f->balance.configured = f->balance.config.enabled = true;
    f->knock.ready = f->knock.configured = f->knock.config.enabled = true;
    f->knock.init.callback = sampling_done;
    f->ign.config.use_ignition_acceptance = scenario & 2;
    f->ign.config.process_update_trigger = scenario % 3;
    f->inj.config.process_update_trigger = (scenario / 3) % 3;
    for(unsigned gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
      f->ign.config.groups[gr].enabled = scenario % 11 != 0 || gr == 0;
      f->ign.config.groups[gr].advance_slew_rate_mode = scenario % IGNITION_CONFIG_GROUP_ADVANCE_SLEW_RATE_MODE_MAX;
      f->ign.config.groups[gr].process_update_trigger = scenario % 4;
      f->ign.config.groups[gr].mode = scenario % IGNITION_CONFIG_GROUP_MODE_MAX;
      for(unsigned cy = 0; cy < cylinders; cy++) {
        f->ign.config.groups[gr].cylinders[cy].output_pin = cy;
        f->ign.config.groups[gr].cylinders[cy].disabled = scenario % 7 == 0 && cy == 1;
      }
    }
    for(unsigned gr = 0; gr < INJECTION_CONFIG_GROUP_MAX; gr++) {
      if(scenario >= 48) {
        injection_config_group_setup_t *group = &f->inj.config.groups[gr];
        *group = ecu_timings_injection_config_default.groups[0];
        group->performance_fuel_pressure_manifold_source = scenario & 8 ?
            INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_RELATIVE_MAP : INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_NONE;
        group->performance_fuel_pressure_fuelramp_source = scenario & 16 ?
            INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_RELATIVE_MAP : INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_NONE;
        group->performance_fuel_pressure_manifold_io_type = ECU_CONFIG_IO_MAP_MANIFOLD;
        group->performance_fuel_pressure_fuelramp_io_type = ECU_CONFIG_IO_MAP_THROTTLE;
        group->performance_static_units = scenario & 1 ? INJECTION_CONFIG_GROUP_PERF_UNITS_CC : INJECTION_CONFIG_GROUP_PERF_UNITS_G;
        group->performance_static = 400.0f + 27.3f * gr;
        group->performance_static_fuel_pressure = 2.0f + (scenario % 7) * 0.35f;
        group->performance_fuelramp_nominal_pressure = 3.2f + gr * 0.31f;
        group->performance_fuel_mass_per_cc = 720.0f + scenario % 23;
        group->phase_mode = scenario & 2 ? INJECTION_CONFIG_GROUP_PHASE_MODE_ENDING : INJECTION_CONFIG_GROUP_PHASE_MODE_BEGINNING;
      }
      f->inj.config.groups[gr].enabled = scenario % 11 != 0 || gr == 0;
      f->inj.config.groups[gr].phase_slew_rate_mode = scenario % INJECTION_CONFIG_GROUP_PHASE_SLEW_RATE_MODE_MAX;
      f->inj.config.groups[gr].process_update_trigger = scenario % 3;
      f->inj.config.groups[gr].mode = scenario % INJECTION_CONFIG_GROUP_MODE_MAX;
      for(unsigned cy = 0; cy < cylinders; cy++) {
        f->inj.config.groups[gr].cylinders[cy].output_pin = cy + 8;
        f->inj.config.groups[gr].cylinders[cy].performance_static_mul = 1.0f;
        f->inj.config.groups[gr].cylinders[cy].performance_dynamic_mul = 1.0f;
      }
    }
    for(unsigned s = 0; s < KNOCKWINDOW_CONFIG_SETUP_MAX; s++) {
      f->knock.setups[s].pulsedadc_ctx = &adc[s];
      adc[s].sampling_frequency = 100000;
      for(unsigned cy = 0; cy < cylinders; cy++) {
        f->knock.config.setups[s].cylinders_supported[cy].cy = cy;
        f->knock.config.setups[s].cylinders_supported[cy].enabled = true;
      }
    }
  }
}

static unsigned mean_rounding_differences;
static double mean_max_difference;
__attribute__((optimize("O2,no-fast-math")))
static void verify_advance_mean(unsigned gr, unsigned cylinders)
{
  double expected = 0, requested = 0;
  for(unsigned cy = 0; cy < cylinders; cy++) {
    expected += fixtures[0].ign.runtime.groups[gr].advance_cy[cy];
    requested += fixtures[0].ign.runtime.groups[gr].advance_requested_cy[cy];
  }
  expected /= cylinders;
  requested /= cylinders;
  double actual = fixtures[1].ign.runtime.groups[gr].advance_mean;
  double actual_requested = fixtures[1].ign.runtime.groups[gr].advance_requested_mean;
  assert(isfinite(actual) && isfinite(actual_requested));
  assert(fabs(actual - expected) <= 8.0 * FLT_EPSILON * fmax(1.0, fabs(expected)));
  assert(fabs(actual_requested - requested) <= 8.0 * FLT_EPSILON * fmax(1.0, fabs(requested)));
  float old_requested = fixtures[0].ign.runtime.groups[gr].advance_requested_mean;
  float new_requested = fixtures[1].ign.runtime.groups[gr].advance_requested_mean;
  if(memcmp(&old_requested, &new_requested, sizeof(float))) mean_rounding_differences++;
  double difference = fabs((double)old_requested - new_requested);
  if(difference > mean_max_difference) mean_max_difference = difference;
}

static void test_callbacks(void)
{
  unsigned long total_events = 0;
  for(case_id = 0; case_id < 112; case_id++) {
    unsigned cylinders = case_id & 1 ? 8 : 4;
    init_fixture(cylinders, case_id & 2 ? 2 : 1, case_id);
    for(step_id = 0; step_id < 2400; step_id++) {
      clock_us += step_id % 58 == 0 ? 300 : 100;
      unsigned count_ref = 0;
      for(unsigned k = 0; k < 2; k++) {
        active = &fixtures[k];
        timing_base_data_crankshaft_t *crank = &active->base.data.crankshaft;
        crank->mode = step_id < 30 || step_id > 2200 ? TIMING_CRANKSHAFT_MODE_IDLE :
            step_id < 600 || (step_id > 1800 && step_id < 1900) ? TIMING_CRANKSHAFT_MODE_VALID : TIMING_CRANKSHAFT_MODE_VALID_PHASED;
        crank->valid = crank->mode >= TIMING_CRANKSHAFT_MODE_VALID && (!active->base.config.phased_only || crank->mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED);
        crank->sensor_data.previous = crank->sensor_data.current;
        unsigned tooth = step_id % 116;
        tooth += 2 * (tooth / 58); /* 60-2, including both missing-tooth gaps. */
        crank->sensor_data.current.position = (int)tooth * 6.0f - 360.0f;
        if(crank->mode != TIMING_CRANKSHAFT_MODE_VALID_PHASED)
          timing_position_clamp(crank->sensor_data.current.position, false, &crank->sensor_data.current.position);
        crank->sensor_data.current_position = crank->sensor_data.current.position;
        crank->sensor_data.current.timestamp = clock_us;
        crank->sensor_data.period = 6000.0f;
        crank->sensor_data.rpm = 10000.0f;
        crank->sensor_data.us_per_degree_pulsed = crank->sensor_data.us_per_degree_revolution = 6000.0f / 360.0f;
        for(unsigned b = 0; b < calibration.cylinders.banks_count; b++) {
          active->ign.runtime.input_banked[b].allowed = (ecu_core_runtime_value_ctx_t){step_id % 500 < 450, true};
          active->ign.runtime.input_banked[b].ignition_advance = (ecu_core_runtime_value_ctx_t){10.0f + (step_id / 113) % 25, true};
          active->inj.runtime.input_banked[b].allowed = (ecu_core_runtime_value_ctx_t){step_id % 500 < 450, true};
          active->inj.runtime.input_banked[b].injection_phase = (ecu_core_runtime_value_ctx_t){270.0f, true};
          active->inj.runtime.input_banked[b].injection_mass = (ecu_core_runtime_value_ctx_t){15.0f + step_id % 10, true};
          active->knock.runtime.input_banked[b].allowed = (ecu_core_runtime_value_ctx_t){1, true};
          active->knock.runtime.input_banked[b].knock_window_start = (ecu_core_runtime_value_ctx_t){-5, true};
          active->knock.runtime.input_banked[b].knock_window_end = (ecu_core_runtime_value_ctx_t){-50, true};
        }
        for(unsigned s = 0; s < KNOCKWINDOW_CONFIG_SETUP_MAX; s++) {
          knockwindow_setup_ctx_t *setup = &active->knock.setups[s];
          if(setup->runtime.working && step_id % 11 == 0) {
            setup->runtime.cplt_irq = true;
            setup->sampling_cplt_ctx.samples_buffer = samples;
            setup->sampling_cplt_ctx.samples_count = 400;
            setup->sampling_cplt_ctx.time_start = clock_us - 400;
            setup->sampling_cplt_ctx.time_cplt = clock_us;
          }
        }
        event_count = 0;
        if(k == 0) {
          reference_timing_base_signal_update_cb(&active->base, &active->base.data, &active->base.diag);
          count_ref = event_count;
          memcpy(reference_events, events, sizeof(events));
        } else {
          timing_base_signal_update_cb(&active->base, &active->base.data, &active->base.diag);
          assert(event_count == count_ref);
          equal(events, reference_events, event_count * sizeof(events[0]), "peripheral events");
          total_events += event_count;
        }
      }
      equal(&fixtures[0].base.process_update_trigger_counter, &fixtures[1].base.process_update_trigger_counter, sizeof(unsigned), "base trigger counter");
      equal(&fixtures[0].base.diag, &fixtures[1].base.diag, sizeof(fixtures[0].base.diag), "base diagnostics");
      equal(&fixtures[0].base.data, &fixtures[1].base.data, sizeof(fixtures[0].base.data), "base data");
      // advance_mean is newly published by 06fdef7. Its independent numerical
      // oracle allows FP accumulation/division rounding, not changes to old fields.
      for(unsigned gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
        verify_advance_mean(gr, cylinders);
        fixtures[0].ign.runtime.groups[gr].advance_mean = fixtures[1].ign.runtime.groups[gr].advance_mean;
        fixtures[0].ign.runtime.groups[gr].advance_requested_mean = fixtures[1].ign.runtime.groups[gr].advance_requested_mean;
      }
      equal(&fixtures[0].ign.runtime, &fixtures[1].ign.runtime, sizeof(fixtures[0].ign.runtime), "ignition");
      equal(&fixtures[0].inj.runtime, &fixtures[1].inj.runtime, sizeof(fixtures[0].inj.runtime), "injection");
      equal(&fixtures[0].balance.runtime, &fixtures[1].balance.runtime, sizeof(fixtures[0].balance.runtime), "cybalance");
      for(unsigned s = 0; s < KNOCKWINDOW_CONFIG_SETUP_MAX; s++) {
        equal(&fixtures[0].knock.setups[s].runtime, &fixtures[1].knock.setups[s].runtime, sizeof(fixtures[0].knock.setups[s].runtime), "knockwindow state");
        equal(&fixtures[0].knock.setups[s].sampling_cplt_ctx, &fixtures[1].knock.setups[s].sampling_cplt_ctx, sizeof(fixtures[0].knock.setups[s].sampling_cplt_ctx), "knockwindow completion");
      }
    }
  }
  printf("Callbacks: 112 scenarios x 2400 CKP updates; %lu identical peripheral events.\n", total_events);
  printf("Ignition mean diagnostics: %u rounding differences, maximum %.9g degrees; both means match double oracle.\n", mean_rounding_differences, mean_max_difference);
}
int main(void)
{
  time_init_timebase(&clock_us, UINT_MAX);
  test_lookup();
  test_angle_clamp();
  test_positions();
  test_callbacks();
  puts("PASS");
  return 0;
}
