/*
 * Example of using esp-wifi-config library to join
 * accessories to WiFi networks.
 *
 * When accessory starts without WiFi config or
 * configure WiFi network is not available, it creates
 * it's own WiFi AP "my-accessory-XXXXXX" (where XXXXXX
 * is last 3 digits of accessory's mac address in HEX).
 * If you join that network, you will be presented with
 * a page to choose WiFi network to connect accessory to.
 *
 * After successful connection accessory shuts down AP.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <wifi_config.h>

const int led_gpio = 2;
void led_on_callback(homekit_characteristic_t *_ch, homekit_value_t on, void *context);

void led_write(bool on)
{
   gpio_write(led_gpio, on ? 1 : 0);
}

homekit_characteristic_t led_on = HOMEKIT_CHARACTERISTIC_(
   ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(led_on_callback)
);

bool outletStatus = false;

void led_init()
{
   gpio_enable(led_gpio, GPIO_OUTPUT);
   led_write(led_on.value.bool_value);
}

void led_on_callback(homekit_characteristic_t *_ch, homekit_value_t on, void *context)
{
   led_write(led_on.value.bool_value);
}

void led_identify_task(void *_args)
{
   for(int i = 0; i < 3; i++)
   {
      for(int j = 0; j < 2; j++)
      {
         led_write(true);
         vTaskDelay(100 / portTICK_PERIOD_MS);
         led_write(false);
         vTaskDelay(100 / portTICK_PERIOD_MS);
      }

      vTaskDelay(250 / portTICK_PERIOD_MS);
   }

   led_write(led_on.value.bool_value);

   vTaskDelete(NULL);
}

void led_identify(homekit_value_t _value)
{
   printf("LED identify\n");
   xTaskCreate(led_identify_task, "LED identify", 128, NULL, 2, NULL);
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "Outlet safety");

homekit_characteristic_t switch_outlet_in_use = HOMEKIT_CHARACTERISTIC_(OUTLET_IN_USE, true);

homekit_value_t led_on_get() {
    return HOMEKIT_BOOL(outletStatus);
}

void led_on_set(homekit_value_t value) {
    if (value.format != homekit_format_bool) {
        printf("Invalid value format: %d\n", value.format);
        return;
    }

    outletStatus = value.bool_value;
    led_write(outletStatus);
}

homekit_accessory_t *accessories[] =
   {
      HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_outlet, .services=(homekit_service_t*[]){
         HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
         &name,
         HOMEKIT_CHARACTERISTIC(MANUFACTURER, "TrapHouse"),
         HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "137A3BRCF11A"),
         HOMEKIT_CHARACTERISTIC(MODEL, "OutletGen1"),
         HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "4.20"),
         HOMEKIT_CHARACTERISTIC(IDENTIFY, led_identify),
         NULL
         }),
         HOMEKIT_SERVICE(OUTLET, .primary=true, .characteristics=(homekit_characteristic_t*[]){
         HOMEKIT_CHARACTERISTIC(NAME, "Outlet"),
         &switch_outlet_in_use,
         HOMEKIT_CHARACTERISTIC(
         ON, false,
         .getter=led_on_get,
         .setter=led_on_set
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
      .password = "111-11-111"
   };

void on_wifi_ready()
{
   homekit_server_init(&config);
}

void user_init(void)
{
   uart_set_baud(0, 115200);

   // accessory name suffix.
   uint8_t macaddr[6];
   sdk_wifi_get_macaddr(STATION_IF, macaddr);
   int name_len = snprintf(NULL, 0, "Outlet-%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
   char *name_value = malloc(name_len + 1);
   snprintf(name_value, name_len + 1, "Outlet-%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
   name.value = HOMEKIT_STRING(name_value);

   wifi_config_init("TrapHouse:", NULL, on_wifi_ready);
   led_init();
}
