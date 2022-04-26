/**
  ******************************************************************************
  * @file    USBCore.cpp
  * @author  MCD Application Team
  * @brief   This file provides the HID core functions.
  ******************************************************************************
  * @attention
  * 
  * Modified by Levi Gillis @ 2022
  * Adjusted and reimplemented for compatibility with arduino
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

#ifdef USBCON
#ifdef USBD_USE_HID_COMPOSITE

/* Includes ------------------------------------------------------------------*/
#include "Arduino.h"
#include "USBCore_stm32.h"
#include "usbd_desc.h"
#include "USBAPI.h"
#include "PluggableUSB.h"

#define SMALL_EP(Ep) (Ep & 0xFU)

static uint8_t USBD_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t *USBD_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length);
static uint8_t USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

USBD_ClassTypeDef USBD_COMPOSITE_HID = {
    USBD_HID_Init,
    USBD_HID_DeInit,
    USBD_COMPOSITE_HID_Setup,
    NULL,            /* EP0_TxSent */
    NULL,            /* EP0_RxReady */
    USBD_HID_DataIn, /* DataIn */
    NULL,            /* DataOut */
    NULL,            /* SOF */
    NULL,
    NULL,
    USBD_HID_GetHSCfgDesc,
    USBD_HID_GetFSCfgDesc,
    USBD_HID_GetOtherSpeedCfgDesc,
    USBD_HID_GetDeviceQualifierDesc,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};

/* Buffers for properly handling interface and report descriptors */
uint8_t _initEndpoints[USB_ENDPOINTS];
#define TEMPCFGBUFFER_LEN 128
__ALIGN_BEGIN uint8_t tempcfgBuffer[TEMPCFGBUFFER_LEN] __ALIGN_END;
uint8_t tempcfgbufferpos = 0;
bool cfgBufferMode = false;
__ALIGN_BEGIN uint8_t tempdescBuffer[TEMPCFGBUFFER_LEN] __ALIGN_END;
uint8_t tempdescbufferpos = 0;
bool descBufferMode = false;

/* USB Device Core HID composite handle declaration */
USBD_HandleTypeDef hUSBD_Device_HID;

bool HID_initialized = false;

void USB_Begin()
{
  if (!HID_initialized)
  {
    /* Init Device Library */
    if (USBD_Init(&hUSBD_Device_HID, &USBD_Desc, 0) == USBD_OK)
    {
      /* Add Supported Class */
      if (USBD_RegisterClass(&hUSBD_Device_HID, &USBD_COMPOSITE_HID) == USBD_OK)
      {
        /* Start Device Process */
        USBD_Start(&hUSBD_Device_HID);
        HID_initialized = true;
      }
    }
  }
}

bool USB_Running()
{
  return HID_initialized;
}

void USB_End()
{
  if (HID_initialized)
  {
    /* Stop Device Process */
    USBD_Stop(&hUSBD_Device_HID);
    /* DeInit Device Library */
    USBD_DeInit(&hUSBD_Device_HID);
    HID_initialized = false;
  }
}

bool IsEPIN(uint8_t ep)
{
  return (_initEndpoints[SMALL_EP(ep)] & USB_ENDPOINT_DIRECTION_MASK);
}

uint8_t GetFullEP(uint8_t ep)
{
  return IsEPIN(ep) ? USB_ENDPOINT_IN(SMALL_EP(ep)) : SMALL_EP(ep);
}

USBD_EndpointTypeDef *GetEPTypeDef(uint8_t ep)
{
  if (IsEPIN(ep))
  {
    return &hUSBD_Device_HID.ep_in[SMALL_EP(ep)];
  }
  return &hUSBD_Device_HID.ep_out[SMALL_EP(ep)];
}

