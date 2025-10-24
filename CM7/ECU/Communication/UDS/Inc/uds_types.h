/*
 * uds_types.h
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_UDS_INC_UDS_TYPES_H_
#define COMMUNICATION_UDS_INC_UDS_TYPES_H_

#define UDS_RESPONSE_NEGATIVE_CODE    0x7F
#define UDS_RESPONSE_POSITIVE_OFFSET  0x40

#include "common.h"
#include "time.h"
#include "versioned_uds.h"

typedef enum {
  UDS_OK = 0,
  UDS_MAX
}uds_error_code_t;

typedef enum {
  UDS_SID_START                               = 0x10,

  UDS_SID_DIAGNOSTIC_SESSION_CONTROL          = 0x10,
  UDS_SID_ECU_RESET                           = 0x11,
  UDS_SID_CLEAR_DIAGNOSTIC_INFORMATION        = 0x14,
  UDS_SID_READ_DTC_INFORMATION                = 0x19,
  UDS_SID_READ_DATA_BY_IDENTIFIER             = 0x22,
  UDS_SID_READ_MEMORY_BY_ADDRESS              = 0x23,
  UDS_SID_READ_SCALING_DATA_BY_IDENTIFIER     = 0x24,
  UDS_SID_SECURITY_ACCESS                     = 0x27,
  UDS_SID_COMMUNICATION_CONTROL               = 0x28,
  UDS_SID_READ_DATA_BY_PERIODIC_IDENTIFIER    = 0x2A,
  UDS_SID_DYNAMICALLY_DEFINE_DATA_IDENTIFIER  = 0x2C,
  UDS_SID_WRITE_DATA_BY_IDENTIFIER            = 0x2E,
  UDS_SID_INPUT_OUTPUT_CONTROL_BY_IDENTIFIER  = 0x2F,
  UDS_SID_ROUTINE_CONTROL                     = 0x31,
  UDS_SID_REQUEST_DOWNLOAD                    = 0x34,
  UDS_SID_REQUEST_UPLOAD                      = 0x35,
  UDS_SID_TRANSFER_DATA                       = 0x36,
  UDS_SID_REQUEST_TRANSFER_EXIT               = 0x37,
  USD_SID_REQUEST_FILE_TRANSFER               = 0x38,
  UDS_SID_WRITE_MEMORY_BY_ADDRESS             = 0x3D,
  UDS_SID_TESTER_PRESENT                      = 0x3E,

  UDS_SID_ACCESS_TIMING_PARAMETERS            = 0x83,
  UDS_SID_SECURE_DATA_TRANSMISSION            = 0x84,
  UDS_SID_CONTROL_DTC_SETTING                 = 0x85,
  UDS_SID_RESPONSE_ON_EVENT                   = 0x86,
  UDS_SID_LINK_CONTROL                        = 0x87,

  UDS_SID_VENDOR_SPECIFIC_START               = 0xC0,
  UDS_SID_VENDOR_SPECIFIC_END                 = 0xFF,

  UDS_SID_END                                 = 0xFF
}uds_sid_t;

typedef enum
{
  UDS_DID_VENDOR_RESERVED_START                     = 0x1000,
  UDS_DID_VENDOR_RESERVED_END                       = 0x17FF,

  UDS_DID_BOOT_SOFTWARE_IDENTIFICATION              = 0xF180,  // Boot software identification
  UDS_DID_APPLICATION_SOFTWARE_IDENTIFICATION       = 0xF181,  // Application software identification
  UDS_DID_APPLICATION_DATA_IDENTIFICATION           = 0xF182,  // Application data (calibration) identification
  UDS_DID_BOOT_SOFTWARE_FINGERPRINT                 = 0xF183,  // Boot software fingerprint (checksum / signature)
  UDS_DID_APPLICATION_SOFTWARE_FINGERPRINT          = 0xF184,  // Application software fingerprint
  UDS_DID_APPLICATION_DATA_FINGERPRINT              = 0xF185,  // Application data fingerprint
  UDS_DID_ACTIVE_DIAGNOSTIC_SESSION                 = 0xF186,  // Active diagnostic session ID
  UDS_DID_VIN                                       = 0xF187,  // Vehicle Identification Number (VIN)
  UDS_DID_ECU_SOFTWARE_NUMBER                       = 0xF188,  // Vehicle manufacturer ECU software number
  UDS_DID_ECU_MANUFACTURING_DATE                    = 0xF189,  // ECU manufacturing date (YYYYMMDD or BCD)
  UDS_DID_ECU_SERIAL_NUMBER                         = 0xF18A,  // ECU serial number
  UDS_DID_SPARE_PART_NUMBER                         = 0xF18B,  // Spare part number
  UDS_DID_SYSTEM_SUPPLIER_IDENTIFIER                = 0xF18C,  // System supplier identifier
  UDS_DID_ECU_HARDWARE_NUMBER                       = 0xF18D,  // ECU hardware number
  UDS_DID_SYSTEM_NAME_OR_ENGINE_TYPE                = 0xF18E,  // System name or engine type
  UDS_DID_ODX_FILE_IDENTIFIER                       = 0xF18F,  // ODX file identifier

  UDS_DID_VIN_STANDARD_ALIAS                        = 0xF190,  // Standard VIN alias (alternate DID for VIN)
  UDS_DID_VEHICLE_MANUFACTURER_SPARE_PART_NUMBER    = 0xF191,  // Vehicle manufacturer spare part number
  UDS_DID_VEHICLE_MANUFACTURER_ECU_HARDWARE_NUMBER  = 0xF192,  // Vehicle manufacturer ECU hardware number
  UDS_DID_SYSTEM_SUPPLIER_ECU_HARDWARE_NUMBER       = 0xF193,  // System supplier ECU hardware number
  UDS_DID_SYSTEM_SUPPLIER_ECU_SOFTWARE_NUMBER       = 0xF194,  // System supplier ECU software number
  UDS_DID_ECU_PROGRAMMING_DATE                      = 0xF195,  // ECU programming date
  UDS_DID_CALIBRATION_EQUIPMENT_SOFTWARE_NUMBER     = 0xF196,  // Calibration equipment software number
  UDS_DID_ECU_REPAIR_SHOP_CODE                      = 0xF197,  // Repair shop or service station code
  UDS_DID_TESTER_SERIAL_NUMBER                      = 0xF198,  // Tester serial number (traceability)
  UDS_DID_PROGRAMMING_COUNTER                       = 0xF199,  // Programming attempt counter

  UDS_DID_RESERVED_START                            = 0xF19A,
  UDS_DID_RESERVED_END                              = 0xF1FF
}uds_did_t;

typedef enum
{
  UDS_SECURITY_LEVEL_NONE     = 0x00,  // No security access (default state)
  UDS_SECURITY_LEVEL_1        = 0x01,  // Level 1 - basic diagnostic or calibration access
  UDS_SECURITY_LEVEL_2        = 0x02,  // Level 2 - programming access
  UDS_SECURITY_LEVEL_3        = 0x03,  // Level 3 - engineering / developer access
  UDS_SECURITY_LEVEL_4        = 0x04,  // Level 4 - extended OEM-specific access
  UDS_SECURITY_LEVEL_5        = 0x05,  // Level 5 - production or factory-only access
  UDS_SECURITY_LEVEL_SUPPLIER_SPECIFIC_START  = 0x06,
  UDS_SECURITY_LEVEL_SUPPLIER_SPECIFIC_END    = 0xFF
}uds_security_level_t;

typedef enum
{
  UDS_SESSION_DEFAULT                         = 0x01,  // Default diagnostic session
  UDS_SESSION_PROGRAMMING                     = 0x02,  // Programming session
  UDS_SESSION_EXTENDED_DIAGNOSTIC             = 0x03,  // Extended diagnostic session
  UDS_SESSION_SUPPLIER_SPECIFIC_START         = 0x40,
  UDS_SESSION_SUPPLIER_SPECIFIC_END           = 0x5F,
  UDS_SESSION_MANUFACTURER_SPECIFIC_START     = 0x60,
  UDS_SESSION_MANUFACTURER_SPECIFIC_END       = 0x7F,
  UDS_SESSION_DEVELOPER_SPECIFIC_START        = 0x80,
  UDS_SESSION_DEVELOPER_SPECIFIC_END          = 0xFF
}uds_session_type_t;

typedef enum
{
  UDS_RESPONSE_OK                                           = 0x00,  // Not an actual NRC, used for convenience

  UDS_RESPONSE_GENERAL_REJECT                               = 0x10,  // General reject
  UDS_RESPONSE_SERVICE_NOT_SUPPORTED                        = 0x11,  // Service not supported
  UDS_RESPONSE_SUBFUNCTION_NOT_SUPPORTED                    = 0x12,  // Sub-function not supported
  UDS_RESPONSE_INVALID_MESSAGE_LENGTH_OR_FORMAT             = 0x13,  // Invalid message length or format
  UDS_RESPONSE_RESPONSE_TOO_LONG                            = 0x14,  // Response too long (ECU cannot transmit)

  UDS_RESPONSE_BUSY_REPEAT_REQUEST                          = 0x21,  // ECU busy, try again
  UDS_RESPONSE_CONDITIONS_NOT_CORRECT                       = 0x22,  // Conditions not correct (e.g., engine not running)
  UDS_RESPONSE_REQUEST_SEQUENCE_ERROR                       = 0x24,  // Request sequence error
  UDS_RESPONSE_NO_RESPONSE_FROM_SUBNET_COMPONENT            = 0x25,  // No response from sub-node or network component
  UDS_RESPONSE_FAILURE_PREVENTS_EXECUTION                   = 0x26,  // Internal ECU failure prevents execution

  UDS_RESPONSE_REQUEST_OUT_OF_RANGE                         = 0x31,  // Parameter or DID out of range
  UDS_RESPONSE_SECURITY_ACCESS_DENIED                       = 0x33,  // Security access denied (key mismatch)
  UDS_RESPONSE_INVALID_KEY                                  = 0x35,  // Invalid key during SecurityAccess
  UDS_RESPONSE_EXCEEDED_NUMBER_OF_ATTEMPTS                  = 0x36,  // Exceeded number of login attempts
  UDS_RESPONSE_REQUIRED_TIME_DELAY_NOT_EXPIRED              = 0x37,  // Required delay time not yet expired

  UDS_RESPONSE_UPLOAD_DOWNLOAD_NOT_ACCEPTED                 = 0x70,  // ECU not ready for upload/download
  UDS_RESPONSE_TRANSFER_DATA_SUSPENDED                      = 0x71,  // Data transfer suspended
  UDS_RESPONSE_GENERAL_PROGRAMMING_FAILURE                  = 0x72,  // General programming failure
  UDS_RESPONSE_WRONG_BLOCK_SEQUENCE_COUNTER                 = 0x73,  // Wrong block sequence counter
  UDS_RESPONSE_REQUEST_CORRECTLY_RECEIVED_RESPONSE_PENDING  = 0x78,  // ECU accepted request but processing is pending (Response Pending)

  UDS_RESPONSE_SUBFUNCTION_NOT_SUPPORTED_IN_ACTIVE_SESSION  = 0x7E,  // Sub-function not supported in current session
  UDS_RESPONSE_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION      = 0x7F,  // Service not supported in current session

  UDS_RESPONSE_RPM_TOO_HIGH                                 = 0x81,  // Engine speed too high
  UDS_RESPONSE_RPM_TOO_LOW                                  = 0x82,  // Engine speed too low
  UDS_RESPONSE_ENGINE_RUNNING                               = 0x83,  // Engine running — operation not allowed
  UDS_RESPONSE_ENGINE_NOT_RUNNING                           = 0x84,  // Engine not running — operation not possible
  UDS_RESPONSE_ENGINE_RUN_TIME_TOO_LOW                      = 0x85,  // Engine run time too low
  UDS_RESPONSE_TEMPERATURE_TOO_HIGH                         = 0x86,  // Temperature too high
  UDS_RESPONSE_TEMPERATURE_TOO_LOW                          = 0x87,  // Temperature too low
  UDS_RESPONSE_VEHICLE_SPEED_TOO_HIGH                       = 0x88,  // Vehicle speed too high
  UDS_RESPONSE_VEHICLE_SPEED_TOO_LOW                        = 0x89,  // Vehicle speed too low
  UDS_RESPONSE_THROTTLE_PEDAL_TOO_HIGH                      = 0x8A,  // Accelerator pedal too high
  UDS_RESPONSE_THROTTLE_PEDAL_TOO_LOW                       = 0x8B,  // Accelerator pedal not pressed enough
  UDS_RESPONSE_TRANSMISSION_RANGE_NOT_IN_NEUTRAL            = 0x8C,  // Transmission not in neutral
  UDS_RESPONSE_TRANSMISSION_RANGE_NOT_IN_GEAR               = 0x8D,  // Transmission not in gear mode
  UDS_RESPONSE_BRAKE_SWITCH_NOT_CLOSED                      = 0x8F,  // Brake not applied — activation required
  UDS_RESPONSE_SHIFTER_LEVER_NOT_IN_PARK                    = 0x90,  // Shifter lever not in “Park” position
  UDS_RESPONSE_TORQUE_CONVERTER_CLUTCH_LOCKED               = 0x91,  // Torque converter clutch locked
  UDS_RESPONSE_VOLTAGE_TOO_HIGH                             = 0x92,  // Voltage too high
  UDS_RESPONSE_VOLTAGE_TOO_LOW                              = 0x93,  // Voltage too low

  // Reserved / non-standard
  UDS_RESPONSE_RESERVED_0x94                                = 0x94,
  UDS_RESPONSE_RESERVED_0x95                                = 0x95,
  UDS_RESPONSE_RESERVED_0x96                                = 0x96,
  UDS_RESPONSE_RESERVED_0x97                                = 0x97,
  UDS_RESPONSE_RESERVED_0x98                                = 0x98,
  UDS_RESPONSE_RESERVED_0x99                                = 0x99,
  UDS_RESPONSE_RESERVED_0x9A                                = 0x9A,
  UDS_RESPONSE_RESERVED_0x9B                                = 0x9B,

  UDS_RESPONSE_VENDOR_SPECIFIC_START                        = 0xC0,
  UDS_RESPONSE_VENDOR_SPECIFIC_END                          = 0xFF
} uds_response_code_t;

typedef struct uds_ctx_tag uds_ctx_t;

typedef void (*uds_error_callback_t)(uds_ctx_t *ctx, uds_error_code_t code, void *userdata);

typedef struct {
    uds_error_callback_t error_callback;
    void *callback_userdata;
}uds_init_ctx_t;

typedef struct uds_ctx_tag {
    uds_config_t config;
    uds_init_ctx_t init;
    bool initialized;
    bool configured;

    uds_error_code_t error_code;
    bool reset_trigger;

}uds_ctx_t;

#endif /* COMMUNICATION_UDS_INC_UDS_TYPES_H_ */
