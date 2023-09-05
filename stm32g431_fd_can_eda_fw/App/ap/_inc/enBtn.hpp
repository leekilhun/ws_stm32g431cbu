/*
 * enBtn.hpp
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */

#ifndef AP__INC_ENBTN_HPP_
#define AP__INC_ENBTN_HPP_



struct enBtn
{
  struct cfg_t
  {
    GPIO_TypeDef * gpio_port{};
    uint16_t       gpio_pin{};

    cfg_t() = default;
    // copy constructor
    cfg_t(const cfg_t& rhs) = default;
    // copy assignment operator
    cfg_t& operator=(const cfg_t& rhs) = default;
    // move constructor
    cfg_t(cfg_t&& rhs) = default;
    // move assignment operator
    cfg_t& operator=(cfg_t&& rhs) = default;

    ~cfg_t() = default;

  } m_cfg{};

public:
  enBtn() = default;
  ~enBtn()= default;
  inline error_t Init(cfg_t& cfg){
    LOG_PRINT("Init Success!");
    m_cfg = cfg;
    return ERROR_SUCCESS;
  }

  inline bool IsPressed(){
    return (HAL_GPIO_ReadPin(m_cfg.gpio_port,m_cfg.gpio_pin)==GPIO_IN_STATE_ON);
  }

  inline bool IsReleased(){
    return !IsPressed();
  }

};


#endif /* AP__INC_ENBTN_HPP_ */
