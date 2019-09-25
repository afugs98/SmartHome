/*
 * LedUtilities.c
 *
 *  Created on: Sep 22, 2019
 *      Author: afuglem
 */

#ifndef DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_C_
#define DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_C_

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "LedUtilities.h"
#include <FreeRTOS.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <task.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <wifi_config.h>
#include "ws2812_i2s/ws2812_i2s.h"

void DoAThingTask()
{
   while(1)
   {
      printf("Hey a thing!\r\n");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
   }
}

#define LED_ON 0                // this is the value to write to GPIO for led on (0 = GPIO low)
#define LED_INBUILT_GPIO 2      // this is the onboard LED used to show on/off only
#define LED_COUNT 127            // this is the number of WS2812B leds on the strip
#define LED_RGB_SCALE 255       // this is the scaling factor used for color conversion

ws2812_pixel_t pixels[LED_COUNT];
// Global variables
float led_hue = 0;              // hue is scaled 0 to 360
float led_saturation = 59;      // saturation is scaled 0 to 100
float led_brightness = 100;     // brightness is scaled 0 to 100
bool led_on = false;            // on is boolean on or off

void SetFloatHue(float hue)
{
   led_hue = hue;
}
void SetFloatSaturation(float saturation)
{
   led_saturation = saturation;
}
void SetFloatBrightness(float brightness)
{
   led_brightness = brightness;
}
void SetBoolLedStatus(bool status)
{
   led_on = status;
}

float GetFloatHue()
{
   return led_hue;
}
float GetFloatSaturation()
{
   return led_saturation;
}
float GetFloatBrightness()
{
   return led_brightness;
}
bool GetBoolLedStatus()
{
   return led_on;
}

//http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
static void hsi2rgb(float h, float s, float i, ws2812_pixel_t* rgb)
{
   int r, g, b;

   while(h < 0)
   {
      h += 360.0F;
   };     // cycle h around to 0-360 degrees
   while(h >= 360)
   {
      h -= 360.0F;
   };
   h = 3.14159F * h / 180.0F;            // convert to radians.
   s /= 100.0F;                        // from percentage to ratio
   i /= 100.0F;                        // from percentage to ratio
   s = s > 0 ? (s < 1 ? s : 1) : 0;    // clamp s and i to interval [0,1]
   i = i > 0 ? (i < 1 ? i : 1) : 0;    // clamp s and i to interval [0,1]
   i = i * sqrt(i);                    // shape intensity to have finer granularity near 0

   if(h < 2.09439)
   {
      r = LED_RGB_SCALE * i / 3 * (1 + s * cos(h) / cos(1.047196667 - h));
      g = LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos(h) / cos(1.047196667 - h)));
      b = LED_RGB_SCALE * i / 3 * (1 - s);
   }
   else if(h < 4.188787)
   {
      h = h - 2.09439;
      g = LED_RGB_SCALE * i / 3 * (1 + s * cos(h) / cos(1.047196667 - h));
      b = LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos(h) / cos(1.047196667 - h)));
      r = LED_RGB_SCALE * i / 3 * (1 - s);
   }
   else
   {
      h = h - 4.188787;
      b = LED_RGB_SCALE * i / 3 * (1 + s * cos(h) / cos(1.047196667 - h));
      r = LED_RGB_SCALE * i / 3 * (1 + s * (1 - cos(h) / cos(1.047196667 - h)));
      g = LED_RGB_SCALE * i / 3 * (1 - s);
   }

   rgb->red = (uint8_t)r;
   rgb->green = (uint8_t)g;
   rgb->blue = (uint8_t)b;
   rgb->white = (uint8_t)0;           // white channel is not used
}

void led_string_fill(ws2812_pixel_t rgb)
{

   // write out the new color to each pixel
   for(int i = 0; i < LED_COUNT; i++)
   {
      pixels[i] = rgb;
   }
   ws2812_i2s_update(pixels, PIXEL_RGB);
}

void led_string_set(void)
{
   ws2812_pixel_t rgb =
      {
         { 0, 0, 0, 0 } };

   if(led_on)
   {
      // convert HSI to RGBW
      hsi2rgb(led_hue, led_saturation, led_brightness, &rgb);
      //printf("h=%d,s=%d,b=%d => ", (int)led_hue, (int)led_saturation, (int)led_brightness);
      //printf("r=%d,g=%d,b=%d,w=%d\n", rgbw.red, rgbw.green, rgbw.blue, rgbw.white);

      // set the inbuilt led
      gpio_write(LED_INBUILT_GPIO, LED_ON);
   }
   else
   {
      // printf("off\n");
      gpio_write(LED_INBUILT_GPIO, 1 - LED_ON);
   }

   // write out the new color
   led_string_fill(rgb);
}

void led_init()
{
   // initialise the onboard led as a secondary indicator (handy for testing)
   gpio_enable(LED_INBUILT_GPIO, GPIO_OUTPUT);

   // initialise the LED strip
   ws2812_i2s_init(LED_COUNT, PIXEL_RGB);

   // set the initial state
   led_string_set();
}

void led_identify_task(void *_args)
{
   const ws2812_pixel_t COLOR_PINK =
      {
         { 255, 0, 127, 0 } };
   const ws2812_pixel_t COLOR_BLACK =
      {
         { 0, 0, 0, 0 } };

   for(int i = 0; i < 3; i++)
   {
      for(int j = 0; j < 3; j++)
      {
         gpio_write(LED_INBUILT_GPIO, LED_ON);
         led_string_fill(COLOR_PINK);
         vTaskDelay(100 / portTICK_PERIOD_MS);
         gpio_write(LED_INBUILT_GPIO, 1 - LED_ON);
         led_string_fill(COLOR_BLACK);
         vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      vTaskDelay(250 / portTICK_PERIOD_MS);
   }

   led_string_set();
   vTaskDelete(NULL);
}

void led_identify(homekit_value_t _value)
{
   // printf("LED identify\n");
   xTaskCreate(led_identify_task, "LED identify", 128, NULL, 2, NULL);
}

#endif /* DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_C_ */
