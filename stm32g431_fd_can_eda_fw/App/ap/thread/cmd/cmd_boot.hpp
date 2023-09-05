/*
 * cmd_boot.hpp
 *
 *  Created on: Aug 28, 2023
 *      Author: gns2.lee
 */

#ifndef AP_THREAD_CMD_CMD_BOOT_HPP_
#define AP_THREAD_CMD_CMD_BOOT_HPP_



#include "ap_def.h"

//event driven architecture
namespace eda
{

  struct cmd_boot
  {

  public:
    cmd_boot() = default;
    ~cmd_boot()= default;


    static inline bool  Init(void)
    {
      return true;
    }

    static bool IsBusy(void);
    static void Update(cmd_t *p_cmd);
    static bool Process(cmd_t *p_cmd);


  };

}
// end of namespace


#endif /* AP_THREAD_CMD_CMD_BOOT_HPP_ */
