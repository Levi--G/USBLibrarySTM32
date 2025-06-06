/*
 * usbd_conf.c
 * Template generated with Stm32CubeMX "usbd_conf.c":
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 * Implementation/modification:
 * Copyright (C) 2022-2025 Levi Gillis - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GNU Lesser General Public License v3.0 license.
 */

#ifdef USBCON
/* Includes ------------------------------------------------------------------*/
#include "USB_EP_conf.h"
#include "stm32yyxx_ll_pwr.h"
#include "usbd_core.h"
#include "usbd_if.h"

#ifndef HAL_PCD_MODULE_ENABLED
#error "HAL_PCD_MODULE_ENABLED is required"
#else
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if !defined(USBD_VBUS_DETECTION_ENABLE)
#define VBUS_SENSING DISABLE
#else
#define VBUS_SENSING ENABLE
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
PCD_HandleTypeDef g_hpcd;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
                       PCD BSP Routines
*******************************************************************************/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  const PinMap *map = NULL;
#if defined(PWR_CR2_USV) || defined(PWR_SVMCR_USV)
  /* Enable VDDUSB on Pwrctrl CR2 register*/
  HAL_PWREx_EnableVddUSB();
#endif
#ifdef STM32H7xx
  if (!LL_PWR_IsActiveFlag_USB())
  {
    HAL_PWREx_EnableUSBVoltageDetector();
  }
#endif
#if defined(USB)
  if (hpcd->Instance == USB)
  {

    /* Configure USB FS GPIOs */
    map = PinMap_USB;
    while (map->pin != NC)
    {
      pin_function(map->pin, map->function);
      map++;
    }

    /* Enable USB FS Clock */
    __HAL_RCC_USB_CLK_ENABLE();

#if defined(SYSCFG_CFGR1_USB_IT_RMP) && defined(USE_USB_INTERRUPT_REMAPPED)
    /* USB interrupt remapping enable */
    __HAL_REMAPINTERRUPT_USB_ENABLE();
#endif

#if defined(USB_H_IRQn)
    /* Set USB High priority Interrupt priority */
    HAL_NVIC_SetPriority(USB_HP_IRQn, USBD_IRQ_PRIO, USBD_IRQ_SUBPRIO);
    /* Enable USB High priority Interrupt */
    HAL_NVIC_EnableIRQ(USB_HP_IRQn);
#endif
    /* Set USB Interrupt priority */
    HAL_NVIC_SetPriority(USB_IRQn, USBD_IRQ_PRIO, USBD_IRQ_SUBPRIO);

    /* Enable USB Interrupt */
    HAL_NVIC_EnableIRQ(USB_IRQn);

    if (hpcd->Init.low_power_enable == 1)
    {
      /* Enable EXTI for USB wakeup */
#ifdef __HAL_USB_WAKEUP_EXTI_CLEAR_FLAG
      __HAL_USB_WAKEUP_EXTI_CLEAR_FLAG();
#endif
#ifdef __HAL_USB_WAKEUP_EXTI_ENABLE_RISING_EDGE
      __HAL_USB_WAKEUP_EXTI_ENABLE_RISING_EDGE();
#endif
      __HAL_USB_WAKEUP_EXTI_ENABLE_IT();
#if defined(USB_WKUP_IRQn)
      /* USB Wakeup Interrupt */
      HAL_NVIC_EnableIRQ(USB_WKUP_IRQn);

      /* Enable USB Wake-up interrupt */
      HAL_NVIC_SetPriority(USB_WKUP_IRQn, 0, 0);
#endif
    }
  }
#endif /* USB */
#if defined(USB_OTG_FS)
  if (hpcd->Instance == USB_OTG_FS)
  {

    /* Configure USB FS GPIOs */
    map = PinMap_USB_OTG_FS;
    while (map->pin != NC)
    {
      pin_function(map->pin, map->function);
      map++;
    }

    /* Enable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Set USB FS Interrupt priority */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, USBD_IRQ_PRIO, USBD_IRQ_SUBPRIO);

    /* Enable USB FS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    if (hpcd->Init.low_power_enable == 1)
    {
      /* Enable EXTI Line 18 for USB wakeup */
#ifdef __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG
      __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG();
#endif
#ifdef __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_EDGE
      __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_EDGE();
