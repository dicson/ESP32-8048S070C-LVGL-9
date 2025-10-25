#ifndef DISPLAY_H
#define DISPLAY_H

#include <lvgl.h>                /* 9.2.2 */
#include <Arduino_GFX_Library.h> /* 1.5.0 */
#include "TAMC_GT911.h"          /* 1.0.2 */

// Configuration for Display and Touch
// #define TFT_BL 2
#define TOUCH_GT911_SCL 20
#define TOUCH_GT911_SDA 19
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST 38
#define TOUCH_MAP_X1 800
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0

// Initialize touchscreen object
TAMC_GT911 ts(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));

#define GFX_BL 2
#ifdef GFX_BL
#include <driver/ledc.h>
ledc_channel_config_t ledc_channel = {
    .gpio_num = GFX_BL,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_2,
    .timer_sel = LEDC_TIMER_0,
    .duty = 200,
    .hpoint = 0,
};
ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,   // timer mode
    .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
    .timer_num = LEDC_TIMER_0,           // timer index
    .freq_hz = 1000,                     // frequency of PWM signal
    .clk_cfg = LEDC_USE_RTC8M_CLK,       // Force source clock to RTC8M
};
#endif

Arduino_ESP32RGBPanel rgbpanel(
    41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
    9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
    15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */,
    0 /* hsync_polarity */, 20 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
    0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
    true /* pclk_active_neg */);

Arduino_RGB_Display gfx(800, 480, &rgbpanel, 0 /* rotation */, true);

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf;

uint32_t millis_cb(void)
{
  return millis();
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
#ifndef DIRECT_MODE
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  gfx.draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
#endif
  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  ts.read();        // Read touch data
  if (ts.isTouched) // Check if the screen is touched
  {
    for (int i = 0; i < ts.touches; i++) // Loop through all detected touch points
    {
      // We can use the first touch (or other logic for multiple touches)
      if (i == 0)
      {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = ts.points[i].x; // Get x coordinate
        data->point.y = ts.points[i].y; // Get y coordinate
      }
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup_display()
{
  Serial.begin(115200);
  delay(500); // Short delay to ensure serial is ready
  Serial.println("Initializing display...");

  gfx.begin();

  gfx.fillScreen(0x000000);

#ifdef GFX_BL
  ledc_timer_config(&ledc_timer);
  ledc_channel_config(&ledc_channel);
  ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, ledc_channel.duty);
  ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
#endif

  ts.begin(); // Initialize the touchscreen
  ts.setRotation(1);

  lv_init();
  lv_tick_set_cb(millis_cb);

  screenWidth = gfx.width();
  screenHeight = gfx.height();
#ifdef DIRECT_MODE
  bufSize = screenWidth * screenHeight;
#else
  bufSize = screenWidth * 40;
#endif

  disp_draw_buf = (lv_color_t *)malloc(bufSize * 2);
  if (!disp_draw_buf)
  {
    Serial.println("Failed to allocate display buffer!");
    return;
  }

  disp = lv_display_create(screenWidth, screenHeight);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  Serial.println("Display setup complete.");
}

void loop_display()
{
  lv_task_handler();
  delay(5);
}

#endif // DISPLAY_H
