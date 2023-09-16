/*
 * cnComm.hpp
 *
 *  Created on: 2023. 9. 12.
 *      Author: gns2.lee
 */
#pragma once
#ifndef AP__INC_CN_COMM_HPP_
#define AP__INC_CN_COMM_HPP_

#include "ap_def.h"

// communication module interface
namespace cmi
{

  static const char *usb_mode_str[] =
      {
          "MODE_USB_TO_CAN",
          "MODE_USB_TO_RS485",
          "MODE_USB_TO_CLI"};
  static const char *usb_type_str[] =
      {
          "TYPE_USB_PACKET",
          "TYPE_USB_UART",
  };

  struct cnComm
  {
    enum ModeUsbMode_t
    {
      MODE_USB_TO_CAN,
      MODE_USB_TO_RS485,
      MODE_USB_TO_CLI,
      MODE_USB_MAX,
    };

    enum ModeUsbType_t
    {
      TYPE_USB_PACKET,
      TYPE_USB_UART,
    };
    /****************************************************
     *  data
     ****************************************************/

    struct cfg_t
    {

      enComm *ptr_comm{};

      enLed *ptr_leds{};
      enBtn *prt_btns{};

      uint8_t can_ch{};
      CanFilterType_t can_filter_type{};
      CanIdType_t can_filter_id_type{};
      uint32_t can_filter_id1{};
      uint32_t can_filter_id2{};
      bool (*can_open)(uint8_t, CanMode_t, CanFrame_t, CanBaud_t, CanBaud_t){};
      bool (*can_IsOpen)(uint8_t){};
      void (*can_close)(uint8_t){};
      bool (*can_update)(void){};
      uint32_t (*can_MsgAvailable)(uint8_t){};
      bool (*can_MsgRead)(uint8_t, can_msg_t *){};
      bool (*can_MsgInit)(can_msg_t *, CanFrame_t, CanIdType_t, CanDlc_t){};
      bool (*can_MsgWrite)(uint8_t, can_msg_t *, uint32_t){};
      bool (*can_SetFilterType)(CanFilterType_t){};
      bool (*can_ConfigFilter)(uint8_t, uint8_t, CanIdType_t, uint32_t, uint32_t);

      uint8_t rs485_ch{};
      uint32_t uart_baud{};
      bool (*uart_open)(uint8_t, uint32_t){};
      uint32_t (*uart_available)(uint8_t){};
      uint8_t (*uart_read)(uint8_t){};
      uint32_t (*uart_GetBaud)(uint8_t){};
      uint32_t (*uart_write)(uint8_t, uint8_t *, uint32_t);

      bool (*usb_IsOpen)(void){};
      bool (*eeprom_ReadByte)(uint32_t, uint8_t *);
      bool (*eeprom_WriteByte)(uint32_t, uint8_t);

      cfg_t() = default;
      // copy constructor
      cfg_t(const cfg_t &other) = default;
      // copy assignment
      cfg_t &operator=(const cfg_t &other) = default;
      // move constructor
      cfg_t(cfg_t &&other) = default;
      // move assignment
      cfg_t &operator=(cfg_t &&other) = default;
    } m_cfg{};

    ModeUsbMode_t m_usbMode{};
    ModeUsbType_t m_usbType{};
    ModeUsbMode_t m_preUsbMode{};
    ModeUsbType_t m_preUsbType{};

    bool m_isUsbOpen{};
    bool m_isCanOpen{};
    bool m_is485Open{};

    bool m_isRxUpdate{};
    bool m_isTxUpdate{};

    prc_step_t m_step{};
    /****************************************************
     *  Constructor
     ****************************************************/
  public:
    cnComm() = default;
    ~cnComm() = default;

    /****************************************************
     *  Init
     ****************************************************/

    inline error_t Init(cfg_t &cfg)
    {
      m_cfg = cfg;
      uint8_t ee_data = 0;
      m_cfg.eeprom_ReadByte(HW_EEPROM_MODE, &ee_data);

      m_usbMode = (ModeUsbMode_t)constrain(ee_data, MODE_USB_TO_CAN, MODE_USB_TO_CLI);
      m_usbType = TYPE_USB_PACKET;

      m_isUsbOpen = m_cfg.usb_IsOpen();

      LOG_PRINT("[  ] Mode : %s", usb_mode_str[m_usbMode]);
      LOG_PRINT("[  ] Type : %s", usb_type_str[m_usbType]);

      m_preUsbMode = m_usbMode;
      m_preUsbType = m_usbType;
      //LOG_PRINT("Init Success! USB Open[%s]", m_isUsbOpen ? "True" : "False");
      return ERROR_SUCCESS;
    }

