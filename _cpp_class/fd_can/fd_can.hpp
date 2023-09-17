#pragma once
#ifndef _FD_CAN_HPP
#define _FD_CAN_HPP

#include "../def.hpp"

class serial_comm;

//module system class
namespace msc
{


  enum PACKET_TYPE : uint8_t
  {
    PKT_TYPE_CMD = 0x00,
    PKT_TYPE_RESP = 0x01,
    PKT_TYPE_EVENT = 0x02,
    PKT_TYPE_LOG = 0x03,
    PKT_TYPE_PING = 0x04,
    PKT_TYPE_CAN = 0x05,
    PKT_TYPE_UART = 0x06,
  };

  enum CMD_TYPE : uint16_t
  {
    CMD_BOOT_INFO = 0x0000,
    CMD_BOOT_VERSION = 0x0001,
    CMD_BOOT_FLASH_ERASE = 0x0003,
    CMD_BOOT_FLASH_WRITE = 0x0004,
    CMD_BOOT_FLASH_READ = 0x0005,
    CMD_BOOT_FW_VER = 0x0006,
    CMD_BOOT_FW_ERASE = 0x0007,
    CMD_BOOT_FW_WRITE = 0x0008,
    CMD_BOOT_FW_READ = 0x0009,
    CMD_BOOT_FW_VERIFY = 0x000A,
    CMD_BOOT_FW_UPDATE = 0x000B,
    CMD_BOOT_FW_JUMP = 0x000C,
    CMD_BOOT_FW_BEGIN = 0x000D,
    CMD_BOOT_FW_END = 0x000E,
    CMD_BOOT_LED = 0x0010,

    CMD_RS485_OPEN = 0x0100,
    CMD_RS485_CLOSE = 0x0101,
    CMD_RS485_DATA = 0x0102,

    CMD_CAN_OPEN = 0x0110,
    CMD_CAN_CLOSE = 0x0111,
    CMD_CAN_DATA = 0x0112,
    CMD_CAN_ERR_INFO = 0x0113,
    CMD_CAN_SET_FILTER = 0x0114,
    CMD_CAN_GET_FILTER = 0x0115,
  };

  constexpr uint8_t CMD_STX0 = 0x02;
  constexpr uint8_t CMD_STX1 = 0xFD;


  constexpr int PKT_DATA_LENGTH = 1024;
  constexpr int CMD_MAX_PACKET_LENGTH = (PKT_DATA_LENGTH + 10);
  constexpr int PACKET_BUFF_LENGTH = CMD_MAX_PACKET_LENGTH;
  class fd_can
  {

public:
    struct cfg_t
    {
      serial_comm* ptr_uart_comm{  };

      cfg_t() = default;
      ~cfg_t() = default;

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
      uint8_t* data;

      uint8_t         buffer_idx{};
      uint16_t        data_cnt{};
      uint32_t        resp_ms{};
      bool            wait_resp{};
      step::state_st<> state{};

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

private:
    bool m_IsConnected{};
    cfg_t m_cfg{};
    packet_st m_packetData{};
    std::thread m_trd{};
    std::atomic<bool> m_stopThread{};
    std::function<int(void*, void*)> m_cb{};
    std::mutex m_mutex{};

    uint32_t m_msgTime{};

#ifdef _USE_HW_RTOS
    osMutexId m_mutex_id;
#endif

    /****************************************************
     *  Constructor
     ****************************************************/
  public:
    fd_can() = default;
    ~fd_can() = default;

    /****************************************************
     *  functions
     ****************************************************/


    inline void processCplt()
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      std::cout << "RxData.Type (resp time): " << std::hex << (int)m_packetData.type << std::dec << "(" << m_packetData.resp_ms << ")" << std::endl;
      PACKET_TYPE rx_type = (PACKET_TYPE)m_packetData.type;

      // 수신된 packet 정보를 0x00 0x00 형태로 출력
      std::cout << "RxData.Data: " << std::hex; 
      for (int i = 0; i < m_packetData.length; i++)
        std::cout << (int)m_packetData.data[i] << " ";
      std::cout << std::endl;      


      std::cout << std::dec;
      switch (rx_type)
      {
      case PKT_TYPE_CMD:
        break;

      case PKT_TYPE_RESP:
        break;

      case PKT_TYPE_EVENT:
        break;

      case PKT_TYPE_LOG:
        break;

      case PKT_TYPE_PING:
        break;

      case PKT_TYPE_CAN:
        break;

      case PKT_TYPE_UART:
        break;

      default:
        break;
      }
    }

    inline int UartCallback(void* w_parm, void* l_parm)
    {
      if (w_parm == nullptr)
        return -1;
      int length = *((int*)w_parm);
      int index = 0;

      for (index = 0; index < length; index++)
      {
        uint8_t data = *((uint8_t*)l_parm + index);
        if (receivePacket(data))
        {
          m_packetData.wait_resp = false;
          m_packetData.resp_ms = tim::millis() - m_msgTime;
          processCplt();
        }
      }

      return ERROR_SUCCESS;
    }

