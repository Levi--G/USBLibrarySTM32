#pragma once

#include <stm32_def.h>

//These defaults are for values that depend on a certain board

#if defined(USB)
#define PACKETBUFFER_COUNT_DEFAULT 3
#elif defined(USB_OTG_FS)
#define PACKETBUFFER_COUNT_DEFAULT 2
#endif

#if defined(USB)
#define PACKETBUFFER_ALLOW_OVERWRITE_DEFAULT true
#elif defined(USB_OTG_FS)
#define PACKETBUFFER_ALLOW_OVERWRITE_DEFAULT false
#endif