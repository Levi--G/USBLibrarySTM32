/**
  ******************************************************************************
  * @file    usbd_ep_conf.c
  * @brief   USB Device endpoints configuration
  ******************************************************************************
  * @attention
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

ep_desc_t ep_def[USB_MAX_EPS_SLOTS] = {
    {0x00, 0, USB_FS_MAX_PACKET_SIZE, PCD_SNG_BUF},
    {0x80, 0, USB_FS_MAX_PACKET_SIZE, PCD_SNG_BUF}};

uint8_t USB_PMA_GetNumEndpointsSlots()
{
  uint8_t eps = USB_MAX_EPS_SLOTS;
  for (uint8_t i = 1; i < USB_MAX_EPS_SLOTS; i++)
  {
    if (ep_def[i].ep_size == 0)
    {
      eps = i;
      break;
    }
  }
  return eps;
}

uint32_t USB_PMA_GetNextEndpoint()
{
  uint32_t maxep = 0;
  for (uint8_t i = 1; i < USB_MAX_EPS_SLOTS; i++)
  {
    if (ep_def[i].ep_size != 0)
    {
      maxep = MAX(maxep, SMALL_EP(ep_def[i].ep_num));
    }
    else
    {
      break;
    }
  }
  if (maxep >= USB_MAX_EPS)
  {
    return 0;
  }
  return maxep;
}

uint8_t USB_PMA_GetNextEndpointSlot()
{
  uint8_t eps = USB_PMA_GetNumEndpointsSlots();
  if (eps >= USB_MAX_EPS_SLOTS)
  {
    return 0;
  }
  return eps;
}

int USB_PMA_GetEndpointsSize()
{
  int size = 0;
  for (uint8_t i = 0; i < USB_MAX_EPS_SLOTS; i++)
  {
    size += ep_def[i].ep_kind == PCD_DBL_BUF ? ep_def[i].ep_size * 2 : ep_def[i].ep_size;
  }
  return size;
}

#endif /* HAL_PCD_MODULE_ENABLED && USBCON */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