    /**
     * @brief
     * re-open can communication through command information
     * @return true
     * @return false
     */
    inline bool canCmdOpen(cmi::uart_cmd::packet_st &packet)
    {
      CanMode_t mode;
      CanFrame_t frame;
      CanBaud_t baud;
      CanBaud_t baud_data;

      mode = (CanMode_t)packet.data[0];
      frame = (CanFrame_t)packet.data[1];
      baud = (CanBaud_t)packet.data[2];
      baud_data = (CanBaud_t)packet.data[3];

      LOG_PRINT("     mode      : %d", mode);
      LOG_PRINT("     frame     : %d", frame);
      LOG_PRINT("     baud      : %d", baud);
      LOG_PRINT("     baud_data : %d", baud_data);

      uint8_t can_ch = m_cfg.can_ch;
      if (m_cfg.can_IsOpen(can_ch) == true)
      {
        m_cfg.can_close(can_ch);
      }

      m_isCanOpen = m_cfg.can_open(can_ch, mode, frame, baud, baud_data);
      LOG_PRINT("     is_open   : %s", m_isCanOpen ? "True" : "False");

      if (m_cfg.can_SetFilterType(m_cfg.can_filter_type) != true)
        LOG_PRINT("can_SetFilterType fail !");

      if (m_cfg.can_ConfigFilter(can_ch, 0, m_cfg.can_filter_id_type, m_cfg.can_filter_id1, m_cfg.can_filter_id2) != true)
        LOG_PRINT("can_ConfigFilter fail !");        

      if (m_cfg.ptr_comm->sendResp(packet, OK, NULL, 0) != ERROR_SUCCESS)
        LOG_PRINT("sendResp fail !");
      return true;
    }

    inline bool canCmdClose(cmi::uart_cmd::packet_st &packet)
    {
      LOG_PRINT("[  ] canCmdClose()");
      m_isCanOpen = false;

      if (m_cfg.ptr_comm->sendResp(packet, OK, NULL, 0) != ERROR_SUCCESS)
        LOG_PRINT("sendResp fail !");
      return true;
    }

    inline bool canCmdData(cmi::uart_cmd::packet_st &packet)
    {
      can_msg_t msg;

      if (m_isCanOpen != true)
        return false;

      // USB -> CAN
      //
      m_isTxUpdate = true;

      CanFrame_t frame;
      CanIdType_t id_type;
      CanDlc_t dlc;
      data_t id;
      data_t timestamp;
      uint8_t length;

      (void)timestamp;
      (void)length;

      timestamp.u8Data[0] = packet.data[0];
      timestamp.u8Data[1] = packet.data[1];

      frame = (CanFrame_t)packet.data[2];
      id_type = (CanIdType_t)packet.data[3];
      dlc = (CanDlc_t)packet.data[4];

      id.u8Data[0] = packet.data[5];
      id.u8Data[1] = packet.data[6];
      id.u8Data[2] = packet.data[7];
      id.u8Data[3] = packet.data[8];

      length = packet.data[9];

      m_cfg.can_MsgInit(&msg, frame, id_type, dlc);
      for (int i = 0; i < msg.length; i++)
        msg.data[i] = packet.data[10 + i];

      msg.id = id.u32D;

      // logPrintf("id    0x%X\n", msg.id);
      // logPrintf("frame   %d\n", msg.frame);
      // logPrintf("id_type %d\n", msg.id_type);
      // logPrintf("dlc     %d\n", msg.dlc);

      m_cfg.can_MsgWrite(m_cfg.can_ch, &msg, 10);

      return true;
    }

    inline bool canCmdSetFilter(cmi::uart_cmd::packet_st &packet)
    {
      if (m_isCanOpen != true)
        return false;

      m_cfg.can_filter_type = (CanFilterType_t)packet.data[0];
      m_cfg.can_filter_id_type = (CanIdType_t)packet.data[1];

      memcpy(&m_cfg.can_filter_id1, &packet.data[2], 4);
      memcpy(&m_cfg.can_filter_id2, &packet.data[6], 4);

      // logPrintf("type    : %d\n", can_filter_type);
      // logPrintf("id_type : %d\n", can_filter_id_type);
      // logPrintf("id1     : 0x%X\n", can_filter_id1);
      // logPrintf("id2     : 0x%X\n", can_filter_id2);

      m_cfg.can_SetFilterType(m_cfg.can_filter_type);
      m_cfg.can_ConfigFilter(m_cfg.can_ch, 0, m_cfg.can_filter_id_type, m_cfg.can_filter_id1, m_cfg.can_filter_id2);
      if (m_cfg.ptr_comm->sendResp(packet, OK, NULL, 0) != ERROR_SUCCESS)
        LOG_PRINT("sendResp fail !");
      return true;
    }

