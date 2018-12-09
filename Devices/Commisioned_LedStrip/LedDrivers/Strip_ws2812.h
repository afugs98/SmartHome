/*
 * Strip_ws2812.h
 *
 *  Created on: Dec 3, 2018
 *      Author: afuglem
 *
 *  This is the driver for the ws2812 led strip. This is a thin wrapper to
 *  make my life easier.
 */

#ifndef DEVICES_COMMISIONED_LEDSTRIP_LEDDRIVERS_STRIP_WS2812_H_
#define DEVICES_COMMISIONED_LEDSTRIP_LEDDRIVERS_STRIP_WS2812_H_

#include "ws2812/ws2812_i2s/ws2812_i2s.h"
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void LedIdentify(homekit_value_t _value);
void LedInit();

homekit_value_t LedGetSate();
void LedSetState(homekit_value_t value);

homekit_value_t LedGetBrightness();
void LedSetBrightness(homekit_value_t value);

homekit_value_t LedHueGet();
void LedHueSet(homekit_value_t value);

homekit_value_t LedGetSaturation();
void LedSetSaturation(homekit_value_t value);

void Testicles();

#endif /* DEVICES_COMMISIONED_LEDSTRIP_LEDDRIVERS_STRIP_WS2812_H_ */
