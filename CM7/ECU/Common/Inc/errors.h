/*
 * errors.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_ERRORS_H_
#define COMMON_INC_ERRORS_H_

typedef enum {
  E_OK = 0,
  E_FAULT = -1,
  E_TIMEOUT = -2,
  E_AGAIN = -3,
  E_BUSY = -4,
  E_PARAM = -5,
  E_IO = -6,
  E_HAL = -7,
  E_NOTRDY = -8,
  E_OVERFLOW = -9,
  E_NOTSUPPORT = -10,
  E_INVALACT = -11,
  E_BADRESP = -12,
  E_BADVALUE = -13,
}error_t;

#endif /* COMMON_INC_ERRORS_H_ */
