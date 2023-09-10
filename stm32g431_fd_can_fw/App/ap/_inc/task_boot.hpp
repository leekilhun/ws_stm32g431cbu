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
      enLed* ptr_rx_led{};
      enLed* ptr_tx_led{};

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


    struct boot_info_t
    {
      uint32_t mode{};
    };


    struct boot_version_t
    {
      firm_ver_t boot{};
      firm_ver_t firm{};
      firm_ver_t update{};

      boot_version_t() = default;
      // copy constructor
      boot_version_t(const boot_version_t& other) = default;
      // copy assignment
      boot_version_t& operator=(const boot_version_t& other) = default;
      // move constructor
      boot_version_t(boot_version_t&& other) = default;
      // move assignment
      boot_version_t& operator=(boot_version_t&& other) = default;
    };


    struct boot_begin_t
    {
      char     fw_name[64]{};
      uint32_t fw_size{};
    };


    boot_info_t    m_bootInfo{};
    boot_version_t m_bootVersion{};
    boot_begin_t   m_bootBegin{};

    bool m_isBegin{};
    uint32_t m_fwReceiveSize{};
    prc_step_t m_step{};
    /****************************************************
      *  Constructor
      ****************************************************/
  public:
    task_boot() = default;
    ~task_boot() = default;

    /****************************************************
     *  Init
     ****************************************************/
     // callback function
    inline static void receiveDataFunc(void* obj, void* w_parm, void* l_parm) {
      task_boot* ptr_this = (task_boot*)obj;

      //ptr_this->m_waitReplyOK = false;
      if (w_parm == nullptr && obj == nullptr && l_parm == nullptr)
        return;
      ptr_this->Process(*(cmi::uart_cmd::packet_st*)l_parm);
    }

    inline error_t Init(cfg_t& cfg) {
      LOG_PRINT("Init Success!");
      m_cfg = cfg;
      cfg.ptr_comm->AttCallbackFunc(this, receiveDataFunc);
      return ERROR_SUCCESS;
    }

    inline bool IsBusy(void) {
      return m_isBegin;
    }

    inline errno_t SendData(cmi::PACKET_TYPE type, uint16_t cmd, uint16_t err_code, uint8_t* p_data, uint32_t length) {
      //cmd_driver_t* p_driver = p_cmd->p_driver;
      //cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      uint8_t* ptr_tx_buffer = ((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet.buffer.data();
      uint32_t data_len;
      if (m_cfg.ptr_comm->IsOpen() != true) return false;
      enum :uint8_t {
        _stx0, _stx1, _type, _cmd_l, _cmd_h, _err_l, _err_h, _len_l, _len_h, _data, _chk, _max
      };
      //std::array<uint8_t, _max> datas {0,};
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

    inline errno_t SendBootInfo() {
      m_bootInfo.mode = 1;
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, CMD_OK, (uint8_t*)&m_bootInfo, sizeof(boot_info_t));
    }


    inline errno_t SendBootVersion() {
      firm_ver_t* p_boot = (firm_ver_t*)(FLASH_ADDR_BOOT + FLASH_SIZE_VER);
      firm_ver_t* p_firm = (firm_ver_t*)(FLASH_ADDR_FIRM + FLASH_SIZE_TAG + FLASH_SIZE_VER);
      firm_ver_t update{};
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      //std::memset(&boot_version, 0, sizeof(boot_version));
      //memset(&m_bootVersion, 0, sizeof(boot_version_t));
      if (p_boot->magic_number == VERSION_MAGIC_NUMBER)
        m_bootVersion.boot = *p_boot;

      if (p_firm->magic_number == VERSION_MAGIC_NUMBER)
        m_bootVersion.firm = *p_firm;

      flashRead(FLASH_ADDR_UPDATE + FLASH_SIZE_TAG + FLASH_SIZE_VER, (uint8_t*)&update, sizeof(firm_ver_t));
      if (update.magic_number == VERSION_MAGIC_NUMBER)
        m_bootVersion.update = update; //uint16_t cmd,
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, CMD_OK, (uint8_t*)&m_bootVersion, sizeof(boot_version_t));
    }

    inline errno_t SendFirmVersion() {
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, CMD_OK, (uint8_t*)(FLASH_ADDR_FIRM + FLASH_SIZE_VER), sizeof(firm_ver_t));
    }

    inline errno_t EraseFirm() {
      uint32_t addr = 0;
      uint32_t length = 0;
      //cmd_packet_t* p_packet = &p_cmd->packet;
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      uint16_t err_code = CMD_OK;

      addr = *(reinterpret_cast<uint32_t*>(&p_packet->data[0]));
      length = *(reinterpret_cast<uint32_t*>(&p_packet->data[4]));
      // addr = ((uint32_t)p_packet->data[0] << 0);
      // addr |= ((uint32_t)p_packet->data[1] << 8);
      // addr |= ((uint32_t)p_packet->data[2] << 16);
      // addr |= ((uint32_t)p_packet->data[3] << 24);

      // length = ((uint32_t)p_packet->data[4] << 0);
      // length |= ((uint32_t)p_packet->data[5] << 8);
      // length |= ((uint32_t)p_packet->data[6] << 16);
      // length |= ((uint32_t)p_packet->data[7] << 24);

      if ((addr + length) < FLASH_SIZE_FIRM)
      {
        if (flashErase(FLASH_ADDR_UPDATE + addr, length) != true)
        {
          err_code = ERR_BOOT_FLASH_ERASE;
        }
      }
      else
      {
        err_code = ERR_BOOT_WRONG_RANGE;
      }
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, NULL, 0);
    }




    inline errno_t WriteFirm() {
      uint32_t addr = 0;
      uint32_t length = 0;
      //uart_cmd::packet_t* p_packet = &m_cfg.ptr_comm->m_packet;
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      uint16_t err_code = CMD_OK;

      addr = *(reinterpret_cast<uint32_t*>(&p_packet->data[0]));
      length = *(reinterpret_cast<uint32_t*>(&p_packet->data[4]));

      if ((addr + length) < FLASH_SIZE_FIRM)
      {
        if (flashWrite(FLASH_ADDR_UPDATE + addr, &p_packet->data[8], length) == true)
        {
          uint32_t index = 0;
          uint32_t rd_len;
          uint8_t buf[32];

          if (m_isBegin && m_fwReceiveSize <= addr)
            m_fwReceiveSize = addr + length;

          while (index < length)
          {
            rd_len = constrain(length - index, 0, 32);

            flashRead(FLASH_ADDR_UPDATE + addr + index, buf, rd_len);
            for (uint32_t i = 0; i < rd_len; i++)
            {
              if (p_packet->data[8 + index + i] != buf[i])
              {
                err_code = ERR_BOOT_FLASH_WRITE;
                break;
              }
            }
            index += rd_len;

            if (err_code != CMD_OK)
            {
              break;
            }
          }
        }
        else
        {
          err_code = ERR_BOOT_FLASH_WRITE;
        }

      }
      else
      {
        err_code = ERR_BOOT_WRONG_RANGE;
      }

      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, NULL, 0);
    }

    inline errno_t ReadFirm()
    {
      uint32_t addr = 0;
      uint32_t length = 0;
      //uart_cmd::packet_t* p_packet = &m_cfg.ptr_comm->m_packet;
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      uint16_t err_code = CMD_OK;

      addr = *(reinterpret_cast<uint32_t*>(&p_packet->data[0]));
      length = *(reinterpret_cast<uint32_t*>(&p_packet->data[4]));

      if ((addr + length) < FLASH_SIZE_FIRM)
      {
        if (flashRead(FLASH_ADDR_UPDATE + addr, &p_packet->data[0], length) != true)
        {
          err_code = ERR_BOOT_FLASH_WRITE;
        }
      }
      else
      {
        err_code = ERR_BOOT_WRONG_RANGE;
      }
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, &p_packet->data[0], length);
    }



    inline errno_t VerifyFirm() {
      uint32_t addr = 0;
      uint32_t length = 0;
      uint16_t crc;
      uint16_t err_code = CMD_OK;
      uint32_t rd_len;
      //uint8_t  rd_buf[128];
      constexpr uint8_t buff_max = 128;
      std::array<uint8_t, buff_max> rd_buf{};
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;

      firm_tag_t tag;

      do
      {
        firm_tag_t* p_tag = (firm_tag_t*)&tag;

        flashRead(FLASH_ADDR_UPDATE, (uint8_t*)p_tag, sizeof(firm_tag_t));


        if (p_tag->magic_number != TAG_MAGIC_NUMBER)
        {
          err_code = ERR_BOOT_TAG_MAGIC;
          break;
        }

        if (p_tag->fw_size >= FLASH_SIZE_FIRM)
        {
          err_code = ERR_BOOT_TAG_SIZE;
          break;
        }

        addr = FLASH_ADDR_UPDATE + p_tag->fw_addr;
        length = p_tag->fw_size;
        crc = 0;

        uint32_t index;

        index = 0;
        while (index < length)
        {
          rd_len = length - index;
          if (rd_len > buff_max)
            rd_len = buff_max;

          if (flashRead(addr + index, rd_buf.data(), rd_len) != true)
          {
            err_code = ERR_BOOT_FLASH_READ;
            break;
          }

          index += rd_len;

          for (uint32_t i = 0; i < rd_len; i++)
          {
            utilUpdateCrc(&crc, rd_buf[i]);
          }
        }

        if (err_code == CMD_OK)
        {
          if (p_tag->fw_crc != crc)
          {
            err_code = ERR_BOOT_FW_CRC;
          }
        }
      } while (0);
      return SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, NULL, 0);
    }



    inline void UpdateFirm() {
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, CMD_OK, NULL, 0);
      delay(100);

      resetSetBootMode(1 << MODE_BIT_UPDATE);
      resetToReset();
    }



    inline void JumpFirm() {
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, CMD_OK, NULL, 0);
    }

    inline void BeginFirm() {
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      uint16_t err_code = CMD_OK;


      m_fwReceiveSize = 0;

      if (p_packet->length == sizeof(boot_begin_t))
      {
        memcpy(&m_bootBegin, p_packet->data, sizeof(boot_begin_t));
        m_isBegin = true;
      }
      else
      {
        err_code = ERR_BOOT_WRONG_RANGE;
      }
      SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, NULL, 0);
    }



    inline void EndFirm() {
      uint16_t err_code = CMD_OK;
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;
      m_isBegin = false;
      SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, NULL, 0);
    }

    inline void ToggleLed()
    {
      uint16_t err_code = CMD_OK;
      cmi::uart_cmd::packet_st* p_packet = &((cmi::uart_cmd*)m_cfg.ptr_comm)->m_packet;

      //ledOn(EXHW_LED_CH_RX);
      m_cfg.ptr_rx_led->On();
      m_cfg.ptr_tx_led->On();
      //ledOn(EXHW_LED_CH_TX);
      delay(50);
      //ledOff(EXHW_LED_CH_RX);
      m_cfg.ptr_rx_led->Off();
      m_cfg.ptr_tx_led->Off();
      //ledOff(EXHW_LED_CH_TX);
      SendData(cmi::PACKET_TYPE::PKT_TYPE_RESP, p_packet->cmd, err_code, NULL, 0);
    }





    inline bool Process(cmi::uart_cmd::packet_st& packet) {
      bool ret = true;

      enum : uint16_t
      {
        BOOT_CMD_INFO = 0x0000,
        BOOT_CMD_VERSION = 0x0001,

        BOOT_CMD_FLASH_ERASE = 0x0003,
        BOOT_CMD_FLASH_WRITE = 0x0004,
        BOOT_CMD_FLASH_READ = 0x0005,
        BOOT_CMD_FW_VER = 0x0006,
        BOOT_CMD_FW_ERASE = 0x0007,
        BOOT_CMD_FW_WRITE = 0x0008,
        BOOT_CMD_FW_READ = 0x0009,
        BOOT_CMD_FW_VERIFY = 0x000A,
        BOOT_CMD_FW_UPDATE = 0x000B,
        BOOT_CMD_FW_JUMP = 0x000C,
        BOOT_CMD_FW_BEGIN = 0x000D,
        BOOT_CMD_FW_END = 0x000E,
        BOOT_CMD_LED = 0x0010,

      };

      switch (packet.cmd)
      {
      case BOOT_CMD_INFO:
        SendBootInfo();
        break;

      case BOOT_CMD_VERSION:
        SendBootVersion();
        break;

      case BOOT_CMD_FW_VER:
        SendFirmVersion();
        break;

      case BOOT_CMD_FW_ERASE:
        EraseFirm();
        break;

      case BOOT_CMD_FW_WRITE:
        WriteFirm();
        break;

      case BOOT_CMD_FW_READ:
        ReadFirm();
        break;

      case BOOT_CMD_FW_VERIFY:
        VerifyFirm();
        break;

      case BOOT_CMD_FW_UPDATE:
        UpdateFirm();
        break;

      case BOOT_CMD_FW_JUMP:
        JumpFirm();
        break;

      case BOOT_CMD_FW_BEGIN:
        BeginFirm();
        break;

      case BOOT_CMD_FW_END:
        EndFirm();
        break;

      case BOOT_CMD_LED:
        ToggleLed();
        break;

      default:
        ret = false;
        break;
      }

      return ret;
    }

    inline   void ThreadJob() {

      doRunStep();

    }


    inline void doRunStep() {

      constexpr uint8_t step_retry_max = 3;
      constexpr uint32_t step_wait_delay = 50;

      enum STEP : uint8_t
      {
        STEP_INIT,
        STEP_TODO,
        STEP_TIMEOUT,
        STEP_RESET_COMM_ALARM,
        STEP_STATE_UPDATE,
        STEP_STATE_UPDATE_START,
        STEP_STATE_UPDATE_WAIT,
        STEP_STATE_UPDATE_END,
        STEP_DATA_UPDATE,
        STEP_DATA_UPDATE_START,
        STEP_DATA_UPDATE_WAIT,
        STEP_DATA_UPDATE_END,

      };


      switch (m_step.GetStep())
      {
      case STEP_INIT:
      {
        m_step.SetStep(STEP_TODO);
      }
      break;

      /*######################################################
        to do
      ######################################################*/
      case STEP_TODO:
      {
        if (m_step.msgQ.Available())
        {
          volatile uint8_t step{};
          m_step.msgQ.Get((uint8_t*)&step);
          m_step.SetStep(step);
        }
      }
      break;
      /*######################################################
        timeout
      ######################################################*/
      case STEP_TIMEOUT:
      {
        LOG_PRINT("STEP_TIMEOUT recovery result[%d]", m_cfg.ptr_comm->Recovery());

        m_step.SetStep(STEP_TODO);
      }
      break;
      /*######################################################
        STEP_STATE_UPDATE
      ######################################################*/
      case STEP_STATE_UPDATE:
      {
        m_step.wait_resp = false;
        m_step.wait_step = 0;
        m_step.retry_cnt = 0;

        m_step.SetStep(STEP_STATE_UPDATE_START);
      }
      break;

      case STEP_STATE_UPDATE_START:
      {
        //if (SendData(tx_t::TX_MCU_DATA) == ERROR_SUCCESS)
        //  m_step.SetStep(STEP_STATE_UPDATE_WAIT);
        //else
        {
          LOG_PRINT("STEP_STATE_UPDATE_START send data failed!");
          m_step.SetStep(STEP_TIMEOUT);
        }
      }
      break;

      case STEP_STATE_UPDATE_WAIT:
      {
        if (m_step.LessThan(step_wait_delay))
          break;

        if (m_cfg.ptr_comm->IsAvailableComm())
          m_step.SetStep(STEP_STATE_UPDATE_END);
        else
        {
          if (m_step.retry_cnt++ < step_retry_max)
          {
            m_step.SetStep(STEP_STATE_UPDATE_WAIT); // timer reset
            break;
          }
          m_step.SetStep(STEP_TIMEOUT);
          LOG_PRINT("STEP_TIMEOUT retry[%d]", m_step.retry_cnt);
        }
      }
      break;

      case STEP_STATE_UPDATE_END:
      {

        m_step.SetStep(STEP_TODO);
      }
      break;
      /*######################################################
          STEP_DATA_UPDATE
        ######################################################*/
      case STEP_DATA_UPDATE:
      {
        m_step.wait_resp = false;
        m_step.wait_step = 0;
        m_step.retry_cnt = 0;

        m_step.SetStep(STEP_DATA_UPDATE_START);
      }
      break;

      case STEP_DATA_UPDATE_START:
      {
        //if (SendData(tx_t::TX_CTRL_DATA) == ERROR_SUCCESS)
        //  m_step.SetStep(STEP_DATA_UPDATE_WAIT);
        //else
        {
          LOG_PRINT("STEP_DATA_UPDATE_START send data failed!");
          m_step.SetStep(STEP_TIMEOUT);
        }
      }
      break;

      case STEP_DATA_UPDATE_WAIT:
      {
        if (m_step.LessThan(step_wait_delay))
          break;

        if (m_cfg.ptr_comm->IsAvailableComm())
          m_step.SetStep(STEP_DATA_UPDATE_END);
        else
        {
          if (m_step.retry_cnt++ < step_retry_max)
          {
            m_step.SetStep(STEP_DATA_UPDATE_WAIT); // timer reset
            break;
          }
          m_step.SetStep(STEP_TIMEOUT);
          LOG_PRINT("STEP_TIMEOUT retry[%d]", m_step.retry_cnt);
        }
      }
      break;

      case STEP_DATA_UPDATE_END:
      {

        m_step.SetStep(STEP_TODO);
      }
      break;

      default:
        break;
      }
      // end of switch

    }







#if 0




    //////////////////////////////////////////////////












    void eda::cmd_boot::Update(cmd_t* p_cmd)
    {
    }


#endif
    //////////////////////////////////////////////////
  };

}
// end of namespace cmi

#endif /* APP_AP__INC_TASK_BOOT_HPP_ */
