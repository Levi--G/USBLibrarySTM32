/*
 * usbd_conf.h
 * Template generated with Stm32CubeMX "usbd_conf.h":
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

#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef USBCON

/* Includes ------------------------------------------------------------------*/
#include "stm32_def.h"

#if !defined(USB_BASE) && !defined(USB_OTG_DEVICE_BASE)
#error "This board does not support USB! Select 'None' in the 'Tools->USB interface' menu"
#endif
#if defined(USE_USB_HS) && !defined(USB_OTG_HS)
#error "This board does not support USB High Speed! Select 'Full Speed' in the 'Tools->USB interface' menu"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(USB_BASE)

#if defined(STM32F1xx) || defined(STM32F3xx) || defined(STM32G4xx) || \
    defined(STM32L1xx) || defined(STM32WBxx)
#if defined(SYSCFG_CFGR1_USB_IT_RMP) && !defined(USE_USB_INTERRUPT_REMAPPED)
#define USB_IRQn            USB_LP_CAN_RX0_IRQn
#define USB_IRQHandler      USB_LP_CAN_RX0_IRQHandler
#define USB_H_IRQn          USB_HP_CAN_TX_IRQn
#define USB_H_IRQHandler    USB_HP_CAN_TX_IRQHandler
#define USB_WKUP_IRQn       USBWakeUp_IRQn
#define USB_WKUP_IRQHandler USBWakeUp_IRQHandler
#else
#define USB_IRQn         USB_LP_IRQn
#define USB_IRQHandler   USB_LP_IRQHandler
#define USB_H_IRQn       USB_HP_IRQn
#define USB_H_IRQHandler USB_HP_IRQHandler
#if defined(SYSCFG_CFGR1_USB_IT_RMP) && defined(USE_USB_INTERRUPT_REMAPPED)
#define USB_WKUP_IRQn       USBWakeUp_RMP_IRQn
#define USB_WKUP_IRQHandler USBWakeUp_RMP_IRQHandler
#elif defined(STM32F1xx) || defined(STM32F3xx) || defined(STM32G4xx)
#define USB_WKUP_IRQn       USBWakeUp_IRQn
#define USB_WKUP_IRQHandler USBWakeUp_IRQHandler
#elif defined(STM32L1xx)
#define USB_WKUP_IRQn       USB_FS_WKUP_IRQn
#define USB_WKUP_IRQHandler USB_FS_WKUP_IRQHandler
#endif
#endif
#elif defined(STM32G0xx)
#define USB_IRQn       USB_UCPD1_2_IRQn
#define USB_IRQHandler USB_UCPD1_2_IRQHandler
#elif defined(STM32L5xx)
#define USB_IRQn       USB_FS_IRQn
#define USB_IRQHandler USB_FS_IRQHandler
#endif

#endif /* USB_BASE */

#ifndef __HAL_PCD_GATE_PHYCLOCK
#define __HAL_PCD_GATE_PHYCLOCK(_DUMMY_)
#endif

#ifndef __HAL_PCD_UNGATE_PHYCLOCK
#define __HAL_PCD_UNGATE_PHYCLOCK(_DUMMY_)
#endif

#ifndef USBD_MAX_NUM_INTERFACES
#define USBD_MAX_NUM_INTERFACES 5U
#endif /* USBD_MAX_NUM_INTERFACES */

#ifndef USBD_MAX_NUM_CONFIGURATION
#define USBD_MAX_NUM_CONFIGURATION 1U
#endif /* USBD_MAX_NUM_CONFIGURATION */

#ifndef USBD_MAX_STR_DESC_SIZ
#define USBD_MAX_STR_DESC_SIZ 0x100U
#endif /* USBD_MAX_STR_DESC_SIZ */

#ifndef USBD_SELF_POWERED
#define USBD_SELF_POWERED 1U
#endif /* USBD_SELF_POWERED */

#ifndef USBD_DEBUG_LEVEL
#define USBD_DEBUG_LEVEL 0U
#endif /* USBD_DEBUG_LEVEL */

/* ECM, RNDIS, DFU Class Config */
#ifndef USBD_SUPPORT_USER_STRING_DESC
#define USBD_SUPPORT_USER_STRING_DESC 0U
#endif /* USBD_SUPPORT_USER_STRING_DESC */

/* BillBoard Class Config */
#ifndef USBD_CLASS_USER_STRING_DESC
#define USBD_CLASS_USER_STRING_DESC 1U
#endif /* USBD_CLASS_USER_STRING_DESC */

/* Interrupt priority */
#ifndef USBD_IRQ_PRIO
#define USBD_IRQ_PRIO 1
#endif /* USBD_IRQ_PRIO */

#ifndef USBD_IRQ_SUBPRIO
#define USBD_IRQ_SUBPRIO 0
#endif /* USBD_IRQ_SUBPRIO */

/* Memory management macros make sure to use static memory allocation */
/* Currently dynamic memory allocation usage */
/** Alias for memory allocation. */
#ifndef USBD_malloc
#define USBD_malloc malloc
#endif /* USBD_malloc */

/** Alias for memory release. */
#ifndef USBD_free
#define USBD_free free
#endif /* USBD_free */

/** Alias for memory set. */
#ifndef USBD_memset
#define USBD_memset memset
#endif /* USBD_memset */

/** Alias for memory copy. */
#ifndef USBD_memcpy
#define USBD_memcpy memcpy
#endif /* USBD_memcpy */

/** Alias for delay. */
#ifndef USBD_Delay
#define USBD_Delay HAL_Delay
#endif /* USBD_Delay */

/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0U)
#define USBD_UsrLog(...) \
  do                     \
  {                      \
    printf(__VA_ARGS__); \
    printf("\n");        \
  } while (0)
#else
#define USBD_UsrLog(...) \
  do                     \
  {                      \
  } while (0)
#endif

#if (USBD_DEBUG_LEVEL > 1U)

#define USBD_ErrLog(...) \
  do                     \
  {                      \
    printf("ERROR: ");   \
    printf(__VA_ARGS__); \
    printf("\n");        \
  } while (0)
#else
#define USBD_ErrLog(...) \
  do                     \
  {                      \
  } while (0)
#endif

#if (USBD_DEBUG_LEVEL > 2U)
#define USBD_DbgLog(...) \
  do                     \
  {                      \
    printf("DEBUG : ");  \
    printf(__VA_ARGS__); \
    printf("\n");        \
  } while (0)
#else
#define USBD_DbgLog(...) \
  do                     \
  {                      \
  } while (0)
#endif

  /* Exported functions -------------------------------------------------------*/
  void *USBD_static_malloc(uint32_t size);
  void USBD_static_free(void *p);

#endif /* USBCON */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
