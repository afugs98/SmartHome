/*
 * Outlet.h
 *
 *  Created on: Dec 1, 2018
 *      Author: afuglem
 *
 *  This is the file that will hold the outlet's private functions making it
 *  easier to init and work with. This will expose the init function, get pointer
 *  and also the change state, just in case.
 */

#ifndef DEVICES_BASEOBJECTS_OUTLET_H_
#define DEVICES_BASEOBJECTS_OUTLET_H_

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

homekit_accessory_t * CreateOutlet();
void outlet_init();
#endif /* DEVICES_BASEOBJECTS_OUTLET_H_ */
