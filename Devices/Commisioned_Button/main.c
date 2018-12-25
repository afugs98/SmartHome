#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <wifi_config.h>

#include "button.h"
#include "wifi.h"

#ifndef BUTTON_PIN
#error BUTTON_PIN is not specified
#endif

void button_identify(homekit_value_t _value)
{
   printf("Button identify\n");
}

homekit_characteristic_t button_event = HOMEKIT_CHARACTERISTIC_(PROGRAMMABLE_SWITCH_EVENT, 0);

void button_callback(uint8_t gpio, button_event_t event)
{
   switch(event)
   {
      case button_event_single_press:
         printf("single press\n");
         homekit_characteristic_notify(&button_event, HOMEKIT_UINT8(0));
         break;
      case button_event_double_press:
         printf("double press\n");
         homekit_characteristic_notify(&button_event, HOMEKIT_UINT8(1));
         break;
      case button_event_long_press:
         printf("long press\n");
         homekit_characteristic_notify(&button_event, HOMEKIT_UINT8(2));
         break;
      default:
         printf("unknown button event: %d\n", event);
   }
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "Button");

homekit_accessory_t *accessories[] =
   {
      HOMEKIT_ACCESSORY(
         .id = 1,
         .category = homekit_accessory_category_programmable_switch,
         .services=(homekit_service_t*[]) {
         HOMEKIT_SERVICE(
         ACCESSORY_INFORMATION,
         .characteristics=(homekit_characteristic_t*[]) {
         &name,
         HOMEKIT_CHARACTERISTIC(MANUFACTURER, "TrapHouse"),
         HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "032A3BCBF19D"),
         HOMEKIT_CHARACTERISTIC(MODEL, "ButtonGen1"),
         HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "4.20"),
         HOMEKIT_CHARACTERISTIC(IDENTIFY, button_identify),
         NULL
         },
         ),
         HOMEKIT_SERVICE(
         STATELESS_PROGRAMMABLE_SWITCH,
         .primary=true,
         .characteristics=(homekit_characteristic_t*[]) {
         HOMEKIT_CHARACTERISTIC(NAME, "Button"),
         &button_event,
         NULL
         },
         ),
         NULL
         },
         ),
      NULL
   };

homekit_server_config_t config =
   {
      .accessories = accessories,
      .password = "111-11-111"
   };

void on_wifi_ready()
{
   homekit_server_init(&config);
}

void create_accessory_name()
{
   uint8_t macaddr[6];
   sdk_wifi_get_macaddr(STATION_IF, macaddr);

   int name_len = snprintf(NULL, 0, "Button-%02X%02X%02X",
      macaddr[3], macaddr[4], macaddr[5]);
   char *name_value = malloc(name_len + 1);
   snprintf(name_value, name_len + 1, "Button-%02X%02X%02X",
      macaddr[3], macaddr[4], macaddr[5]);

   name.value = HOMEKIT_STRING(name_value);
}

void user_init(void)
{
   uart_set_baud(0, 115200);

   create_accessory_name();

   wifi_config_init("TrapHouse_Button", NULL, on_wifi_ready);

   if(button_create(BUTTON_PIN, button_callback))
   {
      printf("Failed to initialize button\n");
   }
}
