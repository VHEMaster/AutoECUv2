/*
 * core_runtime_parameters.h
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_
#define CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_

#include "common.h"
#include "config_engine.h"

typedef struct {
    struct {
        cj125_operating_status_t status;
        float lambda_value;
    }read;
    struct {
        cj125_heatup_type_t heatup;
    }write;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_device_wbls_ctx_t;

typedef struct {
    struct {
        bool failure;
        int32_t pos_current;
        int32_t pos_target;
    }read;
    struct {
        bool set_enabled;
        bool set_target;
        bool set_current;
        bool set_reset;

        bool enabled;
        int32_t pos_target;
        int32_t pos_current;
        int32_t pos_reset;

    }write;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_device_stepper_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_device_wbls_ctx_t wbls[ECU_DEVICE_WBLS_MAX];
    ecu_core_runtime_global_parameters_device_stepper_ctx_t stepper[ECU_DEVICE_STEPPER_MAX];
}ecu_core_runtime_global_parameters_devices_ctx_t;

typedef struct {
    float value;
    bool read_valid;
}ecu_core_runtime_global_parameters_sensor_value_ctx_t;

typedef struct {
    struct {
        timing_crankshaft_mode_t mode;
        float period;
        float rpm;
        uint32_t revs_count;
    }read;
    bool read_valid;
}ecu_core_runtime_global_parameters_module_timing_ctx_t;

typedef struct {
    struct {
        bool set_enabled;
        bool set_target;

        bool enabled;
        float target;
    }write;
    struct {
        bool enabled;
        float pos_current;
        float pos_target;
    }read;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_etc_ctx_t;

typedef struct {
    struct {
        bool set_enabled;
        bool set_target_pos;
        bool set_target_dc;

        bool enabled;
        float target_dc;
        float target_pos;
    }write;
    struct {
        bool enabled;
        float pos_current;
        float pos_target;
        float dc_current;
        float dc_target;
    }read;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_vvt_ctx_t;

typedef struct {
    struct {
        bool set_activate_trigger;
        bool activate_trigger;

        bool set_emergency_trigger;
        bool emergency_trigger;
    }write;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_coolingfan_ctx_t;

typedef struct {
    struct {
        bool operating;
    }read;
    struct {
        bool set_operating;
        bool set_trigger;

        bool operating;
    }write;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_ignpower_ctx_t;

typedef struct {
    struct {
        bool set_manual_engaged;

        bool manual_enabled;
    }write;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_indication_ctx_t;

typedef struct {
    struct {
        bool set_enabled;
        bool set_boost_target;
        bool set_dc_target;

        bool enabled;
        float boost_target;
        float dc_target;
    }write;
    bool write_valid;
}ecu_core_runtime_global_parameters_module_wgcv_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_module_timing_ctx_t timing[ECU_MODULE_TIMING_MAX];
    ecu_core_runtime_global_parameters_module_etc_ctx_t etc[ECU_MODULE_ETC_MAX];
    ecu_core_runtime_global_parameters_module_vvt_ctx_t vvt[ECU_MODULE_VVT_MAX];
    ecu_core_runtime_global_parameters_module_coolingfan_ctx_t coolingfan[ECU_MODULE_COOLINGFAN_MAX];
    ecu_core_runtime_global_parameters_module_ignpower_ctx_t ignpower[ECU_MODULE_IGNPOWER_MAX];
    ecu_core_runtime_global_parameters_module_indication_ctx_t indication[ECU_MODULE_INDICATION_MAX];
    ecu_core_runtime_global_parameters_module_wgcv_ctx_t wgcv[ECU_MODULE_WGCV_MAX];
}ecu_core_runtime_global_parameters_modules_ctx_t;

typedef struct {
    struct {
        struct {
            float saturation_time;
            float advance[ECU_BANK_MAX];
        }groups[ECU_CONFIG_IGNITION_GROUP_MAX];
    }read;
    struct {
        float ignition_advance_banked[ECU_BANK_MAX];
    }write;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_timing_ignition_ctx_t;

typedef struct {
    struct {
        struct {
            float phase;
            float lag_time;
            float time_inject_mean;
            float dutycycle_max;
            float dutycycle_mean;
            float enrichment_late_phase;
        }groups[ECU_CONFIG_INJECTION_GROUP_MAX];
    }read;
    struct {
        float injection_phase;
        float injection_mass_banked[ECU_BANK_MAX];
    }write;
    bool read_valid;
    bool write_valid;
}ecu_core_runtime_global_parameters_timing_injection_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_timing_ignition_ctx_t ignition;
    ecu_core_runtime_global_parameters_timing_injection_ctx_t injection;
}ecu_core_runtime_global_parameters_timings_ctx_t;

typedef struct {
    ecu_core_runtime_global_parameters_sensor_value_ctx_t sensors[ECU_SENSOR_TYPE_MAX][ECU_SENSOR_INSTANCE_MAX];
    ecu_core_runtime_global_parameters_devices_ctx_t devices;
    ecu_core_runtime_global_parameters_modules_ctx_t modules;
    ecu_core_runtime_global_parameters_timings_ctx_t timings;
}ecu_core_runtime_global_parameters_ctx_t;

#endif /* CORE_CORE_INC_CORE_RUNTIME_PARAMETERS_H_ */
