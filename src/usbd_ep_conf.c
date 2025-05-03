/**
 ******************************************************************************
 * @file    usbd_ep_conf.c
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
#if defined(HAL_PCD_MODULE_ENABLED) && defined(USBCON)
/* Includes ------------------------------------------------------------------*/
#include "usbd_ep_conf.h"

ep_desc_t ep_def[USB_MAX_EPS_SLOTS];
uint_fast8_t ep_num = 0;

void USB_EP_ClearEndpoints()
{
  ep_desc_t epdef = {0, 0};
  for (uint_fast8_t i = 0; i < USB_MAX_EPS_SLOTS; i++)
  {
    ep_def[i] = epdef;
  }
  ep_num = 0;
}

int USB_EP_GetEndpointsSize()
{
  return (2 * USB_ABS_EP0_SIZE + ep_num * USB_ABS_EP_SIZE);
}

uint8_t USB_EP_AddEndpoint(uint32_t ep_type)
{
  if (ep_num >= USB_MAX_EPS_SLOTS || USB_EP_GetEndpointsSize() + USB_ABS_EP_SIZE > PMA_MAX_SIZE)
  {
    return 0;
  }
  uint8_t ep = ep_num + 1;
  ep_def[ep_num].ep_num = ep | (ep_type & (uint32_t)USB_ENDPOINT_DIRECTION_MASK);
  ep_def[ep_num].ep_type = (ep_type & (uint32_t)USB_ENDPOINT_TYPE_MASK);
  ep_num++;
  return ep;
}

uint_fast8_t USB_EP_GetNumEndpoints()
{
  return ep_num;
}

const ep_desc_t *USB_EP_GetEndpointsSlots()
{
  return ep_def;
}

#endif /* HAL_PCD_MODULE_ENABLED && USBCON */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