    bool canCmdGetFilter(cmi::uart_cmd::packet_st &packet)
    {
      uint8_t buf[64];

      if (m_isCanOpen != true)
        return false;

      buf[0] = (uint8_t)m_cfg.can_filter_type;
      buf[1] = (uint8_t)m_cfg.can_filter_id_type;

      memcpy(&buf[2], &m_cfg.can_filter_id1, 4);
      memcpy(&buf[6], &m_cfg.can_filter_id2, 4);

      if (m_cfg.ptr_comm->sendResp(packet, OK, buf, 10) != ERROR_SUCCESS)
        LOG_PRINT("sendResp fail !");

      return true;
    }

    /**
     * @brief
     * It is called from the main thread and has one state-machine.
     * It must be written in non-block code.
     *
     */
    inline void ThreadJob()
    {

      /*
      Check the mode switching switch event and usb connection status.
      */
      checkPortEvent();

      /*
      update the packets received by the can-port.
       */
      canPortUpdate();

      /*
      update the packets received by the rs485-port.
       */
      rs485PortUpdate();

      /*
       */

      if (m_usbType == TYPE_USB_PACKET)
      {
        
        if (m_cfg.ptr_comm->receivePacket())
        {

          /**
           *  check a cmd of the packet received.
           *  check index 
           *  1. is it a boot cmd ?
           *  2. is it a task cmd ?
           *  3. return unknown cmd
           */
          cmi::uart_cmd::packet_st packet = m_cfg.ptr_comm->m_cfg.ptr_comm->m_packet;
          if (m_cfg.ptr_comm->m_cfg.ptr_boot->Process(packet))
          {
            LOG_PRINT("boot Process cplt");
          }
          else if (Process(packet))
          {
            LOG_PRINT("cmd task Process cplt");
          }
          else
          {
            if (m_cfg.ptr_comm->sendResp(packet, ERR_CMD_NO_CMD, NULL, 0) != ERROR_SUCCESS)
              LOG_PRINT("sendResp fail !");
          }
        }
      }
      /*
       C
      */
      doRunStep();
    }

    /**
     * @brief
     * re-open RS485 port with usb command
     * @param packet
     * @return true
     * @return false
     */
    inline bool rs485CmdOpen(cmi::uart_cmd::packet_st &packet)
    {
      data_t baud;

      memcpy(baud.u8Data, &packet.data[0], 4);

      logPrintf("[  ] rs485CmdOpen()\n");
      logPrintf("     %d bps\n", baud.u32D);

      m_is485Open = true;
      m_cfg.uart_baud = baud.u32D;

      if (m_cfg.ptr_comm->sendResp(packet, OK, NULL, 0) != ERROR_SUCCESS)
        LOG_PRINT("sendResp fail !");
      return true;
    }

    inline bool rs485CmdClose(cmi::uart_cmd::packet_st &packet)
    {
      LOG_PRINT("[  ] rs485CmdClose()");
      m_is485Open = false;

      if (m_cfg.ptr_comm->sendResp(packet, OK, NULL, 0) != ERROR_SUCCESS)
        LOG_PRINT("sendResp fail !");
      return true;
    }

    inline bool rs485CmdData(cmi::uart_cmd::packet_st &packet)
    {
      // USB -> RS485
      //
      m_isTxUpdate = true;
      m_cfg.uart_write(m_cfg.rs485_ch, packet.data, packet.length);
      return true;
    }

    /**
     * @brief
     * Check the received information using USB ↔ RS485 uart communication. (CLI)
     */
    inline void rs485UpdateUart(void)
    {
      constexpr int buff_max = 256;
      uint32_t length;
      uint8_t buf[buff_max];
      uint32_t usb_baud;
      usb_baud = m_cfg.uart_GetBaud(HW_UART_CH_USB);

      if (usb_baud != m_cfg.uart_GetBaud(m_cfg.rs485_ch))
      {
        m_cfg.uart_open(m_cfg.rs485_ch, usb_baud);
        LOG_PRINT("[  ] rs485 baud %d", m_cfg.uart_GetBaud(m_cfg.rs485_ch));
      }

      // USB -> RS485
      //
      length = cmin(m_cfg.uart_available(HW_UART_CH_USB), buff_max);
      if (length > 0)
      {
        m_isTxUpdate = true;
        for (uint32_t i = 0; i < length; i++)
        {
          buf[i] = m_cfg.uart_read(HW_UART_CH_USB);
        }
        m_cfg.uart_write(m_cfg.rs485_ch, buf, length);
      }

      // RS485 -> USB
      //
      length = cmin(m_cfg.uart_available(m_cfg.rs485_ch), buff_max);
      if (length > 0)
      {
        m_isRxUpdate = true;
        for (uint32_t i = 0; i < length; i++)
        {
          buf[i] = m_cfg.uart_read(m_cfg.rs485_ch);
        }
        m_cfg.uart_write(HW_UART_CH_USB, buf, length);
      }
    }

