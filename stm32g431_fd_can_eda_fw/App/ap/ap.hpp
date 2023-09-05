/*
 * ap.hpp
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */

#ifndef AP_AP_HPP_
#define AP_AP_HPP_




#include "ap_def.h"

#include "def_obj.hpp"
#include "def_err.hpp"

/* ap object*/
#include "ap_utils.hpp"

//#include "ap_dat.hpp"
//#include "ap_log.hpp"

/* ap object */
/* register -> communication -> basic unit(engine) -> combined unit(engine) -> control */
//register + io_manager
//#include "ap_reg.hpp"
//#include "ap_io.hpp"

//basic
//#include "uart_moons.hpp"
//#include "uart_nextion.hpp"
//#include "uart_remote.hpp"

//#include "enOp.hpp"
//#include "enCyl.hpp"
//#include "enVac.hpp"
//#include "enMotor_moons.hpp"
//#include "enLed.hpp"
//#include "enBtn.hpp"



//control
//#include "cnMotors.hpp"
//#include "cnAuto.hpp"
//#include "cnTasks.hpp"

//machine test
//#include "machine_test.hpp"

//api
//#include "api_lcd.hpp"
//#include "api_remote.hpp"

#include "thread.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "cmd.hpp"
#include "cmd_driver.hpp"
#include "cmd_boot.hpp"


void apInit(void);
void apMain(void);




#endif /* AP_AP_HPP_ */
