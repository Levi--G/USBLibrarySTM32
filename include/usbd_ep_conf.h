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
extern "C" {
#endif

#include <stdint.h>

#define SMALL_EP(Ep) (Ep & 0xFU)
#define IN_EP(Ep) (lowByte((Ep) | 0x80))
#define IS_IN_EP(Ep) ((Ep | 0x80) == 0x80)

typedef struct
{
  uint32_t ep_num;  /* Endpoint number+direction */
  uint32_t ep_type;  /* Endpoint type */
  uint32_t ep_size; /* Endpoint size */
  uint32_t ep_kind; /* PCD Endpoint Kind: PCD_SNG_BUF or PCD_DBL_BUF */
} ep_desc_t;

#define USB_MAX_EPS 8
#define USB_MAX_EPS_SLOTS (USB_MAX_EPS * 2)

#define PMA_BASE_ADDR 0x40 //4*uint16*max EP
#ifndef PMA_MAX_SIZE
#define PMA_MAX_SIZE (512 - PMA_BASE_ADDR)
#endif

extern ep_desc_t ep_def[USB_MAX_EPS_SLOTS];

uint8_t USB_PMA_GetNumEndpoints();
uint8_t USB_PMA_GetNumEndpointsSlots();
uint32_t USB_PMA_GetNextEndpoint();
uint8_t USB_PMA_GetNextEndpointSlot();
int USB_PMA_GetEndpointsSize();

#ifdef __cplusplus
}
#endif

#endif /* __USBD_EP_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/