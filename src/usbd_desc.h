/*
 * usbd_desc.h
 * Template generated with Stm32CubeMX "usbd_desc.h":
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

#ifndef __USBD_DESC_H
#define __USBD_DESC_H
#ifdef USBCON
/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DEVICE_ID1 (UID_BASE)
#define DEVICE_ID2 (UID_BASE + 0x4U)
#define DEVICE_ID3 (UID_BASE + 0x8U)

/*
 * USB Billboard Class USER string desc Defines Template
 * index should start form 0x10 to avoid using the reserved device string desc indexes
 */
#if (USBD_CLASS_USER_STRING_DESC == 1)
#define USBD_BB_IF_STRING_INDEX       0x10U
#define USBD_BB_URL_STRING_INDEX      0x11U
#define USBD_BB_ALTMODE0_STRING_INDEX 0x12U
#define USBD_BB_ALTMODE1_STRING_INDEX 0x13U
/* Add Specific USER string Desc */
#define USBD_BB_IF_STR_DESC       (uint8_t *)"STM32 BillBoard Interface"
#define USBD_BB_URL_STR_DESC      (uint8_t *)"www.st.com"
#define USBD_BB_ALTMODE0_STR_DESC (uint8_t *)"STM32 Alternate0 Mode"
#define USBD_BB_ALTMODE1_STR_DESC (uint8_t *)"STM32 Alternate1 Mode"
#endif

#define USB_SIZ_STRING_SERIAL 0x1AU

#if (USBD_LPM_ENABLED == 1)
#define USB_SIZ_BOS_DESC 0x0CU
#elif (USBD_CLASS_BOS_ENABLED == 1)
#define USB_SIZ_BOS_DESC 0x5DU
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern USBD_DescriptorsTypeDef USBD_Desc;

#endif /* USBCON */
#endif /* __USBD_DESC_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
