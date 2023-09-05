/*
 * task_boot.hpp
 *
 *  Created on: 2023. 8. 28.
 *      Author: gns2.lee
 */

#pragma once
#ifndef APP_AP__INC_TASK_BOOT_HPP_
#define APP_AP__INC_TASK_BOOT_HPP_


#include "ap_def.h"

// communication module interface
namespace cmi
{

  struct task_boot
  {
     /****************************************************
     *  data
     ****************************************************/
    struct cfg_t
    {
      IComm* ptr_comm{};

      cfg_t() = default;
      // copy constructor
      cfg_t(const cfg_t& other) = default;
      // copy assignment
      cfg_t& operator=(const cfg_t& other) = default;
      // move constructor
      cfg_t(cfg_t&& other) = default;
      // move assignment
      cfg_t& operator=(cfg_t&& other) = default;
    } m_cfg{};

   /****************************************************
     *  Constructor
     ****************************************************/
  public:
    task_boot() = default;
    ~task_boot() = default;

    /****************************************************
     *  Init
     ****************************************************/
    inline error_t Init(cfg_t& cfg){
      LOG_PRINT("Init Success!");
      m_cfg = cfg;
      return ERROR_SUCCESS;
    }


  };

}
// end of namespace cmi

#endif /* APP_AP__INC_TASK_BOOT_HPP_ */
