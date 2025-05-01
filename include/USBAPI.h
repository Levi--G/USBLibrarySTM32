/*
  USBAPI.h
  Copyright (c) 2005-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __USBAPI__
#define __USBAPI__

#include <inttypes.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#include "Arduino.h"
#include "USBEP.h"

#define EPX_SIZE USB_EP_SIZE

#if defined(USBCON)

#include "USBCore.h"

//================================================================================
//================================================================================
//	USB

#define EP_TYPE_CONTROL				(USB_ENDPOINT_TYPE_CONTROL)
#define EP_TYPE_BULK_IN				(USB_ENDPOINT_IN(USB_ENDPOINT_TYPE_BULK))
#define EP_TYPE_BULK_OUT			(USB_ENDPOINT_TYPE_BULK)
#define EP_TYPE_INTERRUPT_IN		(USB_ENDPOINT_IN(USB_ENDPOINT_TYPE_INTERRUPT))
#define EP_TYPE_INTERRUPT_OUT		(USB_ENDPOINT_TYPE_INTERRUPT)
#define EP_TYPE_ISOCHRONOUS_IN		(USB_ENDPOINT_IN(USB_ENDPOINT_TYPE_ISOCHRONOUS))
#define EP_TYPE_ISOCHRONOUS_OUT		(USB_ENDPOINT_TYPE_ISOCHRONOUS)

//================================================================================
//================================================================================
//  Low level API

typedef struct
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t wValueL;
	uint8_t wValueH;
	uint16_t wIndex;
	uint16_t wLength;
} USBSetup;

//================================================================================
//================================================================================

#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

int USB_SendControl(uint8_t flags, const void* d, int len);
int USB_RecvControl(void* d, int len);
uint8_t	USB_Available(uint8_t ep);
uint8_t USB_SendSpace(uint8_t ep);
int USB_SendZLP(uint8_t ep);	// blocking
int USB_Send(uint8_t ep, const void* data, int len);	// blocking
int USB_SendQuick(uint8_t ep, const void* data, int len);	// non-blocking, make sure to use a proper buffer!
bool USB_SendAvailable(uint8_t ep);
int USB_Recv(uint8_t ep, void* data, int len);		// non-blocking
int USB_Recv(uint8_t ep);	// non-blocking
void USB_Flush(uint8_t ep);
bool USB_Begin();
bool USB_Running();
void USB_End();

// for pluggableusb support
int PLUG_GetInterface(uint8_t* interfaceCount);
int PLUG_GetDescriptor(USBSetup& setup);
bool PLUG_Setup(USBSetup& setup);
uint8_t PLUG_GetNumEndpoints();
uint8_t PLUG_GetNumInterfaces();
uint8_t PLUG_GetEndpointTypes(uint8_t *types);

#endif

#endif /* if defined(USBCON) */