#endif
#ifdef __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_IT
      __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_IT();
#endif
#if !defined(STM32L4xx) && !defined(STM32U5xx)
      /* Set EXTI Wakeup Interrupt priority */
      HAL_NVIC_SetPriority(OTG_FS_WKUP_IRQn, USBD_IRQ_PRIO, USBD_IRQ_SUBPRIO);

      /* Enable EXTI Interrupt */
      HAL_NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
#endif
    }
  }
#endif /* USB_OTG_FS */
#if defined(USB_OTG_HS)
  if (hpcd->Instance == USB_OTG_HS)
  {
    /* Configure USB HS GPIOs */
    map = PinMap_USB_OTG_HS;
    while (map->pin != NC)
    {
      pin_function(map->pin, map->function);
      map++;
    }
#ifndef USE_USB_HS_IN_FS
    __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
#endif /* USE_USB_HS_IN_FS */

    /* Enable USB HS Clocks */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

    /* Set USB HS Interrupt priority */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, USBD_IRQ_PRIO, USBD_IRQ_SUBPRIO);

    /* Enable USB HS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);

    if (hpcd->Init.low_power_enable == 1)
    {
      /* Enable EXTI Line 20 for USB wakeup */
#ifdef __HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG
      __HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG();
#endif
#ifdef __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_RISING_EDGE
      __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_RISING_EDGE();
#endif
      __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_IT();

      /* Set EXTI Wakeup Interrupt priority */
      HAL_NVIC_SetPriority(OTG_HS_WKUP_IRQn, USBD_IRQ_PRIO, USBD_IRQ_SUBPRIO);

      /* Enable EXTI Interrupt */
      HAL_NVIC_EnableIRQ(OTG_HS_WKUP_IRQn);
    }
  }
#endif /* USB_OTG_HS */
}

/**
 * @brief  De-Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  /* Disable USB FS Clock */
#if defined(USB)
  if (hpcd->Instance == USB)
  {
    __HAL_RCC_USB_CLK_DISABLE();
  }
#endif
#if defined(USB_OTG_FS)
  if (hpcd->Instance == USB_OTG_FS)
  {
    /* Disable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
  }
#endif
#if defined(USB_OTG_HS)
  if (hpcd->Instance == USB_OTG_HS)
  {
    /* Disable USB HS Clocks */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
  }
#endif /* USB_OTG_HS */
}

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/

/**
 * @brief  SetupStage callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup);
}

/**
 * @brief  DataOut Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
 * @brief  DataIn Stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
 * @brief  SOF callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_SOF(hpcd->pData);
}

/**
 * @brief  Reset callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

#if defined(USB_OTG_HS)
  /* Set USB Current Speed */
  switch (hpcd->Init.speed)
  {
  case PCD_SPEED_HIGH:
    speed = USBD_SPEED_HIGH;
    break;

  case PCD_SPEED_FULL:
    speed = USBD_SPEED_FULL;
    break;

  default:
    speed = USBD_SPEED_FULL;
    break;
  }
#endif

  USBD_LL_SetSpeed(hpcd->pData, speed);
  /* Reset Device */
  USBD_LL_Reset(hpcd->pData);
}

/**
 * @brief  Suspend callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_Suspend(hpcd->pData);
  __HAL_PCD_GATE_PHYCLOCK(hpcd);

  /*Enter in STOP mode */
  if (hpcd->Init.low_power_enable)
  {
    /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register */
    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
}

/**
 * @brief  Resume callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
  if (hpcd->Init.low_power_enable)
  {
    USBD_SystemClockConfigFromResume();

    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t) ~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
  __HAL_PCD_UNGATE_PHYCLOCK(hpcd);
  USBD_LL_Resume(hpcd->pData);
}

/**
 * @brief  ISOOUTIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ISOINIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
  USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

/**
 * @brief  ConnectCallback callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevConnected(hpcd->pData);
}

/**
 * @brief  Disconnect callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
  USBD_LL_DevDisconnected(hpcd->pData);
}

/**
 * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
 * @param  None
 * @retval None
 */
#ifdef USE_USB_HS
void OTG_HS_IRQHandler(void)
#elif defined(USB_OTG_FS)
void OTG_FS_IRQHandler(void)
#else /* USB */
void USB_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&g_hpcd);
}

