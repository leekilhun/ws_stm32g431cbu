/*
 * enComm.hpp
 *
 *  Created on: 2023. 9. 12.
 *      Author: gns2.lee
 */
#pragma once
#ifndef AP__INC_ENCOMM_HPP_
#define AP__INC_ENCOMM_HPP_

#include "ap_def.h"

// communication module interface
namespace cmi
{

  struct enComm
  {
    /****************************************************
     *  data
     ****************************************************/

    struct cfg_t
    {
      uart_cmd *ptr_comm{};
      task_boot* ptr_boot{};

    } m_cfg{};

    /****************************************************
     *  Constructor
     ****************************************************/
  public:
    enComm() = default;
    ~enComm() = default;

    /****************************************************
     *  Init
     ****************************************************/
    inline error_t Init(cfg_t &cfg)
    {
      m_cfg = cfg;
      //cfg.ptr_comm->AttCallbackFunc(this, receiveDataFunc);
      LOG_PRINT("Init Success!");
      return ERROR_SUCCESS;
    }

    // callback function
    inline static void receiveDataFunc(void *obj, void *w_parm, void *l_parm)
    {
      enComm *ptr_this = (enComm *)obj;

      // ptr_this->m_waitReplyOK = false;
      if (w_parm == nullptr && obj == nullptr && l_parm == nullptr)
        return;
      ptr_this->processPacket(*(cmi::uart_cmd::packet_st *)l_parm);
    }

    inline errno_t SendData(cmi::PACKET_TYPE type, uint16_t cmd, uint16_t err_code, uint8_t *p_data, uint32_t length)
    {
      // cmd_driver_t* p_driver = p_cmd->p_driver;
      // cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      uint8_t *ptr_tx_buffer = ((cmi::uart_cmd *)m_cfg.ptr_comm)->m_packet.buffer.data();
      uint32_t data_len;
      if (m_cfg.ptr_comm->IsOpen() != true)
        return false;
      enum : uint8_t
      {
        _stx0,
        _stx1,
        _type,
        _cmd_l,
        _cmd_h,
        _err_l,
        _err_h,
        _len_l,
        _len_h,
        _data,
        _chk,
        _max
      };
      // std::array<uint8_t, _max> datas {0,};
      data_len = length;

      ptr_tx_buffer[_stx0] = CMD_STX0;
      ptr_tx_buffer[_stx1] = CMD_STX1;
      ptr_tx_buffer[_type] = (uint8_t)type;
      ptr_tx_buffer[_cmd_l] = (cmd >> 0) & 0xFF;
      ptr_tx_buffer[_cmd_h] = (cmd >> 8) & 0xFF;
      ptr_tx_buffer[_err_l] = (err_code >> 0) & 0xFF;
      ptr_tx_buffer[_err_h] = (err_code >> 8) & 0xFF;
      ptr_tx_buffer[_len_l] = (data_len >> 0) & 0xFF;
      ptr_tx_buffer[_len_h] = (data_len >> 8) & 0xFF;
      uint16_t index;
      index = (uint16_t)_data;
      for (uint32_t i = 0; i < data_len; i++)
      {
        ptr_tx_buffer[index++] = p_data[i];
      }

      uint8_t check_sum = 0;
      for (uint16_t i = 0; i < index; i++)
      {
        check_sum += ptr_tx_buffer[i];
      }
      check_sum = (~check_sum) + 1;
      ptr_tx_buffer[index++] = check_sum;
      return m_cfg.ptr_comm->Send(ptr_tx_buffer, index);
    }

    inline bool sendPacket(cmi::PACKET_TYPE type, uint16_t cmd_code, uint16_t err_code, uint8_t *p_data, uint32_t length)
    {
      return SendData(type, cmd_code, err_code, p_data, length);
    }

    inline errno_t sendResp(cmi::uart_cmd::packet_st &packet, uint16_t err_code, uint8_t *p_data, uint32_t length)
    {
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, packet.cmd, err_code, p_data, length);
    }

    /**
     * @brief
     * check if the packet configured word has been transmitted.
     * @return true
     * @return false
     */
    inline bool receivePacket(void)
    {
      return m_cfg.ptr_comm->ReceiveProcess();
    }

    

    /**
     * @brief
     * Perform command by parsing packets.
     * performs only the task-boot command.
     * @param packet
     */
    inline void processPacket(cmi::uart_cmd::packet_st &packet)
    {
      if (m_cfg.ptr_boot->Process(packet) == false)
      {
        if (packet.type == PKT_TYPE_CMD)
        {
          
        }
      }
    }
  };
}
// end of namespace

#endif /* AP__INC_ENCOMM_HPP_ */
