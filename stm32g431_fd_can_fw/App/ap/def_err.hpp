/*
 * def_err.hpp
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */

#ifndef AP_DEF_ERR_HPP_
#define AP_DEF_ERR_HPP_




#define errcode int

struct ap_err_st
{
  enum err_e : uint8_t
  {
    no_error,


    return_fail,
    undefined_err
  };
  ap_err_st() = default;
  ~ap_err_st() = default;

};


#define ERR_API_LCD_COMM_TIMEOUT  101



#endif /* AP_DEF_ERR_HPP_ */
