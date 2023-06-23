/*
 * hw.c
 *
 *  Created on: Jun 9, 2023
 *      Author: gns2l
 */


#include "hw.h"


bool hwInit(void)
{
  bool ret = true;

#ifdef _USE_HW_CLI
  ret &= cliInit();
#endif

#ifdef _USE_HW_LED
  ret &= ledInit();
#endif

#ifdef _USE_HW_UART
  ret &= uartInit();
#endif

#ifdef _USE_HW_GPIO
  ret &= gpioInit();
#endif

#ifdef _USE_HW_LOG
  ret &= logInit();

  logOpen(HW_LOG_CH, 115200);
  logPrintf("[ fw Begin... ]\r\n");
#else // use small size log func
/*
  uartOpen(HW_LOG_CH, 115200);
  logPrintf("[ Firmware Begin... ]\r\n\n\n");
  logPrintf("Booting..Name \t\t: %s\r\n", p_boot_ver->name);
  logPrintf("Booting..Ver  \t\t: %s\r\n", p_boot_ver->version);
  logPrintf("Firmware.Name \t\t: %s\r\n", _DEF_FW_NAME);
  logPrintf("Firmware.Ver  \t\t: %s\r\n", _DEF_FW_VERSION);
  logPrintf("Core Clock    \t\t: %d Mhz\r\n\n\n", SystemCoreClock/1000000);
  */
#endif


#ifdef _USE_HW_I2C
  ret &= i2cInit();
#endif

#ifdef _USE_HW_FLASH
  ret &= flashInit();
#endif


#ifdef _USE_HW_CAN
  ret &= canInit();
#endif


  return ret;
}

