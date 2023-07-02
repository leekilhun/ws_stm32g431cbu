#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V230701R1"
#define _DEF_BOARD_NAME           "STM32G4-FDCAN-FW"



#define _USE_HW_SPI_FLASH


#define _USE_HW_LED
#define      HW_LED_MAX_CH          5
#define      HW_LED_CH_DEBUG        _DEF_LED1
#define      HW_LED_CH_RX           _DEF_LED2
#define      HW_LED_CH_TX           _DEF_LED3
#define      HW_LED_CH_CAN          _DEF_LED4
#define      HW_LED_CH_RS485        _DEF_LED5


#define _USE_HW_UART
#define      HW_UART_MAX_CH         2
#define      HW_UART_CH_DEBUG       _DEF_UART1
#define      HW_UART_CH_RS485       _DEF_UART2


#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1
#define      HW_LOG_BOOT_BUF_MAX    1024
#define      HW_LOG_LIST_BUF_MAX    1024

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       3
#define      HW_BUTTON_CH_BOOT      _DEF_BUTTON1
#define      HW_BUTTON_CH_S1        _DEF_BUTTON2
#define      HW_BUTTON_CH_S2        _DEF_BUTTON3

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      4

#define _USE_HW_CAN
#define      HW_CAN_MAX_CH          1
#define      HW_CAN_MSG_RX_BUF_MAX  32

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         1

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          1


#endif