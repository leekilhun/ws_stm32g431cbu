/*
 * ap_def.h
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */

#ifndef AP_AP_DEF_H_
#define AP_AP_DEF_H_


#include "exhw.h"




#include <array>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <string>

#ifndef errno_t
#define errno_t int
//typedef int errno_t;
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0
#endif

#ifndef ERROR_FAIL
#define ERROR_FAIL -1
#endif

#define GPIO_IN_STATE_ON    GPIO_PIN_RESET
#define GPIO_IN_STATE_OFF   GPIO_PIN_SET
#define GPIO_OUT_STATE_ON   GPIO_PIN_RESET
#define GPIO_OUT_STATE_OFF  GPIO_PIN_SET

//#define  APP_USE_MOTOR_HIGH
//#define APP_USE_MOTOR_CNT_TWO
#include "def_obj.hpp"

typedef void (*evt_cb)(void* obj, void* w_parm, void* l_parm);


//event driven architecture
namespace eda
{
  typedef struct thread_t_      thread_t;
  typedef struct cmd_process_t_ cmd_process_t;


  typedef enum
  {
    EVENT_MODE_CHANGE,
    EVENT_USB_OPEN,
    EVENT_MAX,
  } EventCode_t;

}

#define AP_EVENT_NODE_MAX           16
#define AP_EVENT_Q_MAX              8

#if 0
#define CMD_RS485_OPEN              0x0100
#define CMD_RS485_CLOSE             0x0101
#define CMD_RS485_DATA              0x0102

#define CMD_CAN_OPEN                0x0110
#define CMD_CAN_CLOSE               0x0111
#define CMD_CAN_DATA                0x0112
#define CMD_CAN_ERR_INFO            0x0113
#define CMD_CAN_SET_FILTER          0x0114
#define CMD_CAN_GET_FILTER          0x0115

#endif


#endif /* AP_AP_DEF_H_ */
