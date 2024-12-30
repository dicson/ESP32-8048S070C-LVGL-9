#ifndef _ARDUINO_GFX_LIBRARIES_H_
#define _ARDUINO_GFX_LIBRARIES_H_

#include "Arduino_DataBus.h"

#include "databus/Arduino_ESP32RGBPanel.h"

#include "Arduino_GFX.h" // Core graphics library

#include "display/Arduino_RPi_DPI_RGBPanel.h"

#define DISPLAY_DEV_KIT
#define ESP32_S3_BOX
#define DF_GFX_SCK TFT_CLK
#define DF_GFX_MOSI TFT_MOSI
#define DF_GFX_MISO TFT_MISO
#define DF_GFX_CS TFT_CS
#define DF_GFX_DC TFT_DC
#define DF_GFX_RST TFT_RST
#define DF_GFX_BL TFT_BL


Arduino_DataBus *create_default_Arduino_DataBus();
Arduino_GFX *create_default_Arduino_GFX();

#endif // _ARDUINO_GFX_LIBRARIES_H_
