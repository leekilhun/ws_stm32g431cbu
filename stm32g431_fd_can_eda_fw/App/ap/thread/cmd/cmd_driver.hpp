/*
 * cmd_driver.hpp
 *
 *  Created on: Aug 28, 2023
 *      Author: gns2.lee
 */

#ifndef AP_THREAD_CMD_CMD_DRIVER_HPP_
#define AP_THREAD_CMD_CMD_DRIVER_HPP_


#include "ap_def.h"

//event driven architecture
namespace eda
{
   
  /**
   * @brief 
   *  communication class for uart packet communication
   */
  struct cmd_uart
  {
    typedef struct
    {
      uint8_t  ch;
      uint32_t baud;
    } cmd_uart_args_t;

  public:
    //cmd_uart() = default;
    //~cmd_uart()= default;


    static inline bool open(void *args)
    {
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

      logPrintf("[OK] cmdUartOpen()\n");

      return uartOpen(p_args->ch, p_args->baud);
    }

    static inline bool close(void *args)
    {
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

      return uartClose(p_args->ch);
    }

    static inline uint32_t available(void *args)
    {
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

      if (p_args->ch != cliGetPort())
        return uartAvailable(p_args->ch);
      else
        return 0;
    }

    static inline bool flush(void *args)
    {
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

      return uartFlush(p_args->ch);
    }

    static inline uint8_t read(void *args)
    {
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

      return uartRead(p_args->ch);
    }

    static inline uint32_t write(void *args, uint8_t *p_data, uint32_t length)
    {
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

      return uartWrite(p_args->ch, p_data, length);
    }


    static inline bool InitDriver(cmd_driver_t *p_driver, uint8_t ch, uint32_t baud){
      cmd_uart_args_t *p_args = (cmd_uart_args_t *)p_driver->args;

      p_args->ch = ch;
      p_args->baud = baud;

      p_driver->open = open;
      p_driver->close = close;
      p_driver->available = available;
      p_driver->flush = flush;
      p_driver->read = read;
      p_driver->write= write;

      return true;
    }

  };

}
// end of namespace


#endif /* AP_THREAD_CMD_CMD_DRIVER_HPP_ */
