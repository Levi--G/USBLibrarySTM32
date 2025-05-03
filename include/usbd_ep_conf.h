/**
 ******************************************************************************
 * @file    usbd_ep_conf.h
 * @brief   USB Device endpoints configuration
 ******************************************************************************
 * @attention
 *
 * Modified by Levi Gillis @ 2022
 * Adjusted and reimplemented for compatibility with arduino
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      www.st.com/SLA0044
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_EP_CONF_H
#define __USBD_EP_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "USBEP.h"

#define SMALL_EP(Ep) (Ep & 0xFU)
#define IN_EP(Ep) (lowByte((Ep) | 0x80))
#define IS_IN_EP(Ep) ((Ep & 0x80) == 0x80)

#ifndef PCD_USE_DBL_BUF
#define PCD_USE_DBL_BUF 0
#endif

#ifndef PCD_DEF_BUF
#if PCD_USE_DBL_BUF
#define PCD_DEF_BUF PCD_DBL_BUF
#else
#define PCD_DEF_BUF PCD_SNG_BUF
#endif
#endif

#define USB_ABS_EP0_SIZE (PCD_USE_DBL_BUF ? USB_EP0_SIZE * 2 : USB_EP0_SIZE)
#define USB_ABS_EP_SIZE (PCD_USE_DBL_BUF ? USB_EP_SIZE * 2 : USB_EP_SIZE)

  typedef struct
  {
    uint32_t ep_num;  /* Endpoint number+direction */
    uint32_t ep_type; /* Endpoint type */
  } ep_desc_t;

#define PMA_BASE_ADDR (4 * 2 * USB_MAX_EPS)
#ifndef PMA_MAX_SIZE
#define PMA_MAX_SIZE (512 - PMA_BASE_ADDR)
#endif

  void USB_EP_ClearEndpoints();
  uint8_t USB_EP_AddEndpoint(uint32_t ep_type);
  uint_fast8_t USB_EP_GetNumEndpoints();
  const ep_desc_t *USB_EP_GetEndpointsSlots();

#ifdef __cplusplus
}
#endif

#endif /* __USBD_EP_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/