/*
 * exhw.c
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */




#include "exhw.h"


bool exhwInit(void)
{

#ifdef _USE_EXHW_LED
  ledInit();
#endif

#ifdef _USE_EXHW_BUTTON
  buttonInit();
#endif

  return true;
}
