/**
  ******************************************************************************
  * @file    USBCore_stm32.h
  * @author  MCD Application Team
  * @brief   Header file for the USBCore.cpp file.
  ******************************************************************************
  * @attention
  * 
  * Modified by Levi Gillis @ 2022
  * Removed USBComposite specific defines and adjusted for compatibility with arduino
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __USBCORE_STM32_H__
#define __USBCORE_STM32_H__

#ifdef USBCON

#include "usbd_ioreq.h"
#include "usbd_ep_conf.h"

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22

#ifndef HID_HS_BINTERVAL
#define HID_HS_BINTERVAL              0x07U
#endif /* HID_HS_BINTERVAL */

#ifndef HID_FS_BINTERVAL
#define HID_FS_BINTERVAL            0x0AU
#endif /* HID_FS_BINTERVAL */

#define HID_REQ_SET_PROTOCOL          0x0BU
#define HID_REQ_GET_PROTOCOL          0x03U

#define HID_REQ_SET_IDLE              0x0AU
#define HID_REQ_GET_IDLE              0x02U

#define HID_REQ_SET_REPORT            0x09U
#define HID_REQ_GET_REPORT            0x01U

typedef enum {
  HID_IDLE = 0,
  HID_BUSY,
} HID_StateTypeDef;

typedef struct {
  uint32_t             Protocol;
  uint32_t             IdleState;
  uint32_t             AltSetting;
  HID_StateTypeDef     EPstate[16];
} USBD_HID_HandleTypeDef;

#endif /* USBCON */
#endif /* __USBCORE_STM32_H__ */