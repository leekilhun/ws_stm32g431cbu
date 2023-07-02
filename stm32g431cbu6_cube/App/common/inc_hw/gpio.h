/*
 * gpio.h
 *
 *  Created on: 2023. 6. 11.
 *      Author: gns2l
 */

#ifndef COMMON_INC_HW_GPIO_H_
#define COMMON_INC_HW_GPIO_H_



#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_GPIO


#define GPIO_MAX_CH     HW_GPIO_MAX_CH

  bool gpioInit(void);
  void gpioPinWrite(uint8_t ch, bool value);
  bool gpioPinRead(uint8_t ch);
  void gpioPinToggle(uint8_t ch);
  bool gpioIsOn(uint8_t ch);

  void gpioAttachCallbackFunc(uint8_t ch,void (*cb_func)(void)/*bool(*cb_func)(uint8_t,uint8_t*,uint8_t)*/);

#endif

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INC_HW_GPIO_H_ */