#if defined(USB_H_IRQn)
/**
 * @brief This function handles USB high priority interrupt.
 * @param  None
 * @retval None
 */
void USB_H_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&g_hpcd);
}
#endif /* USB_H_IRQn */

/**
 * @brief  This function handles USB Wakeup IRQ Handler.
 * @param  None
 * @retval None
 */
#ifdef USE_USB_HS
void OTG_HS_WKUP_IRQHandler(void)
#elif defined(USB_OTG_FS)
void OTG_FS_WKUP_IRQHandler(void)
#elif defined(USB_WKUP_IRQHandler)
void USB_WKUP_IRQHandler(void)
#else
void USBWakeUp_IRQHandler_dummy(void)
#endif
{
  if ((&g_hpcd)->Init.low_power_enable)
  {
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t) ~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

    /* Configures system clock after wake-up */
    USBD_SystemClockConfigFromResume();

    /* ungate PHY clock */
    __HAL_PCD_UNGATE_PHYCLOCK((&g_hpcd));
  }
#if defined(USE_USB_HS) && defined(__HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG)
  /* Clear EXTI pending Bit*/
  __HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG();
#elif defined(USB_OTG_FS) && defined(__HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG)
  /* Clear EXTI pending Bit*/
  __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG();
#elif defined(__HAL_USB_WAKEUP_EXTI_CLEAR_FLAG)
  __HAL_USB_WAKEUP_EXTI_CLEAR_FLAG();
#endif
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/
/**
 * @brief  Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
  USBD_reenumerate();
  /* Set common LL Driver parameters */
  g_hpcd.Init.dev_endpoints = USB_EP_GetNumEndpoints();
#ifdef DEP0CTL_MPS_64
  g_hpcd.Init.ep0_mps = DEP0CTL_MPS_64;
#else
#ifdef EP_MPS_64
  g_hpcd.Init.ep0_mps = EP_MPS_64;
#else
#error "Missing EP0 MPS definition: DEP0CTL_MPS_64 or EP_MPS_64!"
#endif
#endif
#if !defined(STM32F1xx) && !defined(STM32F2xx) || defined(USB)
  g_hpcd.Init.lpm_enable = DISABLE;
  g_hpcd.Init.battery_charging_enable = DISABLE;
#endif
  g_hpcd.Init.low_power_enable = DISABLE;
  g_hpcd.Init.Sof_enable = DISABLE;

  /* Set specific LL Driver parameters */
#ifdef USE_USB_HS
  g_hpcd.Instance = USB_OTG_HS;
  g_hpcd.Init.use_dedicated_ep1 = DISABLE;
  g_hpcd.Init.dma_enable = DISABLE;
#ifdef USE_USB_HS_IN_FS
  g_hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
#else
  g_hpcd.Init.phy_itface = PCD_PHY_ULPI;
#endif
  g_hpcd.Init.speed = PCD_SPEED_HIGH;
  g_hpcd.Init.vbus_sensing_enable = VBUS_SENSING;
  g_hpcd.Init.use_external_vbus = DISABLE;
#else /* USE_USB_FS */
#ifdef USB_OTG_FS
  g_hpcd.Instance = USB_OTG_FS;
  g_hpcd.Init.use_dedicated_ep1 = DISABLE;
  g_hpcd.Init.dma_enable = DISABLE;
  g_hpcd.Init.vbus_sensing_enable = VBUS_SENSING;
  g_hpcd.Init.use_external_vbus = DISABLE;
#else
  g_hpcd.Instance = USB;
#endif
  g_hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
  g_hpcd.Init.speed = PCD_SPEED_FULL;
#endif /* USE_USB_HS */

  /* Link The driver to the stack */
  g_hpcd.pData = pdev;
  pdev->pData = &g_hpcd;

  /* Initialize LL Driver */
  if (HAL_PCD_Init(&g_hpcd) != HAL_OK)
  {
    Error_Handler();
  }

