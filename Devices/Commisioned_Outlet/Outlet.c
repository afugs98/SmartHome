#include "Outlet.h"

const int outlet_gpio = 0; // 0 for the cheap relay modules
bool outletStatus = false;
homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "Outlet safety"); // Deals with the auto gen name
homekit_characteristic_t switch_outlet_in_use = HOMEKIT_CHARACTERISTIC_(OUTLET_IN_USE, true); // Gotta have the outlet in use

void outlet_write(bool on)
{
   gpio_write(outlet_gpio, on ? 0 : 1);
}

void outlet_init()
{
   gpio_enable(outlet_gpio, GPIO_OUTPUT);
   outlet_write(outletStatus);
}

void outlet_identify_task(void *_args)
{
   for(int i = 0; i < 3; i++)
   {
      for(int j = 0; j < 2; j++)
      {
         outlet_write(true);
         vTaskDelay(100 / portTICK_PERIOD_MS);
         outlet_write(false);
         vTaskDelay(100 / portTICK_PERIOD_MS);
      }

      vTaskDelay(250 / portTICK_PERIOD_MS);
   }

   outlet_write(outletStatus);

   vTaskDelete(NULL);
}

void outlet_identify(homekit_value_t _value)
{
   printf("LED identify\n");
   xTaskCreate(outlet_identify_task, "LED identify", 128, NULL, 2, NULL);
}

homekit_value_t outlet_on_get()
{
   return HOMEKIT_BOOL(outletStatus);
}

void outlet_on_set(homekit_value_t value)
{
   if(value.format != homekit_format_bool)
   {
      printf("Invalid value format: %d\n", value.format);
      return;
   }

   outletStatus = value.bool_value;
   printf("------------------- Outlet status: %d\n", outletStatus);
   outlet_write(outletStatus);
}

homekit_accessory_t * CreateOutlet()
{
   // accessory name suffix.
   uint8_t macaddr[6];
   sdk_wifi_get_macaddr(STATION_IF, macaddr);
   int name_len = snprintf(NULL, 0, "Outlet-%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
   char *name_value = malloc(name_len + 1);
   snprintf(name_value, name_len + 1, "Outlet-%02X%02X%02X", macaddr[3], macaddr[4], macaddr[5]);
   name.value = HOMEKIT_STRING(name_value);

   homekit_accessory_t *ptr = HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_outlet, .services=(homekit_service_t*[]){
      HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
      &name,
      HOMEKIT_CHARACTERISTIC(MANUFACTURER, "TrapHouse"),
      HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "137A3BRCF11A"),
      HOMEKIT_CHARACTERISTIC(MODEL, "OutletGen1"),
      HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "4.20"),
      HOMEKIT_CHARACTERISTIC(IDENTIFY, outlet_identify),
      NULL
      }),
      HOMEKIT_SERVICE(OUTLET, .primary=true, .characteristics=(homekit_characteristic_t*[]){
      HOMEKIT_CHARACTERISTIC(NAME, "Outlet"),
      &switch_outlet_in_use,
      HOMEKIT_CHARACTERISTIC(
      ON, false,
      .getter=outlet_on_get,
      .setter=outlet_on_set
      ),
      NULL
      }),
      NULL
      });

   outlet_init();

   return ptr;
}
;
