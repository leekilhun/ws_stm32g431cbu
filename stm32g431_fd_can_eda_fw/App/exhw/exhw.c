/*
 * exhw.c
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */




#include "exhw.h"


bool exhwInit(void)
{

  ledInit();

  buttonInit();

  return true;
}
