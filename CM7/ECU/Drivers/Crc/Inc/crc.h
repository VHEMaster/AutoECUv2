/*
 * crc.h
 *
 *  Created on: May 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CRC_INC_CRC_H_
#define DRIVERS_CRC_INC_CRC_H_

#include "errors.h"
#include "bool.h"
#include <stdint.h>

#define CRC_POLY    (0x8005)

typedef uint16_t crc_checksum_t;

error_t crc_init(void);

error_t crc_lock(void);
error_t crc_unlock(void);

error_t crc_calculate(crc_checksum_t *crc, const void *payload, uint32_t length, bool reset);

#endif /* DRIVERS_CRC_INC_CRC_H_ */
