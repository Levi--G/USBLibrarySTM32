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
#ifdef USBCON

/* Includes ------------------------------------------------------------------*/
#include "Arduino.h"
#include "USBCore_stm32.h"
#include "usbd_desc.h"
#include "USBAPI.h"
#include "PluggableUSB.h"
#include "PacketBuffer.h"

#define SMALL_EP(Ep) (Ep & 0xFU)

static uint8_t USBD_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t *USBD_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_HID_GetDeviceQualifierDesc(uint16_t *length);
static uint8_t USBD_HID_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t endp);

bool PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t endp);

USBD_ClassTypeDef USBD_HID_CLASS = {
    USBD_HID_Init,
    USBD_HID_DeInit,
    USBD_HID_Setup,
    NULL,                 /* EP0_TxSent */
    USBD_HID_EP0_RxReady, /* EP0_RxReady */
    USBD_HID_DataIn,      /* DataIn */
    USBD_HID_DataOut,     /* DataOut */
    NULL,                 /* SOF */
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

/* RX buffers */
PacketBuffer *RX_Buffers[USB_ENDPOINTS];
bool RX_Buffer_Pending[USB_ENDPOINTS];
bool Recv_EP0 = false;

/* USB Device Core HID composite handle declaration */
USBD_HandleTypeDef hUSBD_Device_HID_Handle;

bool HID_initialized = false;

void USB_Begin()
{
  if (!HID_initialized)
  {
    /* Init Device Library */
    if (USBD_Init(&hUSBD_Device_HID_Handle, &USBD_Desc, 0) == USBD_OK)
    {
      /* Add Supported Class */
      if (USBD_RegisterClass(&hUSBD_Device_HID_Handle, &USBD_HID_CLASS) == USBD_OK)
      {
        /* Start Device Process */
        USBD_Start(&hUSBD_Device_HID_Handle);
        HID_initialized = true;
      }
    }
  }
}

bool GetHHID(USBD_HID_HandleTypeDef *&hhid)
{
  hhid = (USBD_HID_HandleTypeDef *)hUSBD_Device_HID_Handle.pClassData;
  return hhid != NULL;
}

bool USB_Running()
{
  USBD_HID_HandleTypeDef *hhid;
  return GetHHID(hhid);
}

void USB_End()
{
  if (HID_initialized)
  {
    /* Stop Device Process */
    USBD_Stop(&hUSBD_Device_HID_Handle);
    /* DeInit Device Library */
    USBD_DeInit(&hUSBD_Device_HID_Handle);
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
    return &hUSBD_Device_HID_Handle.ep_in[SMALL_EP(ep)];
  }
  return &hUSBD_Device_HID_Handle.ep_out[SMALL_EP(ep)];
}

bool USB_SendAvailable(uint8_t endp)
{
  USBD_HID_HandleTypeDef *hhid;
  return GetHHID(hhid) && hUSBD_Device_HID_Handle.dev_state == USBD_STATE_CONFIGURED && hhid->EPstate[SMALL_EP(endp)] != HID_BUSY;
}

int USB_SendQuick(uint8_t endp, const void *data, int len)
{
  USBD_HID_HandleTypeDef *hhid;
  if (USB_SendAvailable(endp) && GetHHID(hhid))
  {
    uint8_t ep = SMALL_EP(endp);
    uint8_t EP = GetFullEP(endp);
    hhid->EPstate[ep] = HID_BUSY;
    (void)USBD_LL_Transmit(&hUSBD_Device_HID_Handle, EP, (uint8_t *)data, len);
  }
  else
  {
    return 0;
  }
  return len;
}

int USB_Send(uint8_t endp, const void *data, int len)
{
  USBD_HID_HandleTypeDef *hhid;
  if (GetHHID(hhid))
  {
    while (!USB_SendAvailable(endp))
    {
      delay(1);
    }
    int ret = USB_SendQuick(endp, data, len);
    // USB_Send is blocking in the original avr implementation
    while (!USB_SendAvailable(endp))
    {
      delay(1);
    }
    return ret;
  }
  return 0;
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
  return (USBD_CtlSendData(&hUSBD_Device_HID_Handle, (uint8_t *)d, len) == USBD_OK) ? len : 0;
}

uint8_t USB_SendSpace(uint8_t endp)
{
  /* Only kept for compatibility */
  return USB_EP_SIZE;
}

int USB_SendZLP(uint8_t endp)
{
  return USB_Send(endp, NULL, 0);
}

void USB_Flush(uint8_t endp)
{
  uint8_t ep = SMALL_EP(endp);
  uint8_t EP = GetFullEP(endp);
  USBD_LL_FlushEP(&hUSBD_Device_HID_Handle, EP);
  USBD_HID_HandleTypeDef *hhid;
  if (GetHHID(hhid))
  {
    while (hhid->EPstate[ep] == HID_BUSY)
    {
      delay(1);
    }
  }
}

uint8_t USB_Available(uint8_t endp)
{
  uint8_t ep = SMALL_EP(endp);
  if (RX_Buffers[ep]->size())
  {
    auto buf = RX_Buffers[ep]->read();
    return buf->len - buf->pos;
  }
  return 0;
}

int USB_Recv(uint8_t endp, void *data, int len)
{
  uint8_t ep = SMALL_EP(endp);
  uint8_t EP = GetFullEP(ep);
  int read = 0;
  if (RX_Buffers[ep]->size())
  {
    auto buf = RX_Buffers[ep]->read();
    read = min(buf->len - buf->pos, len);
    if (read)
    {
      memcpy(data, buf->buf + buf->pos, read);
      buf->pos += read;
    }
    if (buf->pos >= buf->len)
    {
      RX_Buffers[ep]->shift();
      PrepareReceive(&hUSBD_Device_HID_Handle, EP);
    }
  }
  return read;
}

int USB_Recv(uint8_t ep)
{
  uint8_t c;
  if (USB_Recv(ep, &c, 1) != 1)
    return -1;
  return c;
}

int USB_RecvControl(void *data, int len)
{
  int read = 0;
  if (!RX_Buffers[0]->size())
  {
    if (RX_Buffers[0]->available())
    {
      auto buf = RX_Buffers[0]->reserve();
      Recv_EP0 = true;
      USBD_CtlPrepareRx(&hUSBD_Device_HID_Handle, buf->buf, len);
      while (buf->len == 0)
      {
        delay(1);
      }
    }
  }
  if (RX_Buffers[0]->size())
  {
    auto buf = RX_Buffers[0]->read();
    read = min(buf->len - buf->pos, len);
    if (read)
    {
      memcpy(data, buf->buf + buf->pos, read);
      buf->pos += read;
    }
    if (buf->pos >= buf->len)
    {
      RX_Buffers[0]->shift();
    }
  }
  return read;
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

  RX_Buffers[0] = new PacketBuffer();

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
      if (_initEndpoints[ep] & USB_ENDPOINT_DIRECTION_MASK)
      {
        RX_Buffers[ep] = new PacketBuffer();
        USBD_LL_PrepareReceive(pdev, EP, RX_Buffers[ep]->reserve()->buf, USB_FS_MAX_PACKET_SIZE);
        RX_Buffer_Pending[ep] = true;
      }
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
    if (RX_Buffers[ep])
    {
      delete RX_Buffers[ep];
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
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_HID_Setup(USBD_HandleTypeDef *pdev,
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

static uint8_t USBD_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  if (Recv_EP0)
  {
  }
  return (uint8_t)USBD_OK;
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
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t endp)
{
  uint8_t ep = SMALL_EP(endp);
  auto buf = RX_Buffers[ep]->reserve();
  buf->pos = 0;
  buf->len = USBD_LL_GetRxDataSize(pdev, endp);
  RX_Buffers[ep]->commit();
  RX_Buffer_Pending[ep] = false;
  if (!PrepareReceive(pdev, endp))
  {
    USBD_LL_PrepareReceive(pdev, endp, 0, 0);
  }
  return (uint8_t)USBD_OK;
}

bool PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t endp)
{
  uint8_t ep = SMALL_EP(endp);
  if (RX_Buffer_Pending[ep] == false && RX_Buffers[ep]->available())
  {
    USBD_LL_PrepareReceive(pdev, endp, RX_Buffers[ep]->reserve()->buf, USB_FS_MAX_PACKET_SIZE);
    RX_Buffer_Pending[ep] = true;
    return true;
  }
  return false;
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
