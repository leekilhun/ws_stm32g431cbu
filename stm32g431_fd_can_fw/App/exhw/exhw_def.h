/*
 * exhw_def.h
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */

#ifndef EXHW_EXHW_DEF_H_
#define EXHW_EXHW_DEF_H_



#include "hw.h"

//#define _USE_EXHW_LED
#define      EXHW_LED_MAX_CH          5
#define      EXHW_LED_CH_DEBUG        _DEF_LED1
#define      EXHW_LED_CH_RX           _DEF_LED2
#define      EXHW_LED_CH_TX           _DEF_LED3
#define      EXHW_LED_CH_CAN          _DEF_LED4
#define      EXHW_LED_CH_RS485        _DEF_LED5


//#define _USE_EXHW_BUTTON
#define      EXHW_BUTTON_MAX_CH       3
#define      EXHW_BUTTON_CH_BOOT      _DEF_BUTTON1
#define      EXHW_BUTTON_CH_S1        _DEF_BUTTON2
#define      EXHW_BUTTON_CH_S2        _DEF_BUTTON3
#endif /* EXHW_EXHW_DEF_H_ */


//#define _USE_EXHW_CMD
#define      EXHW_CMD_MAX_DATA_LENGTH 1024
