/*
 * ap.cpp
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */




#include "ap.hpp"



void apInit(void)
{
  eda::threadInit();
}


void apMain(void)
{
  while(1)
  {
    eda::threadUpdate();
  }

}
