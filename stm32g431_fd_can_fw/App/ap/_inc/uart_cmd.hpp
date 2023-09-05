/*
 * uart_cmd.hpp
 *
 *  Created on: 2023. 8. 28.
 *      Author: gns2.lee
 */
#pragma once
#ifndef AP__INC_UART_CMD_HPP_
#define AP__INC_UART_CMD_HPP_

/*

 communication module for interface with Master System (PC)

 */

#include "ap_def.h"

// communication module interface
namespace cmi
{


//TX  (secondary -> main) provide information
  enum TX_TYPE:uint16_t
  {
    TX_OK_RESPONSE      = 0x0000,
  };



  //RX (main -> secondary) request information or action

  enum PACKET_TYPE: uint8_t
  {
    PKT_TYPE_CMD        = 0x00,
    PKT_TYPE_RESP       = 0x01,
    PKT_TYPE_EVENT      = 0x02,
    PKT_TYPE_LOG        = 0x03,
    PKT_TYPE_PING       = 0x04,
    PKT_TYPE_CAN        = 0x05,
    PKT_TYPE_UART       = 0x06,
  } ;




  enum BOOT_CMD_TYPE:uint16_t
  {
    BOOT_CMD_INFO           = 0x0000,
    BOOT_CMD_VERSION        = 0x0001,
    BOOT_CMD_FLASH_ERASE    = 0x0003,
    BOOT_CMD_FLASH_WRITE    = 0x0004,
    BOOT_CMD_FLASH_READ     = 0x0005,
    BOOT_CMD_FW_VER         = 0x0006,
    BOOT_CMD_FW_ERASE       = 0x0007,
    BOOT_CMD_FW_WRITE       = 0x0008,
    BOOT_CMD_FW_READ        = 0x0009,
    BOOT_CMD_FW_VERIFY      = 0x000A,
    BOOT_CMD_FW_UPDATE      = 0x000B,
    BOOT_CMD_FW_JUMP        = 0x000C,
    BOOT_CMD_FW_BEGIN       = 0x000D,
    BOOT_CMD_FW_END         = 0x000E,
    BOOT_CMD_LED            = 0x0010,
  };


  constexpr uint8_t CMD_STX0              = 0x02;
  constexpr uint8_t CMD_STX1              = 0xFD;


  constexpr int PKT_DATA_LENGTH           = 1024;
  constexpr int CMD_MAX_PACKET_LENGTH     = (PKT_DATA_LENGTH + 10);
  constexpr int PACKET_BUFF_LENGTH        = CMD_MAX_PACKET_LENGTH;

#ifdef _USE_HW_RTOS
#define AP_UART_CMD_LOCK_BEGIN osMutexWait(m_mutex_id, osWaitForever)
#define AP_UART_CMD_LOCK_END   osMutexRelease(m_mutex_id)
#else
#define AP_UART_CMD_LOCK_BEGIN
#define AP_UART_CMD_LOCK_END
#endif




  struct uart_cmd : public IComm
  {
    /****************************************************
     *  data
     ****************************************************/
    struct cfg_t
    {
      uint8_t ch{};
      uint32_t baud{};

      cfg_t() = default;
      // copy constructor
      cfg_t(const cfg_t& other) = default;
      // copy assignment
      cfg_t& operator=(const cfg_t& other) = default;
      // move constructor
      cfg_t(cfg_t&& other) = default;
      // move assignment
      cfg_t& operator=(cfg_t&& other) = default;
    };


    struct packet_st {
      uint8_t        type{};
      uint16_t       cmd{};
      uint16_t       err_code{};
      uint16_t       length{};
      uint8_t        check_sum{};
      uint8_t        check_sum_recv{};
      std::array <uint8_t, PACKET_BUFF_LENGTH> buffer{};
      uint8_t*       data;

      uint8_t         buffer_idx{};
      uint16_t        data_cnt{};
      uint32_t        resp_ms{};
      prc_step_t      state{};

