/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/** \ingroup CDC_RNDIS
 * \defgroup CDC_RNSID_Host Host
 *  @{ */

#ifndef _TUSB_CDC_RNDIS_HOST_H_
#define _TUSB_CDC_RNDIS_HOST_H_

#include "common/tusb_common.h"
#include "host/usbh.h"
#include "cdc_rndis.h"

#ifdef __cplusplus
 extern "C" {
#endif

<<<<<<< HEAD
=======
<<<<<<<< HEAD:tools/sdk/esp32s3/include/arduino_tinyusb/tinyusb/src/class/cdc/cdc_rndis_host.h
>>>>>>> orig_master_espressif
//--------------------------------------------------------------------+
// INTERNAL RNDIS-CDC Driver API
//--------------------------------------------------------------------+
typedef struct {
  OSAL_SEM_DEF(semaphore_notification);
  osal_semaphore_handle_t sem_notification_hdl;  // used to wait on notification pipe
  uint32_t max_xfer_size; // got from device's msg initialize complete
  uint8_t mac_address[6];
}rndish_data_t;

void rndish_init(void);
<<<<<<< HEAD
tusb_error_t rndish_open_subtask(uint8_t dev_addr, cdch_data_t *p_cdc);
void rndish_xfer_isr(cdch_data_t *p_cdc, pipe_handle_t pipe_hdl, xfer_result_t event, uint32_t xferred_bytes);
void rndish_close(uint8_t dev_addr);
=======
bool rndish_open_subtask(uint8_t dev_addr, cdch_data_t *p_cdc);
void rndish_xfer_isr(cdch_data_t *p_cdc, pipe_handle_t pipe_hdl, xfer_result_t event, uint32_t xferred_bytes);
void rndish_close(uint8_t dev_addr);
========
#define ERROR_ENUM(x) x,
#define ERROR_STRING(x) #x,

#define ERROR_TABLE(ENTRY) \
    ENTRY(TUSB_ERROR_NONE                            )\
    ENTRY(TUSB_ERROR_INVALID_PARA                    )\
    ENTRY(TUSB_ERROR_DEVICE_NOT_READY                )\
    ENTRY(TUSB_ERROR_INTERFACE_IS_BUSY               )\
    ENTRY(TUSB_ERROR_HCD_OPEN_PIPE_FAILED            )\
    ENTRY(TUSB_ERROR_OSAL_TIMEOUT                    )\
    ENTRY(TUSB_ERROR_CDCH_DEVICE_NOT_MOUNTED         )\
    ENTRY(TUSB_ERROR_MSCH_DEVICE_NOT_MOUNTED         )\
    ENTRY(TUSB_ERROR_NOT_SUPPORTED                   )\
    ENTRY(TUSB_ERROR_NOT_ENOUGH_MEMORY               )\
    ENTRY(TUSB_ERROR_FAILED                          )\

/// \brief Error Code returned
/// TODO obsolete and to be remove
typedef enum
{
  ERROR_TABLE(ERROR_ENUM)
  TUSB_ERROR_COUNT
}tusb_error_t;

#if CFG_TUSB_DEBUG
/// Enum to String for debugging purposes. Only available if \ref CFG_TUSB_DEBUG > 0
extern char const* const tusb_strerr[TUSB_ERROR_COUNT];
extern char const* const tusb_speed_str[];

#endif
>>>>>>>> orig_master_espressif:tools/sdk/esp32s2/include/arduino_tinyusb/tinyusb/src/common/tusb_error.h
>>>>>>> orig_master_espressif

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CDC_RNDIS_HOST_H_ */

/** @} */
