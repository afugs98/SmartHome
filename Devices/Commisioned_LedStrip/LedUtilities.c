/*
 * LedUtilities.c
 *
 *  Created on: Sep 22, 2019
 *      Author: afuglem
 */

#ifndef DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_C_
#define DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_C_

#include <stdio.h>
#include "LedUtilities.h"
#include <FreeRTOS.h>

void DoAThingTask()
{
   while(1)
   {
      printf("Hey a thing!\r\n");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
   }

}

#endif /* DEVICES_COMMISIONED_LEDSTRIP_LEDUTILITIES_C_ */
