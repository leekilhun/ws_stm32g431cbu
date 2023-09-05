/*
 * flash.h
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */

#ifndef COMMON_INC_HW_FLASH_H_
#define COMMON_INC_HW_FLASH_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_FLASH


bool flashInit(void);
bool flashErase(uint32_t addr, uint32_t length);
bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length);


#endif


#ifdef __cplusplus
}
#endif


#endif /* COMMON_INC_HW_FLASH_H_ */