      packet_st() = default;
      // copy constructor
      packet_st(const packet_st& other) = default;
      // copy assignment
      packet_st& operator=(const packet_st& other) = default;
      // move constructor
      packet_st(packet_st&& other) = default;
      // move assignment
      packet_st& operator=(packet_st&& other) = default;
      ~packet_st() = default;

      uint8_t BufferAdd(uint8_t rx_data)
      {
        check_sum += rx_data;
        buffer[buffer_idx] = rx_data;
        buffer_idx = (buffer_idx + 1) % CMD_MAX_PACKET_LENGTH;
        return buffer_idx;
      }

      void BufferClear()
      {
        buffer.fill(0);
        buffer_idx = 0;
        data_cnt = 0;
        check_sum = 0;
        state.SetStep(0);
      }


    };

    bool m_Isconnected{};
    cfg_t m_cfg{};
    packet_st m_packet{};

    void* m_obj{};
    evt_cb m_func{};
    uint32_t m_packet_sending_ms{};
    uint8_t m_reqFlag{};

#ifdef _USE_HW_RTOS
    osMutexId m_mutex_id;
#endif

    /****************************************************
     *  Constructor
     ****************************************************/
  public:
    uart_cmd():m_Isconnected{}, m_cfg{}, m_packet{}, m_obj{}, m_func{}, m_packet_sending_ms{}, m_reqFlag{}
    {
#ifdef _USE_HW_RTOS
      osMutexDef(m_mutex_id);
      m_mutex_id = osMutexCreate (osMutex(m_mutex_id));
#endif

    }
    ~uart_cmd (){}

    /****************************************************
     *  func
     ****************************************************/

    inline void Init(cfg_t &cfg) {
      m_cfg = cfg;
      Open();
    }

    inline errno_t Open() {
      if (uartOpen(m_cfg.ch, m_cfg.baud))
      {
        LOG_PRINT("Init Success! Uart ch[%d], baud[%d]",m_cfg.ch, m_cfg.baud);
        m_Isconnected = true;
        return ERROR_SUCCESS;
      }
      return -1;
    } 


    inline bool Recovery() {
      uartClose(m_cfg.ch);
      /* */
      m_Isconnected = uartOpen(m_cfg.ch, m_cfg.baud);
      return m_Isconnected;
    }

    inline bool IsAvailableComm() const{
      return (m_reqFlag == 0);
    }

    inline bool IsConnected() const {
      return m_Isconnected;
    }

    inline bool ReceiveProcess(){
      bool ret = false;
      if (receivePacket())
      {
        receiveCplt();
        ret = true;
      }
      return ret;
    }

    inline void AttCallbackFunc(void* obj, evt_cb cb)
    {
      m_obj = obj;
      m_func = cb;
    }


    inline errno_t SendCmd(uint8_t* ptr_data, uint32_t size) {
      m_packet_sending_ms = millis();
      m_reqFlag++;
      if (uartWrite(m_cfg.ch, ptr_data, size)){
        return ERROR_SUCCESS;
      }
      return -1;
    }


    inline errno_t SendCmdRxResp(uint8_t* ptr_data, uint32_t size, uint32_t timeout){
      if (SendCmd(ptr_data, size) == ERROR_SUCCESS)
      {
        uint32_t pre_ms = millis();
        bool result = true;
        while (receivePacket() == false)
        {
          if ((millis() - pre_ms) > timeout)
          {
            result = false;
            break;
          }
        }
        if (result)
        {
          receiveCplt();
          return ERROR_SUCCESS;
        }
      }
      return -1;
    }