    inline bool receivePacket(uint8_t rx_data)
    {

      // std::cout << "rx_data [0x" << std::hex << (int)rx_data << "]"<< std::endl;
      enum : uint8_t
      {
        STATE_WAIT_STX0,
        STATE_WAIT_STX1,
        STATE_WAIT_TYPE,
        STATE_WAIT_CMD_L,
        STATE_WAIT_CMD_H,
        STATE_WAIT_ERR_L,
        STATE_WAIT_ERR_H,
        STATE_WAIT_LENGTH_L,
        STATE_WAIT_LENGTH_H,
        STATE_WAIT_DATA,
        STATE_WAIT_CHECKSUM,
      };

      if (m_packetData.state.MoreThan(100))
      {
        m_packetData.BufferClear();
      }

      switch (m_packetData.state.GetStep())
      {
      case STATE_WAIT_STX0:
        if (rx_data == CMD_STX0)
        {
          m_packetData.BufferClear();
          m_packetData.BufferAdd(rx_data);
          m_packetData.state.SetStep(STATE_WAIT_STX1);
        }
        break;


      case STATE_WAIT_TYPE:
        m_packetData.type = rx_data;
        m_packetData.BufferAdd(rx_data);
        m_packetData.state.SetStep(STATE_WAIT_CMD_L);
        break;

      case STATE_WAIT_CMD_L:
        m_packetData.cmd = rx_data;
        m_packetData.BufferAdd(rx_data);
        m_packetData.state.SetStep(STATE_WAIT_CMD_H);
        break;

      case STATE_WAIT_CMD_H:
        m_packetData.cmd |= (rx_data << 8);
        m_packetData.BufferAdd(rx_data);
        m_packetData.state.SetStep(STATE_WAIT_ERR_L);
        break;

      case STATE_WAIT_ERR_L:
        m_packetData.err_code = rx_data;
        m_packetData.BufferAdd(rx_data);
        m_packetData.state.SetStep(STATE_WAIT_ERR_H);
        break;

      case STATE_WAIT_ERR_H:
        m_packetData.err_code |= (rx_data << 8);
        m_packetData.BufferAdd(rx_data);
        m_packetData.state.SetStep(STATE_WAIT_LENGTH_L);
        break;

      case STATE_WAIT_LENGTH_L:
        m_packetData.length = rx_data;
        m_packetData.BufferAdd(rx_data);
        m_packetData.state.SetStep(STATE_WAIT_LENGTH_H);
        break;

      case STATE_WAIT_LENGTH_H:
        m_packetData.length |= (rx_data << 8);
        m_packetData.BufferAdd(rx_data);

        //LOG_PRINT("length : %d", m_packetData.length);

        if (m_packetData.length == 0)
          m_packetData.state.SetStep(STATE_WAIT_CHECKSUM);
        else if (m_packetData.length < (PKT_DATA_LENGTH + 1))
          m_packetData.state.SetStep(STATE_WAIT_DATA);
        else
          m_packetData.state.SetStep(STATE_WAIT_STX0);
        break;

      case STATE_WAIT_DATA:
        // assign data address
        if (m_packetData.data_cnt++ == 0)
          m_packetData.data = &m_packetData.buffer[m_packetData.buffer_idx];

        // check length
        if (m_packetData.data_cnt == m_packetData.length)
          m_packetData.state.SetStep(STATE_WAIT_CHECKSUM);

        m_packetData.BufferAdd(rx_data);
        break;

      case STATE_WAIT_CHECKSUM:
        m_packetData.check_sum_recv = rx_data;
        m_packetData.check_sum = (~m_packetData.check_sum) + 1;
        m_packetData.state.SetStep(STATE_WAIT_STX0);
        //LOG_PRINT("cal_checksum : %d,  recv_checksum : %d", m_packetData.check_sum, m_packetData.check_sum_recv);
        if (m_packetData.check_sum == m_packetData.check_sum_recv)
          return true;

        break;

      default:
        return false;
      }
      // end of  switch

      return false;
    }

  public:
    inline errno_t Init(const cfg_t& cfg)
    {
      m_cfg = cfg;
      // serial_comm::cfg_t comm_cfg{};
      m_cfg.ptr_uart_comm->registerCallback(std::bind(&fd_can::UartCallback, this, std::placeholders::_1, std::placeholders::_2));
      if (m_cfg.ptr_uart_comm->IsOpen())
      {
        m_IsConnected = true;
        return ERROR_SUCCESS;
      }
      else
        return m_cfg.ptr_uart_comm->OpenPort();
    }

