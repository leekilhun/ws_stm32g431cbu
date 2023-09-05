/*
 * mode.hpp
 *
 *  Created on: Aug 28, 2023
 *      Author: gns2.lee
 */

#ifndef AP_THREAD_MODE_MODE_HPP_
#define AP_THREAD_MODE_MODE_HPP_



#include "ap_def.h"

namespace eda
{

  typedef enum
  {
    MODE_USB_TO_CAN,
    MODE_USB_TO_RS485,
    MODE_USB_TO_CLI,
  } ModeUsbMode_t;

  typedef enum
  {
    TYPE_USB_PACKET,
    TYPE_USB_UART,
  } ModeUsbType_t;


  typedef struct
  {
    ModeUsbMode_t (*getMode)(void);
    ModeUsbType_t (*getType)(void);
  } mode_obj_t;


  mode_obj_t *modeObj(void);

#if 0
  typedef struct
  {
    bool (*getTxUpdate)(void);
    bool (*getRxUpdate)(void);
  } can_obj_t;


  can_obj_t *canObj(void);


  typedef struct
  {
    bool (*getTxUpdate)(void);
    bool (*getRxUpdate)(void);
  } rs485_obj_t;


  rs485_obj_t *rs485Obj(void);
#endif

}
// end of namespace


#endif /* AP_THREAD_MODE_MODE_HPP_ */
