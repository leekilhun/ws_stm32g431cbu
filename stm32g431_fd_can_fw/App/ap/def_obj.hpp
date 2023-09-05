/*
 * def_obj.hpp
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */

#ifndef AP_DEF_OBJ_HPP_
#define AP_DEF_OBJ_HPP_



#define AP_DEF_OBJ_BASE_MOTOR                   0x0010
#define AP_DEF_OBJ_BASE_CYLINDER                0x0100
#define AP_DEF_OBJ_BASE_VACUUM                  0x0120

#define AP_DEF_OBJ_BASE_IO                      0x0140
#define AP_DEF_START_IN_ADDR                    1000
#define AP_DEF_START_OUT_ADDR                   3000




#define AP_DEF_OBJ_MOTOR_ID_BASE                1
#define M_SetMotorId(instance_no)               AP_DEF_OBJ_MOTOR_ID_BASE + instance_no
#define M_GetMotorInstanceId(node_id)           node_id - AP_DEF_OBJ_MOTOR_ID_BASE
#define M_GetMotorObjId(obj)                    AP_DEF_OBJ_BASE_MOTOR|obj

namespace AP_OBJ
{
  enum MOTOR
  {
    MOTOR_MAX,

  };

  enum CYL
  {

    CYL_MAX,

  };

  enum VAC
  {
    VAC_MAX,
  };

  enum LED
  {
    LED_STATUS,
    LED_COMM_485,
    LED_COMM_CAN,
    LED_COMM_RX,
    LED_COMM_TX,
    LED_MAX,
  };


  enum BTN
  {
    BTN_BOOT,
    BTN_S1,
    BTN_S2,
    BTN_MAX,
  };


}



#endif /* AP_DEF_OBJ_HPP_ */
