/*
 * HomekitUtilities.c
 *
 *  Created on: Sep 22, 2019
 *      Author: afuglem
 */

#include <FreeRTOS.h>
#include <esp8266.h>
#include <task.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "HomekitUtilities.h"
#include "LedUtilities.h"

homekit_value_t led_on_get()
{
//   return HOMEKIT_BOOL(led_on);
   return HOMEKIT_BOOL(GetBoolLedStatus());
}

void led_on_set(homekit_value_t value)
{
   if(value.format != homekit_format_bool)
   {
      // printf("Invalid on-value format: %d\n", value.format);
      return;
   }

//   led_on = value.bool_value;
   SetBoolLedStatus(value.bool_value);
   led_string_set();
}

homekit_value_t led_brightness_get()
{
//   return HOMEKIT_INT(led_brightness);
   return HOMEKIT_INT(GetFloatBrightness());
}
void led_brightness_set(homekit_value_t value)
{
   if(value.format != homekit_format_int)
   {
      // printf("Invalid brightness-value format: %d\n", value.format);
      return;
   }
//   led_brightness = value.int_value;
   SetFloatBrightness(value.int_value);
   led_string_set();
}

homekit_value_t led_hue_get()
{
//   return HOMEKIT_FLOAT(led_hue);
   return HOMEKIT_FLOAT(GetFloatHue());
}

void led_hue_set(homekit_value_t value)
{
   if(value.format != homekit_format_float)
   {
      // printf("Invalid hue-value format: %d\n", value.format);
      return;
   }
//   led_hue = value.float_value;
   SetFloatHue(value.float_value);
   led_string_set();
}

homekit_value_t led_saturation_get()
{
//   return HOMEKIT_FLOAT(led_saturation);
   return HOMEKIT_FLOAT(GetFloatSaturation());
}

void led_saturation_set(homekit_value_t value)
{
   if(value.format != homekit_format_float)
   {
      // printf("Invalid sat-value format: %d\n", value.format);
      return;
   }
//   led_saturation = value.float_value;
   SetFloatSaturation(value.float_value);
   led_string_set();
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "TrapHouse LED Strip");

homekit_accessory_t *accessories[] =
   {
      HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_lightbulb, .services = (homekit_service_t*[]) {
         HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
         &name,
         HOMEKIT_CHARACTERISTIC(MANUFACTURER, "TrapHosue"),
         HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "037A2BABF19D"),
         HOMEKIT_CHARACTERISTIC(MODEL, "LEDStrip"),
         HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "4.20"),
         HOMEKIT_CHARACTERISTIC(IDENTIFY, led_identify),
         NULL
         }),
         HOMEKIT_SERVICE(LIGHTBULB, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
         HOMEKIT_CHARACTERISTIC(NAME, "RGB Led Strip"),
         HOMEKIT_CHARACTERISTIC(
         ON, true,
         .getter = led_on_get,
         .setter = led_on_set
         ),
         HOMEKIT_CHARACTERISTIC(
         BRIGHTNESS, 100,
         .getter = led_brightness_get,
         .setter = led_brightness_set
         ),
         HOMEKIT_CHARACTERISTIC(
         HUE, 0,
         .getter = led_hue_get,
         .setter = led_hue_set
         ),
         HOMEKIT_CHARACTERISTIC(
         SATURATION, 0,
         .getter = led_saturation_get,
         .setter = led_saturation_set
         ),
         NULL
         }),
         NULL
         }),
      NULL
   };

homekit_server_config_t config =
   {
      .accessories = accessories,
      .password = DEVICE_PASSCODE,
      .setupId = DEVICE_SETUP_ID,
   };

void StartHomeKitServer()
{
   homekit_server_init(&config);
}

void SetHomeKitName(char * nameString)
{
   name.value = HOMEKIT_STRING(nameString);
}