#if !defined(USB)

  uint8_t eps = USB_EP_GetNumEndpoints();
  int32_t freemem = PMA_MAX_SIZE - USB_ABS_EP0_SIZE - USB_ABS_EP0_TX_SIZE - (USB_ABS_EP_SIZE * eps);
  if (freemem < 0)
  {
    // this is really not good so bail here
    return USBD_EMEM;
  }
  /* configure EPs FIFOs */
  // we will use all the extra free memory as extra RX buffer
  HAL_PCDEx_SetRxFiFo(&g_hpcd, USB_ABS_EP0_SIZE + freemem);
  HAL_PCDEx_SetTxFiFo(&g_hpcd, 0, USB_ABS_EP0_TX_SIZE);

  const ep_desc_t *epdefs = USB_EP_GetEndpointsSlots();
  for (uint32_t i = 0; i < eps; i++)
  {
    if (IS_IN_EP(epdefs[i].ep_num))
    {
      HAL_PCDEx_SetTxFiFo(&g_hpcd, epdefs[i].ep_num & 0xF, USB_ABS_EP_SIZE);
    }
  }
#else
  uint32_t currentaddress = PMA_BASE_ADDR;

  uint32_t address = currentaddress;
#if PCD_USE_DBL_BUF
  address = (currentaddress << 16U) | (currentaddress + USB_EP0_SIZE);
#endif
  HAL_PCDEx_PMAConfig(&g_hpcd, 0x00, PCD_DEF_BUF, address);
  currentaddress += USB_ABS_EP0_SIZE;
#if PCD_USE_DBL_BUF
  address = (currentaddress << 16U) | (currentaddress + USB_EP0_SIZE);
#else
  address = currentaddress;
#endif
  HAL_PCDEx_PMAConfig(&g_hpcd, 0x80, PCD_DEF_BUF, address);
  currentaddress += USB_ABS_EP0_TX_SIZE;

  uint8_t eps = USB_EP_GetNumEndpoints();
  const ep_desc_t *epdefs = USB_EP_GetEndpointsSlots();

  for (uint32_t i = 0; i < eps; i++)
  {
#if PCD_USE_DBL_BUF
    address = (currentaddress << 16U) | (currentaddress + USB_EP_SIZE);
#else
    address = currentaddress;
#endif
    HAL_PCDEx_PMAConfig(&g_hpcd, epdefs[i].ep_num, PCD_DEF_BUF, address);
    currentaddress += USB_ABS_EP_SIZE;
  }
#endif /* USE_USB_HS */
  return USBD_OK;
}

/**
 * @brief  De-Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
  HAL_PCD_DeInit(pdev->pData);
  return USBD_OK;
}

/**
 * @brief  Starts the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
  HAL_PCD_Start(pdev->pData);
  return USBD_OK;
}

/**
 * @brief  Stops the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
  HAL_PCD_Stop(pdev->pData);
  return USBD_OK;
}

/**
 * @brief  Opens an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  ep_type: Endpoint Type
 * @param  ep_mps: Endpoint Max Packet Size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev,
                                  uint8_t ep_addr,
                                  uint8_t ep_type,
                                  uint16_t ep_mps)
{
  HAL_PCD_EP_Open(pdev->pData,
                  ep_addr,
                  ep_mps,
                  ep_type);
  return USBD_OK;
}

/**
 * @brief  Closes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_Close(pdev->pData, ep_addr);
  return USBD_OK;
}

/**
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_Flush(pdev->pData, ep_addr);
  return USBD_OK;
}

/**
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
  return USBD_OK;
}

/**
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
  return USBD_OK;
}

/**
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Stall (1: Yes, 0: No)
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  PCD_HandleTypeDef *hpcd = pdev->pData;

  if ((ep_addr & 0x80) == 0x80)
  {
    return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
  }
  else
  {
    return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
  }
}

/**
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  dev_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
  HAL_PCD_SetAddress(pdev->pData, dev_addr);
  return USBD_OK;
}

/**
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev,
                                    uint8_t ep_addr,
                                    uint8_t *pbuf,
                                    uint32_t size)
{
  HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
  return USBD_OK;
}

/**
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
                                          uint8_t ep_addr,
                                          uint8_t *pbuf,
                                          uint32_t size)
{
  HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
  return USBD_OK;
}

/**
 * @brief  Returns the last transferred packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Received Data Size
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
  return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

/**
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 */
void USBD_LL_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}
#endif /* HAL_PCD_MODULE_ENABLED */
#endif /* USBCON */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