int USB_Send(uint8_t endp, const void *data, int len)
{
  USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef *)hUSBD_Device_HID.pClassData;

  if (hhid == NULL)
  {
    return 0;
  }

  uint8_t ep = SMALL_EP(endp);
  uint8_t EP = GetFullEP(endp);

  if (hUSBD_Device_HID.dev_state == USBD_STATE_CONFIGURED)
  {
    if (hhid->EPstate[ep] == HID_IDLE)
    {
      hhid->EPstate[ep] = HID_BUSY;
      (void)USBD_LL_Transmit(&hUSBD_Device_HID, EP, (uint8_t *)data, len);
    }
    else
    {
      return 0;
    }
    // USB_Send is blocking in the original avr implementation
    while (hhid->EPstate[ep] == HID_BUSY)
    {
      delay(1);
    }
  }
  return len;
}

int USB_SendControl(uint8_t flags, const void *d, int len)
{
  if (cfgBufferMode)
  {
    if (tempcfgbufferpos + len < TEMPCFGBUFFER_LEN)
    {
      uint8_t cpy = 0;
      uint8_t *ptr = (uint8_t *)d;
      for (uint8_t i = 0; i < len;)
      {
        if (ptr[i] == 0)
        {
          i++;
        }
        else if (ptr[i] + i <= len)
        {
          memcpy(tempcfgBuffer + tempcfgbufferpos, ptr + i, ptr[i]);
          tempcfgbufferpos += ptr[i];
          cpy += ptr[i];
          i += ptr[i];
        }
      }
      return cpy;
    }
  }
  if (descBufferMode)
  {
    if (tempdescbufferpos + len < TEMPCFGBUFFER_LEN)
    {
      memcpy(tempdescBuffer + tempdescbufferpos, d, len);
      tempdescbufferpos += len;
      return len;
    }
  }
  return (USBD_CtlSendData(&hUSBD_Device_HID, (uint8_t *)d, len) == USBD_OK) ? len : 0;
}

uint8_t USB_SendSpace(uint8_t endp)
{
  /* Only kept for compatibility */
  return USB_EP_SIZE;
}

