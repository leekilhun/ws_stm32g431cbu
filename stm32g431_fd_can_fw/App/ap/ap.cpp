/*
 * ap.cpp
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */

#include "ap.hpp"

static enLed leds[AP_OBJ::LED_MAX];
static enBtn btns[AP_OBJ::BTN_MAX];

/**
 * @brief
 * It must be written in non-block code as a function called in 1ms time.
 * @param arg
 */
static void apISR_1ms(void *arg);
static void apISR_10ms(void *arg);

static void updateLED();

#ifdef _USE_HW_CLI
static void cliApp(cli_args_t *args);
#endif

cmi::uart_cmd usb_comm;
cmi::task_boot task_bootloader;
cmi::enComm comm_cmd;
cmi::cnComm comm_mode;

void apInit(void)
{

  /**
   * @brief
   * register the function to the 1ms interrupt sw-timer.
   */
  {
    swtimer_handle_t timer_ch;
    timer_ch = swtimerGetHandle();
    if (timer_ch >= 0)
    {
      swtimerSet(timer_ch, 1, LOOP_TIME, apISR_1ms, NULL);
      swtimerStart(timer_ch);
    }
    else
    {
      LOG_PRINT("[NG] 1ms swtimerGetHandle()");
    }

    timer_ch = swtimerGetHandle();
    if (timer_ch >= 0)
    {
      swtimerSet(timer_ch, 10, LOOP_TIME, apISR_10ms, NULL);
      swtimerStart(timer_ch);
    }
    else
    {
      LOG_PRINT("[NG] 10ms swtimerGetHandle()");
    }
  }

  {
    enLed::cfg_t cfg;
    cfg.gpio_port = LED_GPIO_Port;
    cfg.gpio_pin = LED_Pin;

    if (leds[AP_OBJ::LED_STATUS].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("status_led Init Failed!");

    cfg.gpio_port = LED_485_GPIO_Port;
    cfg.gpio_pin = LED_485_Pin;
    if (leds[AP_OBJ::LED_COMM_485].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_485_led Init Failed!");

    cfg.gpio_port = LED_CAN_GPIO_Port;
    cfg.gpio_pin = LED_CAN_Pin;
    if (leds[AP_OBJ::LED_COMM_CAN].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_can_led Init Failed!");

    cfg.gpio_port = LED_RX_GPIO_Port;
    cfg.gpio_pin = LED_RX_Pin;
    if (leds[AP_OBJ::LED_COMM_RX].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_rx_led Init Failed!");

    cfg.gpio_port = LED_TX_GPIO_Port;
    cfg.gpio_pin = LED_TX_Pin;
    if (leds[AP_OBJ::LED_COMM_TX].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_tx_led Init Failed!");
  }

  {
    enBtn::cfg_t cfg;
    cfg.gpio_port = BOOT_BTN_GPIO_Port;
    cfg.gpio_pin = BOOT_BTN_Pin;
    cfg.repeat_time_detect = 60; // ISR count
    cfg.repeat_time = 200;
    cfg.repeat_time_delay = 250;
    memset(&cfg.name_str[0], 0, sizeof(cfg.name_str));
    strlcpy(&cfg.name_str[0], "0_BOOT", sizeof(cfg.name_str));
    if (btns[AP_OBJ::BTN_BOOT].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("btns[AP_OBJ::BTN_BOOT] Init Failed!");

    cfg.gpio_port = BTN_S1_GPIO_Port;
    cfg.gpio_pin = BTN_S1_Pin;
    cfg.repeat_time_detect = 60; // ISR count
    cfg.repeat_time = 200;
    cfg.repeat_time_delay = 250;
    memset(&cfg.name_str[0], 0, sizeof(cfg.name_str));
    strlcpy(&cfg.name_str[0], "1_S1", sizeof(cfg.name_str));
    if (btns[AP_OBJ::BTN_S1].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("btns[AP_OBJ::BTN_S1] Init Failed!");

    cfg.gpio_port = BTN_S2_GPIO_Port;
    cfg.gpio_pin = BTN_S2_Pin;
    cfg.repeat_time_detect = 60; // ISR count
    cfg.repeat_time = 200;
    cfg.repeat_time_delay = 250;
    memset(&cfg.name_str[0], 0, sizeof(cfg.name_str));
    strlcpy(&cfg.name_str[0], "2_S2", sizeof(cfg.name_str));
    if (btns[AP_OBJ::BTN_S2].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("btns[AP_OBJ::BTN_S2] Init Failed!");
  }

#ifdef _USE_HW_CLI
  if (cliOpen(_DEF_UART1, 115200))
    LOG_PRINT("cliOpen Success! ch[%d], baud[%d]", _DEF_UART1, 115200);
  else
    LOG_PRINT("cliOpen Fail! ch[%d], baud[%d] \n", _DEF_UART1, 115200);
#endif

  {
    cmi::uart_cmd::cfg_t cfg;

    cfg.ch = HW_UART_CH_USB;
    cfg.baud = 1000000;
    usb_comm.Init(cfg);
  }

  {
    cmi::task_boot::cfg_t cfg;
    // cfg.boot_btn = &btns[AP_OBJ::BTN_BOOT];
    cfg.ptr_comm = &usb_comm;
    cfg.ptr_rx_led = &leds[AP_OBJ::LED_COMM_RX];
    cfg.ptr_tx_led = &leds[AP_OBJ::LED_COMM_TX];
    task_bootloader.Init(cfg);
  }

  {
    cmi::enComm::cfg_t cfg;
    cfg.ptr_boot = &task_bootloader;
    cfg.ptr_comm = &usb_comm;
    comm_cmd.Init(cfg);
  }

  {
    cmi::cnComm::cfg_t cfg;

    cfg.ptr_leds = leds;
    cfg.prt_btns = btns;

    cfg.ptr_comm = &comm_cmd;

    cfg.can_ch = _DEF_CAN1;
    cfg.can_filter_type = CAN_ID_MASK;
    cfg.can_filter_id_type = CAN_STD;
    cfg.can_filter_id1 = 0x00000000;
    cfg.can_filter_id2 = 0x00000000;
    cfg.can_open = canOpen;
    cfg.can_IsOpen = canIsOpen;
    cfg.can_close = canClose;
    cfg.can_update = canUpdate;
    cfg.can_MsgAvailable = canMsgAvailable;
    cfg.can_MsgRead = canMsgRead;
    cfg.can_MsgInit = canMsgInit;
    cfg.can_MsgWrite = canMsgWrite;
    cfg.can_SetFilterType = canSetFilterType;
    cfg.can_ConfigFilter = canConfigFilter;

    cfg.rs485_ch = HW_UART_CH_RS485;
    cfg.uart_open = uartOpen;
    cfg.uart_available = uartAvailable;
    cfg.uart_read = uartRead;
    cfg.uart_write = uartWrite;
    cfg.uart_GetBaud = uartGetBaud;
    cfg.usb_IsOpen = usbIsOpen;
    cfg.eeprom_ReadByte = eepromReadByte;
    cfg.eeprom_WriteByte = eepromWriteByte;

    comm_mode.Init(cfg);
  }

#ifdef _USE_HW_CLI
  cliAdd("app", cliApp);
#endif
}

void apMain(void)
{

   uint32_t pre_time;
   pre_time = millis();

  /**
   * @brief  main loop
   *
   */
  while (1)
  {
    {
      comm_mode.ThreadJob();
    }

#ifdef _USE_HW_CLI
    {
      uint8_t cli_ch;
      switch (comm_mode.m_usbMode)
      {
      case cmi::cnComm::MODE_USB_TO_CLI:
        if (comm_mode.m_usbType == cmi::cnComm::TYPE_USB_UART)
        {
          /* check port type*/
          if (usbIsOpen() && usbGetType() == USB_CON_CLI)
            cli_ch = HW_UART_CH_USB;
          else
            cli_ch = HW_UART_CH_DEBUG;

          /* check port channel*/
          if (cli_ch != cliGetPort())
            cliOpen(cli_ch, 0);
          cliMain();
        }
        else
        {
          if (cliGetPort() != HW_UART_CH_DEBUG)
            cliOpen(HW_UART_CH_DEBUG, 0);
          cliMain();
        }
        break;
      case cmi::cnComm::MODE_USB_TO_RS485: __attribute__((fallthrough));
      case cmi::cnComm::MODE_USB_TO_CAN:
        if (cliGetPort() != HW_UART_CH_DEBUG)
          cliOpen(HW_UART_CH_DEBUG, 0);
        cliMain();
        break;

      default:
        break;
      }
    }

#endif

    /**
     * @brief
     * Log Out
     */
    if (millis() - pre_time >= 1000)
    {
      pre_time = millis();
      //logPrintf("Usb Mode [%d] Type[%d], usb baud[%d]\n", comm_mode.m_usbMode, comm_mode.m_usbType, uartGetBaud(HW_UART_CH_USB));
    }
  }
}

void updateLED()
{
  /**
   * @brief
   * status led
   */
  {
    static uint32_t pre_time_update_led;

    if (millis() - pre_time_update_led >= 500)
    {
      pre_time_update_led = millis();
      leds[AP_OBJ::LED_STATUS].Toggle();
    }
  }
  // end

  /**
   * @brief
   * communication tx/rx led
   */
  {
    enum
    {
      LED_IDLE,
      LED_ON,
      LED_OFF,
    };
    static uint8_t state_rx_update_led = LED_IDLE;
    static uint32_t pre_time_rx_update_led;
    static uint8_t state_tx_update_led = LED_IDLE;
    static uint32_t pre_time_tx_update_led;
    bool is_tx_update;
    bool is_rx_update;

    // tx led
    switch (state_tx_update_led)
    {
    case LED_IDLE:
      is_tx_update = comm_mode.getTxUpdate();
      if (is_tx_update)
      {
        is_tx_update = false;
        state_tx_update_led = LED_ON;
        leds[AP_OBJ::LED_COMM_TX].On();
        pre_time_tx_update_led = millis();
      }
      break;
    case LED_ON:
      if (millis() - pre_time_tx_update_led >= 50)
      {
        state_tx_update_led = LED_OFF;
        leds[AP_OBJ::LED_COMM_TX].Off();
        pre_time_tx_update_led = millis();
      }
      break;
    case LED_OFF:
      if (millis() - pre_time_tx_update_led >= 50)
      {
        state_tx_update_led = LED_IDLE;
        pre_time_tx_update_led = millis();
      }
      break;
    default:
      break;
    }
    // end of switch

    // rx led
    switch (state_rx_update_led)
    {
    case LED_IDLE:
      is_rx_update = comm_mode.getRxUpdate();
      if (is_rx_update)
      {
        is_rx_update = false;
        state_rx_update_led = LED_ON;
        leds[AP_OBJ::LED_COMM_RX].On();
        pre_time_rx_update_led = millis();
      }
      break;
    case LED_ON:
      if (millis() - pre_time_rx_update_led >= 50)
      {
        state_rx_update_led = LED_OFF;
        leds[AP_OBJ::LED_COMM_RX].Off();
        pre_time_rx_update_led = millis();
      }
      break;
    case LED_OFF:
      if (millis() - pre_time_rx_update_led >= 50)
      {
        state_rx_update_led = LED_IDLE;
        pre_time_rx_update_led = millis();
      }
      break;
    default:
      break;
    }
    // end of switch
  }
  // end
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _USE_HW_CLI

void cliApp(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1)
  {
    if (args->isStr(0, "info") == true)
    {
      cliPrintf("info \n");
      ret = true;
    }
    else if (args->isStr(0, "btn_info") == true)
    {
      cliPrintf("btn_info \n");
      for (int i = 0; i < AP_OBJ::BTN_MAX; i++)
      {
        cliPrintf("%-12s pin %d\n", btns[i].GetName(), btns[i].m_cfg.gpio_pin);
      }
      ret = true;
    }
    else if (args->isStr(0, "btn_show") == true)
    {
      while (cliKeepLoop())
      {
        for (int i = 0; i < AP_OBJ::BTN_MAX; i++)
        {
          cliPrintf("%-12s pin[%d] : %d\n", btns[i].GetName(), btns[i].m_cfg.gpio_pin, btns[i].IsPressed());
        }
        delay(50);
        cliPrintf("\x1B[%dA", AP_OBJ::BTN_MAX);
        // cliPrintf("\r");
      }
      ret = true;
    }
  }
  else if (args->argc == 2)
  {
  }
  else if (args->argc == 3)
  {
    if (args->isStr(0, "led") == true)
    {
      uint8_t ch;
      bool on_off;

      ch = (uint8_t)args->getData(1);
      on_off = (bool)args->getData(2);

      if (ch < AP_OBJ::LED_MAX)
      {
        if (on_off)
          leds[ch].On();
        else
          leds[ch].Off();
        cliPrintf("led ch[%d] %s \n", ch, on_off ? "On" : "Off");
        ret = true;
      }
    }
  }
  else if (args->argc == 4)
  {
  }

  if (ret == false)
  {
    cliPrintf("app info \n");
    cliPrintf("app btn_info\n");
    cliPrintf("app btn_show\n");
    cliPrintf("app led [ch] [0:1 0 off, 1 on] \n");
  }
}
#endif

void apISR_1ms(void *arg)
{
}

void apISR_10ms(void *arg)
{
  for (auto &elm : btns)
    elm.ISR();

  comm_mode.ISR();

  updateLED();
}
