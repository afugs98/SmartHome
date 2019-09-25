/*
 * Wifi Commissioned ws2812b strip. Now with QR codes!
 *
 * Contributed March 2018 by https://github.com/Dave1001
 * Modified June 2019 by https://github.com/afugs98
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

#include "LedUtilities.h"
#include "HomekitUtilities.h"

void SetTextToBlue()
{
   printf("\033[1;36m");
}

void ResetTextColor()
{
   printf("\033[0m");
}

void user_init(void)
{
   uart_set_baud(0, 115200);

   xTaskCreate(DoAThingTask, "do_a_thing_task", 1024, NULL, 2, NULL);

   printf("Create QR code with password: ");
   SetTextToBlue();
   printf("%s", DEVICE_PASSCODE);
   ResetTextColor();
   printf(" and setup ID: ");
   SetTextToBlue();
   printf("%s\n", DEVICE_SETUP_ID);
   ResetTextColor();

   // This example shows how to use same firmware for multiple similar accessories
   // without name conflicts. It uses the last 3 bytes of accessory's MAC address as
   // accessory name suffix.
   uint8_t macaddr[6];
   sdk_wifi_get_macaddr(STATION_IF, macaddr);
   int name_len = snprintf(NULL, 0, "LED Strip-%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
   char *name_value = malloc(name_len + 1);
   snprintf(name_value, name_len + 1, "LED Strip-%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
//   name.value = HOMEKIT_STRING(name_value);

   led_init();
   wifi_config_init("TrapHouse", NULL, StartHomeKitServer);
}
