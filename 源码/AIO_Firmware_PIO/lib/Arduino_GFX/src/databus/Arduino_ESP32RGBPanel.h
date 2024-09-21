#include "Arduino_DataBus.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)

#ifndef _ARDUINO_ESP32RGBPANEL_H_
#define _ARDUINO_ESP32RGBPANEL_H_

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_interface.h"
#include "esp_private/gdma.h"
#include "esp_pm.h"
#include "hal/dma_types.h"

#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"

#include "esp32s3/rom/cache.h"
// This function is located in ROM (also see esp_rom/${target}/ld/${target}.rom.ld)
extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

// extract from esp-idf esp_lcd_rgb_panel.c
struct esp_rgb_panel_t
{
  esp_lcd_panel_t base;                                        // Base class of generic lcd panel
  int panel_id;                                                // LCD panel ID
  lcd_hal_context_t hal;                                       // Hal layer object
  size_t data_width;                                           // Number of data lines (e.g. for RGB565, the data width is 16)
  size_t sram_trans_align;                                     // Alignment for framebuffer that allocated in SRAM
  size_t psram_trans_align;                                    // Alignment for framebuffer that allocated in PSRAM
  int disp_gpio_num;                                           // Display control GPIO, which is used to perform action like "disp_off"
  intr_handle_t intr;                                          // LCD peripheral interrupt handle
  esp_pm_lock_handle_t pm_lock;                                // Power management lock
  size_t num_dma_nodes;                                        // Number of DMA descriptors that used to carry the frame buffer
  uint8_t *fb;                                                 // Frame buffer
  size_t fb_size;                                              // Size of frame buffer
  int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH];                  // GPIOs used for data lines, we keep these GPIOs for action like "invert_color"
  size_t resolution_hz;                                        // Peripheral clock resolution
  esp_lcd_rgb_timing_t timings;                                // RGB timing parameters (e.g. pclk, sync pulse, porch width)
  gdma_channel_handle_t dma_chan;                              // DMA channel handle
  esp_lcd_rgb_panel_frame_trans_done_cb_t on_frame_trans_done; // Callback, invoked after frame trans done
  void *user_ctx;                                              // Reserved user's data of callback functions
  int x_gap;                                                   // Extra gap in x coordinate, it's used when calculate the flush window
  int y_gap;                                                   // Extra gap in y coordinate, it's used when calculate the flush window
  struct
  {
    unsigned int disp_en_level : 1; // The level which can turn on the screen by `disp_gpio_num`
    unsigned int stream_mode : 1;   // If set, the LCD transfers data continuously, otherwise, it stops refreshing the LCD when transaction done
    unsigned int fb_in_psram : 1;   // Whether the frame buffer is in PSRAM
  } flags;
  dma_descriptor_t dma_nodes[]; // DMA descriptor pool of size `num_dma_nodes`
};

class Arduino_ESP32RGBPanel
{
public:
  Arduino_ESP32RGBPanel(
      int8_t cs, int8_t sck, int8_t sda,
      int8_t de, int8_t vsync, int8_t hsync, int8_t pclk,
      int8_t r0, int8_t r1, int8_t r2, int8_t r3, int8_t r4,
      int8_t g0, int8_t g1, int8_t g2, int8_t g3, int8_t g4, int8_t g5,
      int8_t b0, int8_t b1, int8_t b2, int8_t b3, int8_t b4);

  virtual void begin(int32_t speed = 10000000L);

  void sendCommand(uint8_t c);
  void sendData(uint8_t d);
  uint16_t *getFrameBuffer(int16_t w, int16_t h);

protected:
private:
  INLINE void CS_HIGH(void);
  INLINE void CS_LOW(void);
  INLINE void SCK_HIGH(void);
  INLINE void SCK_LOW(void);
  INLINE void SDA_HIGH(void);
  INLINE void SDA_LOW(void);

  int32_t _speed;
  int8_t _dataMode;
  int8_t _cs, _sck, _sda;
  int8_t _de, _vsync, _hsync, _pclk;
  int8_t _r0, _r1, _r2, _r3, _r4;
  int8_t _g0, _g1, _g2, _g3, _g4, _g5;
  int8_t _b0, _b1, _b2, _b3, _b4;

  esp_lcd_panel_handle_t _panel_handle = NULL;
  esp_rgb_panel_t *_rgb_panel;

  PORTreg_t _csPortSet;  ///< PORT register for chip select SET
  PORTreg_t _csPortClr;  ///< PORT register for chip select CLEAR
  PORTreg_t _sckPortSet; ///< PORT register for SCK SET
  PORTreg_t _sckPortClr; ///< PORT register for SCK CLEAR
  PORTreg_t _sdaPortSet; ///< PORT register for SCK SET
  PORTreg_t _sdaPortClr; ///< PORT register for SCK CLEAR
  uint32_t _csPinMask;   ///< Bitmask for chip select
  uint32_t _sckPinMask;  ///< Bitmask for SCK
  uint32_t _sdaPinMask;  ///< Bitmask for SCK
};

#endif // _ARDUINO_ESP32RGBPANEL_H_

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)
