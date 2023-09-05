/*
 * thread.hpp
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */
#pragma once
#ifndef AP_THREAD_THREAD_HPP_
#define AP_THREAD_THREAD_HPP_



#include "ap_def.h"

#include "cmd.hpp"

//event driven architecture
namespace eda
{

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

  typedef struct thread_t_
  {
    const char *name;
    bool is_enable;

    bool (*init)(void);
    bool (*update)(void);
  } thread_t;


  bool threadInit(void);
  bool threadUpdate(void);

}
// end of namespace

#endif /* AP_THREAD_THREAD_HPP_ */
