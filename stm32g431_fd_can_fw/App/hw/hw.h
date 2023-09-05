/*
 * hw.h
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */

#ifndef HW_HW_H_
#define HW_HW_H_



#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#include "swtimer.h"

#include "can.h"
#include "uart.h"
#include "usb.h"
#include "cdc.h"
#include "cli.h"
#include "log.h"
#include "spi.h"
#include "spi_flash.h"

#include "flash.h"
#include "fs.h"
#include "nvs.h"
#include "eeprom.h"

#include "rtc.h"
#include "reset.h"

#include "fault.h"

#include "utils.h"

  /*
#include "led.h"
#include "button.h"
#include "swtimer.h"
#include "can.h"
#include "gpio.h"
#include "spi.h"
#include "spi_flash.h"

#include "flash.h"
#include "fs.h"
#include "nvs.h"
#include "eeprom.h"
#include "rtc.h"
#include "reset.h"
#include "cmd.h"
#include "util.h"
#include "fault.h"

*/
bool hwInit(void);


#ifdef __cplusplus
}
#endif


#endif /* HW_HW_H_ */