    /**
     * @brief
     * Check the received information using USB ↔ RS485 packet communication.
     */
    inline void rs485UpdatePacket(void)
    {
      constexpr int buff_max = 256;
      uint32_t length;
      uint8_t buf[buff_max];

      if (m_is485Open != true)
        return;

      if (m_cfg.uart_baud != m_cfg.uart_GetBaud(m_cfg.rs485_ch))
      {
        m_cfg.uart_open(m_cfg.rs485_ch, m_cfg.uart_baud);
        LOG_PRINT("[  ] rs485 baud %d", m_cfg.uart_GetBaud(m_cfg.rs485_ch));
      }

      // RS485 -> USB
      //
      length = cmin(m_cfg.uart_available(m_cfg.rs485_ch), buff_max);
      if (length > 0)
      {
        m_isRxUpdate = true;
        for (uint32_t i = 0; i < length; i++)
        {
          buf[i] = m_cfg.uart_read(m_cfg.rs485_ch);
        }
        if (m_cfg.ptr_comm->sendPacket(PKT_TYPE_UART, CMD_RS485_DATA, OK, buf, length) != ERROR_SUCCESS)
          LOG_PRINT("sendPacket fail !");
      }
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    inline bool rs485PortUpdate()
    {
      if ((m_usbMode == MODE_USB_TO_CLI && m_usbType == TYPE_USB_UART) == false)
      {
        if (m_usbType == TYPE_USB_UART)
          rs485UpdateUart();
        else
          rs485UpdatePacket();
      }
      else
      {
        while (m_cfg.uart_available(m_cfg.rs485_ch))
        {
          m_cfg.uart_read(m_cfg.rs485_ch);
        }
      }

      return true;
    }

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    inline bool canPortUpdate()
    {
      if ((m_usbMode == MODE_USB_TO_CLI && m_usbType == TYPE_USB_UART) == false)
      {
        m_cfg.can_update();
        uint8_t can_ch = m_cfg.can_ch;

        if (m_cfg.can_MsgAvailable(can_ch))
        {
          can_msg_t msg;
          uint8_t buf[256];
          data_t stamp;
          uint8_t index;

          m_cfg.can_MsgRead(can_ch, &msg);

          m_isRxUpdate = true;
          stamp.u32D = millis();

          index = 0;
          buf[index++] = stamp.u8Data[0];
          buf[index++] = stamp.u8Data[1];
          buf[index++] = (uint8_t)msg.frame;
          buf[index++] = (uint8_t)msg.id_type;
          buf[index++] = (uint8_t)msg.dlc;
          buf[index++] = (uint8_t)(msg.id >> 0);
          buf[index++] = (uint8_t)(msg.id >> 8);
          buf[index++] = (uint8_t)(msg.id >> 16);
          buf[index++] = (uint8_t)(msg.id >> 24);
          buf[index++] = (uint8_t)(msg.length);
          for (int i = 0; i < msg.length; i++)
            buf[index++] = msg.data[i];
          if (m_cfg.ptr_comm->sendPacket(PKT_TYPE_CAN, CMD_CAN_DATA, OK, buf, index) != ERROR_SUCCESS)
            LOG_PRINT("sendPacket fail !");
        }
      }

      if (m_isCanOpen == true && m_cfg.usb_IsOpen() == false)
      {
        m_isCanOpen = false;
        LOG_PRINT("[  ] canCmdClose()");
      }
      return true;
    }

    /**
     * @brief 
     * Check the mode switching switch event and usb connection status.
     */
    inline void checkPortEvent()
    {
      // Mode Button Process
      //
      if (m_cfg.prt_btns[AP_OBJ::BTN_S2].GetPressedEvent())
      {
        ModeUsbMode_t pre_mode = m_usbMode;
        m_usbMode = (ModeUsbMode_t)(((int)(m_usbMode) + 1) % ((int)(ModeUsbMode_t::MODE_USB_MAX)));
        m_cfg.eeprom_WriteByte(HW_EEPROM_MODE, (uint8_t)m_usbMode);
        LOG_PRINT("usb mode changed [%s] >> [%s] ", usb_mode_str[pre_mode], usb_mode_str[m_usbMode]);
        // evt.code = EVENT_MODE_CHANGE;
        // evt.data = (uint32_t)usb_mode;
        // event::Put(&evt);
      }

      // USB Connect Process
      //
      bool is_open;
      is_open = m_cfg.usb_IsOpen();
      if (m_isUsbOpen != is_open)
      {
        m_isUsbOpen = is_open;
        LOG_PRINT("usb port[%s] ", (is_open ? "Open" : "Close"));
        // evt.code = EVENT_USB_OPEN;
        // evt.data = (uint32_t)is_usb_open;
        // event::Put(&evt);
      }

      // USB Type Process
      //
      switch (m_usbMode)
      {
      case MODE_USB_TO_CAN:
        m_usbType = TYPE_USB_PACKET;
        break;

      case MODE_USB_TO_RS485:
        if (m_cfg.uart_GetBaud(HW_UART_CH_USB) == 600)
          m_usbType = TYPE_USB_PACKET;
        else
          m_usbType = TYPE_USB_UART;
        break;

      case MODE_USB_TO_CLI:
        if (m_cfg.usb_IsOpen() == true)
        {
          if (m_cfg.uart_GetBaud(HW_UART_CH_USB) == 115200)
            m_usbType = TYPE_USB_UART;
          else
            m_usbType = TYPE_USB_PACKET;
        }
        else
        {
          m_usbType = TYPE_USB_PACKET;
        }
        break;

      default:
        break;
      }
      // end of switch

      if (m_usbMode != m_preUsbMode)
        LOG_PRINT("[  ] Mode : %s", usb_mode_str[m_usbMode]);
      if (m_usbType != m_preUsbType)
        LOG_PRINT("[  ] Type : %s", usb_type_str[m_usbType]);
      m_preUsbMode = m_usbMode;
      m_preUsbType = m_usbType;
    }
    /**
     * @brief
     * state-machine, non-block code
     */
    inline void doRunStep()
    {
    }

    inline bool getTxUpdate(void)
    {
      bool ret;
      ret = m_isTxUpdate;
      m_isTxUpdate = false;
      return ret;
    }

    inline bool getRxUpdate(void)
    {
      bool ret;
      ret = m_isRxUpdate;
      m_isRxUpdate = false;
      return ret;
    }

    inline bool Process(cmi::uart_cmd::packet_st &packet)
    {
      bool ret = true;
      LOG_PRINT("cmd[%d]", packet.cmd);

      switch (packet.cmd)
      {
      case CMD_RS485_OPEN:
        ret = rs485CmdOpen(packet);
        break;
      case CMD_RS485_CLOSE:
        ret = rs485CmdClose(packet);
        break;
      case CMD_RS485_DATA:
        ret = rs485CmdData(packet);
        break;
      case CMD_CAN_OPEN:
        ret = canCmdOpen(packet);
        break;
      case CMD_CAN_CLOSE:
        ret = canCmdClose(packet);
        break;
      case CMD_CAN_DATA:
        ret = canCmdData(packet);
        break;
      case CMD_CAN_ERR_INFO:
        break;
      case CMD_CAN_SET_FILTER:
        ret = canCmdSetFilter(packet);
        break;
      case CMD_CAN_GET_FILTER:
        ret = canCmdGetFilter(packet);
        break;
      default:
        ret = false;
        break;
      }
      // end of switch
      return ret;
    }

    /**
     * @brief
     * Interrupt service routein function that is called 1ms.
     * Make sure to create a non-block
     */
    inline void ISR()
    {      
      switch (m_usbMode)
      {
      case MODE_USB_TO_CAN:
        m_cfg.ptr_leds[AP_OBJ::LED_COMM_CAN].On();
        m_cfg.ptr_leds[AP_OBJ::LED_COMM_485].Off();
        break;

      case MODE_USB_TO_RS485:
        m_cfg.ptr_leds[AP_OBJ::LED_COMM_CAN].Off();
        m_cfg.ptr_leds[AP_OBJ::LED_COMM_485].On();
        break;

      case MODE_USB_TO_CLI:
        m_cfg.ptr_leds[AP_OBJ::LED_COMM_CAN].Off();
        m_cfg.ptr_leds[AP_OBJ::LED_COMM_485].Off();
        break;
      default:
        break;
      }
    }
  };

}
// end of namespace cmi

#endif /* AP__INC_CN_COMM_HPP_ */
