/*
 * config_global_ipc.h
 *
 *  Created on: Nov 23, 2026
 */

#ifndef CONFIG_GLOBAL_INC_CONFIG_GLOBAL_IPC_H_
#define CONFIG_GLOBAL_INC_CONFIG_GLOBAL_IPC_H_


#include "config_global.h"
#define ECU_CONFIG_GLOBAL_IPC_BYTES_MAX (512)

typedef struct {
    uint32_t version;
    uint32_t size;
}ecu_config_global_header_t;

#ifdef CORE_CM7
error_t ecu_config_global_ipc_init(ecu_config_global_runtime_ctx_t *ctx);
#endif /* CORE_CM7 */

error_t ecu_config_global_get_type_max(ecu_config_type_t *type_max);
error_t ecu_config_global_get_item_max(ecu_config_type_t type, ecu_index_type_t *item_max);
error_t ecu_config_global_get_instance_max(ecu_config_type_t type, ecu_index_type_t index, ecu_instance_t *instance_max);
error_t ecu_config_global_get_header(ecu_config_type_t type, ecu_index_type_t index,
    ecu_instance_t instance, ecu_config_global_header_t *header);

error_t ecu_config_global_read(ecu_config_type_t type, ecu_index_type_t index,
    ecu_instance_t instance, uint32_t offset, void *data, uint32_t bytes);
error_t ecu_config_global_write(ecu_config_type_t type, ecu_index_type_t index,
    ecu_instance_t instance, uint32_t offset, const void *data, uint32_t bytes);

#endif /* CONFIG_GLOBAL_INC_CONFIG_GLOBAL_IPC_H_ */
