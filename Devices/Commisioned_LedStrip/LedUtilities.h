/*
 * LedUtilities.h
 *
 *  Created on: Sep 22, 2019
 *      Author: afuglem
 */

#ifndef DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_H_
#define DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_H_

void DoAThingTask();


void SetFloatHue(float);
void SetFloatSaturation(float);
void SetFloatBrightness(float);
void SetBoolLedStatus(bool);

float GetFloatHue();
float GetFloatSaturation();
float GetFloatBrightness();
bool GetBoolLedStatus();

void led_string_set();
void led_identify();
void led_init();



#endif /* DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_H_ */