    inline errno_t SendCmd(uint8_t* p_data, uint32_t length)
    {
      if (m_packetData.wait_resp)
        return -1;

      /*
       | STX0 | STX1 | Type  | objId | Data Length |Data      | Checksum |
       | :--- |:-----|:------|:----- |:------------|:---------| :------  |
       | 0x4A | 0x4C | 2byte | 2byte | 2 byte      |Data 0～n | 1byte    |
      */

      std::vector<uint8_t> datas{ CMD_STX0, CMD_STX1 };
      for (uint32_t i = 0; i < length; i++)
        datas.emplace_back(p_data[i]);

      uint8_t checksum = 0;
      for (const auto& elm : datas)
        checksum += elm;
      checksum = (~checksum) + 1;
      datas.emplace_back(checksum);

      std::cout << "TxData.Data: " << std::hex;
      for (const auto& elm : datas)
        std::cout << (int)elm << " ";
      std::cout << std::endl;

      m_msgTime = tim::millis();
      if (m_cfg.ptr_uart_comm->SendData((char*)datas.data(), (uint32_t)datas.size()) > 0)
        return ERROR_SUCCESS;
      else
        return -1;
    }

    inline bool SendCmdRxResp(uint8_t* p_data, uint32_t length, uint32_t timeout = 200)
    {
      if (SendCmd(p_data, length) == ERROR_SUCCESS)
      {
        uint32_t pre_ms = tim::millis();
        m_packetData.wait_resp = true;
        while (m_packetData.wait_resp)
        {
          if ((tim::millis() - pre_ms) > timeout)
          {
            ERR_PRINT("SendCmdRxResp timeout!");
            m_packetData.wait_resp = false;
            return false;
          }

          // Sleep(50);
          tim::delay(1);
        }
        // m_commRespTime = tim::millis() - pre_ms;
        // std::cout << "Response ms : " << std::to_string(m_commRespTime) << std::endl;
        return true;
      }
      return false;
    }

    // inline errno_t SendData(TX_TYPE type)
    // {
    //   return 0;
    // }

    inline errno_t Ret_OkResponse()
    {
      return 0;
      // std::vector<uint8_t> datas{};
      // datas.emplace_back((uint8_t)(TX_OK_RESPONSE >> 0));
      // datas.emplace_back((uint8_t)(TX_OK_RESPONSE >> 8));
      // datas.emplace_back(0x00); // obj id l
      // datas.emplace_back(0x00); // obj id h
      // datas.emplace_back(0x00); // data length l
      // datas.emplace_back(0x00); // data length h

      // return (SendCmdRxResp(datas.data(), (uint32_t)datas.size()) ? ERROR_SUCCESS : -1);
    }

    inline errno_t GetToolsInfo()
    {
      std::vector<uint8_t> datas{};
      // datas.emplace_back((uint8_t)(TX_READ_TOOL_INFO >> 0));
      // datas.emplace_back((uint8_t)(TX_READ_TOOL_INFO >> 8));
      // datas.emplace_back(0x00); // obj id l
      // datas.emplace_back(0x00); // obj id h
      // datas.emplace_back(0x00); // data length l
      // datas.emplace_back(0x00); // data length h

      return (SendCmdRxResp(datas.data(), (uint32_t)datas.size()) ? ERROR_SUCCESS : -1);
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    inline int testCB_func(int argc, char* argv[])
    {
      auto get_data = [](char* int_ptr) -> int
        {
          int ret_idx = 0;
          ret_idx = (int)strtoul((const char*)int_ptr, (char**)NULL, (int)0);
          return ret_idx;
        };

      bool ret{};
      enum : int
      {
        arg_cnt_0,
        arg_cnt_1,
        arg_cnt_2,
        arg_cnt_3,
        arg_cnt_4,
      };

      switch ((argc - 1))
      {
      case arg_cnt_1:
      {
        if (argv[1])
        {
          if (std::string(argv[1]).compare("info") == 0)
          {
            if (this->GetToolsInfo() == ERROR_SUCCESS)
            {
              std::cout << "fa_can info [" << this << "] success " << std::endl;
              ret = true;
            }
            else
              std::cout << "fa_can get info fail" << std::endl;
          }
          else  if (std::string(argv[1]).compare("open") == 0)
          {
            if (this->m_cfg.ptr_uart_comm->ChangeBaudrate(600) == ERROR_SUCCESS)
            {
              std::cout << "fa_can open [" << this << "] success " << std::endl;
              ret = true;
            }
            else
              std::cout << "fa_can get info fail" << std::endl;
          }          
        }
      }
      break;
      case arg_cnt_2:
      {
        if (argv[1])
        {
        }
      }
      break;
      case arg_cnt_3:
      {
        if (argv[1])
        {
        }
      }
      break;
      case arg_cnt_4:
      {
      }
      break;
      default:
        break;
      }
      // end of switch

      if (ret)
        return 0;

      std::cout << "tools info" << std::endl;
      std::cout << "tools open" << std::endl;

      return -1;
    }
  };
  // end of class fd_can

}
// end of namespace msc


#endif // !_FD_CAN_HPP