    inline bool receivePacket() {

      enum : uint8_t
      {
        STATE_WAIT_STX0 ,
        STATE_WAIT_STX1 ,
        STATE_WAIT_TYPE ,
        STATE_WAIT_CMD_L ,
        STATE_WAIT_CMD_H ,
        STATE_WAIT_ERR_L ,
        STATE_WAIT_ERR_H ,
        STATE_WAIT_LENGTH_L ,
        STATE_WAIT_LENGTH_H ,
        STATE_WAIT_DATA ,
        STATE_WAIT_CHECKSUM ,
      };

      uint8_t rx_data = 0x00 ;

      if (m_packet.state.MoreThan(100))
      {
        m_packet.BufferClear();
      }

      while (uartAvailable(m_cfg.ch))
      {
        rx_data = uartRead(m_cfg.ch);
        //LOG_PRINT("rx_data %d", rx_data);

        switch (m_packet.state.GetStep())
        {
          case STATE_WAIT_STX0:
            if (rx_data == CMD_STX0)
            {
              m_packet.BufferClear();
              m_packet.BufferAdd(rx_data);
              m_packet.state.SetStep(STATE_WAIT_STX1);
            }
            break;

          case STATE_WAIT_STX1:
            if (rx_data == CMD_STX1)
            {
              m_packet.BufferAdd(rx_data);
              m_packet.state.SetStep(STATE_WAIT_TYPE);
            }
            break;

          case STATE_WAIT_TYPE:
            m_packet.type = rx_data;
            m_packet.BufferAdd(rx_data);
            m_packet.state.SetStep(STATE_WAIT_CMD_L);
            break;

          case STATE_WAIT_CMD_L:
            m_packet.cmd = rx_data;
            m_packet.BufferAdd(rx_data);
            m_packet.state.SetStep(STATE_WAIT_CMD_H);
            break;

          case STATE_WAIT_CMD_H:
            m_packet.cmd |= (rx_data << 8);
            m_packet.BufferAdd(rx_data);
            m_packet.state.SetStep(STATE_WAIT_ERR_L);
            break;

          case STATE_WAIT_ERR_L:
            m_packet.err_code = rx_data;
            m_packet.BufferAdd(rx_data);
            m_packet.state.SetStep(STATE_WAIT_CMD_H);
            break;

          case STATE_WAIT_ERR_H:
            m_packet.err_code |= (rx_data << 8);
            m_packet.BufferAdd(rx_data);
            m_packet.state.SetStep(STATE_WAIT_LENGTH_L);
            break;

          case STATE_WAIT_LENGTH_L:
            m_packet.length = rx_data;
            m_packet.BufferAdd(rx_data);
            m_packet.state.SetStep(STATE_WAIT_LENGTH_H);
            break;

          case STATE_WAIT_LENGTH_H:
            m_packet.length |= (rx_data << 8);
            m_packet.BufferAdd(rx_data);

            if (m_packet.length == 0)
              m_packet.state.SetStep(STATE_WAIT_CHECKSUM);
            else if (m_packet.length < (PKT_DATA_LENGTH + 1))
              m_packet.state.SetStep(STATE_WAIT_DATA);
            else
              m_packet.state.SetStep(STATE_WAIT_STX0);
            break;

          case STATE_WAIT_DATA:
            // assign data address
            if (m_packet.data_cnt++ == 0)
              m_packet.data = &m_packet.buffer[m_packet.buffer_idx];

            // check length
            if (m_packet.data_cnt == m_packet.length)
              m_packet.state.SetStep(STATE_WAIT_CHECKSUM);

            m_packet.BufferAdd(rx_data);
            break;

          case STATE_WAIT_CHECKSUM:
            m_packet.check_sum_recv = rx_data;
            m_packet.check_sum = (~m_packet.check_sum) + 1;
            m_packet.state.SetStep(STATE_WAIT_STX0);
            if (m_packet.check_sum == m_packet.check_sum_recv)
              return true;

            break;

          default:
            return false;
        }
        // end of  switch
      }
      //while (uartAvailable(m_cfg.ch))




      return false;
    }


  private:
    inline void receiveCplt() {
      m_reqFlag = 0;
      m_packet.resp_ms = millis() - m_packet_sending_ms;
      if (m_func && m_obj)
      {
        m_func(m_obj,nullptr,&m_packet);
      }
    }


  };

}
// end of CMD namespace


#endif /* AP__INC_UART_CMD_HPP_ */
