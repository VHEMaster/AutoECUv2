/*
 * common.h
 *
 *  Created on: Jun 8, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_COMMON_H_
#define COMMON_INC_COMMON_H_

#include "main.h"
#include "bool.h"
#include "compiler.h"
#include "types.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#include "errors.h"

typedef uint32_t ecu_calibration_instance_t;

typedef struct {
    float value;
    bool valid;
}ecu_core_runtime_value_ctx_t;

#endif /* COMMON_INC_COMMON_H_ */