void USB_Flush(uint8_t endp)
{
  uint8_t EP = GetFullEP(endp);
  USBD_LL_FlushEP(&hUSBD_Device_HID, EP);
}

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_HID_Init(USBD_HandleTypeDef *pdev,
                             uint8_t cfgidx)
{
  UNUSED(cfgidx);

  USBD_HID_HandleTypeDef *hhid;

  hhid = (USBD_HID_HandleTypeDef *)USBD_malloc(sizeof(USBD_HID_HandleTypeDef));

  if (hhid == NULL)
  {
    pdev->pClassData = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassData = (void *)hhid;

  for (size_t ep = 0; ep < sizeof(_initEndpoints); ep++)
  {
    if (_initEndpoints[ep] != 0)
    {
      uint8_t EP = GetFullEP(ep);
      USBD_EndpointTypeDef *epdef = GetEPTypeDef(ep);
      if (pdev->dev_speed == USBD_SPEED_HIGH)
      {
        epdef->bInterval = HID_HS_BINTERVAL;
      }
      else
      {
        epdef->bInterval = HID_FS_BINTERVAL;
      }
      (void)USBD_LL_OpenEP(pdev, EP, _initEndpoints[ep] & USB_ENDPOINT_TYPE_MASK,
                           USB_EP_SIZE); //TODO
      epdef->is_used = 1U;
      hhid->EPstate[ep] = HID_IDLE;
    }
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_HID_DeInit
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx)
{
  UNUSED(cfgidx);

  for (size_t ep = 0; ep < sizeof(_initEndpoints); ep++)
  {
    if (_initEndpoints[ep] != 0)
    {
      uint8_t EP = GetFullEP(ep);
      USBD_EndpointTypeDef *epdef = GetEPTypeDef(ep);
      (void)USBD_LL_CloseEP(pdev, EP);
      epdef->is_used = 0U;
      epdef->bInterval = 0U;
    }
  }

  /* Free allocated memory */
  if (pdev->pClassData != NULL)
  {
    (void)USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_COMPOSITE_HID_Setup(USBD_HandleTypeDef *pdev,
                                        USBD_SetupReqTypedef *req)
{
  USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef *)pdev->pClassData;
  USBD_StatusTypeDef ret = USBD_OK;
  uint16_t len = 0U;
  uint8_t *pbuf = NULL;
  uint16_t status_info = 0U;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  USBSetup setup = {req->bmRequest, req->bRequest, lowByte(req->wValue), highByte(req->wValue), req->wIndex, req->wLength};
  tempdescbufferpos = 0;
  descBufferMode = true;
  auto result = PluggableUSB().getDescriptor(setup);
  descBufferMode = false;

  if (result && tempdescbufferpos > 0)
  {
    USB_SendControl(0, tempdescBuffer, min((uint16_t)tempdescbufferpos, req->wLength));
    return USBD_OK;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS:
    switch (req->bRequest)
    {
    case HID_REQ_SET_PROTOCOL:
      hhid->Protocol = (uint8_t)(req->wValue);
      break;

    case HID_REQ_GET_PROTOCOL:
      (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->Protocol, 1U);
      break;

    case HID_REQ_SET_IDLE:
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;

    case HID_REQ_GET_IDLE:
      (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->IdleState, 1U);
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
    }
    break;
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_STATUS:
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
      }
      else
      {
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
      }
      break;
    case USB_REQ_GET_DESCRIPTOR:
      /* Should be handled previously */
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;

    case USB_REQ_GET_INTERFACE:
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->AltSetting, 1U);
      }
      else
      {
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_SET_INTERFACE:
      if (pdev->dev_state == USBD_STATE_CONFIGURED)
      {
        hhid->AltSetting = (uint8_t)(req->wValue);
      }
      else
      {
        USBD_CtlError(pdev, req);
        ret = USBD_FAIL;
      }
      break;

    case USB_REQ_CLEAR_FEATURE:
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
    }
    break;

  default:
    USBD_CtlError(pdev, req);
    ret = USBD_FAIL;
    break;
  }

  return (uint8_t)ret;
}

/**
  * @brief  USBD_HID_GetCfgFSDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_HID_GetFSCfgDesc(uint16_t *length)
{
  /* Since this needs to be returned as pointer to an aligned array
  we will store the "sent" configuration and send the filled buffer instead */
  auto confsize = USB_CONFIGUARTION_DESC_SIZE;
  tempcfgbufferpos = confsize;
  cfgBufferMode = true;
  u8 interfaces = 0;
  *length = PluggableUSB().getInterface(&interfaces);
  *length += confsize;
  cfgBufferMode = false;
  ConfigDescriptor config = D_CONFIG(*length, interfaces);
  memcpy(tempcfgBuffer, &config, confsize);
  return tempcfgBuffer;
}

/**
  * @brief  USBD_HID_GetCfgHSDesc
  *         return HS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_HID_GetHSCfgDesc(uint16_t *length)
{
  //should not happen/matter => return FS
  return USBD_HID_GetFSCfgDesc(length);
}

/**
  * @brief  USBD_HID_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length)
{
  //should not happen/matter => return FS
  return USBD_HID_GetFSCfgDesc(length);
}

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_HID_DataIn(USBD_HandleTypeDef *pdev,
                               uint8_t endp)
{
  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */

  uint8_t ep = SMALL_EP(endp);
  ((USBD_HID_HandleTypeDef *)pdev->pClassData)->EPstate[ep] = HID_IDLE;
  return (uint8_t)USBD_OK;
}

/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length)
{
  Serial1.println("DeviceQ");
  *length = (uint16_t)sizeof(USBD_HID_DeviceQualifierDesc);
  return USBD_HID_DeviceQualifierDesc;
}
#endif /* USBD_USE_HID_COMPOSITE */
#endif /* USBCON */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
