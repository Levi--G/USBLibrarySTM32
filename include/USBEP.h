#pragma once

#define MAX_USB_EP_SIZE 64
#ifndef USB_EP0_SIZE
#define USB_EP0_SIZE MAX_USB_EP_SIZE
#endif
// This definitions is usefull if you want to reduce the EP_SIZE to save ram
// Don't forget to also make the epsize smaller in the interface definition
#ifndef USB_EP_SIZE
#define USB_EP_SIZE 64
#endif

#if defined(USB_OTG_FS)
#define USB_MAX_EPS 4
#else
#define USB_MAX_EPS 8
#endif
#define USB_MAX_EPS_SLOTS (USB_MAX_EPS * 2)