/**
 ******************************************************************************
 * @file    usbd_if.h
 * @author  fpistm
 * @brief   Header file for the usbd_if.c file
 ******************************************************************************
 */

#ifndef __USBD_IF_H
#define __USBD_IF_H

#ifdef USBCON
/* Includes ------------------------------------------------------------------*/
#include "Arduino.h"

/* Re-enumeration handling*/
#ifndef USBD_ENUM_DELAY
#define USBD_ENUM_DELAY 10
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  void USBD_reenumerate(void);

  /* Weaked function */
  void USBD_SystemClockConfigFromResume(void);

#ifdef __cplusplus
}
#endif
#endif /* USBCON */
#endif /* __USBD_IF_H */
