/*
 * uart.h
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */

#ifndef COMMON_INC_HW_UART_H_
#define COMMON_INC_HW_UART_H_


#include "hw_def.h"

#ifdef __cplusplus
extern "C" {
#endif


bool     uartInit(void);
bool     uartDeInit(void);
bool     uartIsInit(void);
bool     uartOpen(uint8_t ch, uint32_t baud);
bool     uartIsOpen(uint8_t ch);
bool     uartClose(uint8_t ch);
uint32_t uartAvailable(uint8_t ch);
bool     uartFlush(uint8_t ch);
uint8_t  uartRead(uint8_t ch);
uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length);
uint32_t uartPrintf(uint8_t ch, const char *fmt, ...);
uint32_t uartVPrintf(uint8_t ch, const char *fmt, va_list arg);
uint32_t uartGetBaud(uint8_t ch);
uint32_t uartGetRxCnt(uint8_t ch);
uint32_t uartGetTxCnt(uint8_t ch);


#ifdef __cplusplus
}
#endif



#endif /* COMMON_INC_HW_UART_H_ */
