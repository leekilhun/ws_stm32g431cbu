/*
 * event.hpp
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */
#pragma once
#ifndef AP_THREAD_EVENT_HPP_
#define AP_THREAD_EVENT_HPP_

#include "ap_def.h"

// event driven architecture
namespace eda
{
  constexpr int EVENT_Q_MAX = 8;
  constexpr int EVENT_NODE_MAX = 16;

  constexpr const char *event_str[] =
      {
          "EVENT_MODE_CHANGE",
          "EVENT_USB_OPEN",
      };

  struct event_t
  {
    EventCode_t code{};
    uint32_t data{};
  };

  struct event
  {
    struct event_node_t
    {
      int32_t count{};
      bool (*event_func[EVENT_NODE_MAX])(event_t *p_event){};
    };


  public:
    event() = default;
    ~event() = default;

    static error_t Init();
    static bool NodeAdd(bool (*func)(event_t *p_event));
    static bool Availble(void);
    static bool Get(event_t *p_event);
    static bool Put(event_t *p_event);
    static bool Update(void);
  };
}
// end of namespace

#endif /* AP_THREAD_EVENT_HPP_ */
