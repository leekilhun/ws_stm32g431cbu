/*
 * ap.cpp
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */




#include "ap.hpp"



static enLed leds[AP_OBJ::LED_MAX];
static enBtn btns[AP_OBJ::BTN_MAX];


cmi::uart_cmd cmd_comm;
cmi::task_boot task_bootloader;

void apInit(void)
{


  {
    enLed::cfg_t cfg;
    cfg.gpio_port = LED_GPIO_Port;
    cfg.gpio_pin = LED_Pin;

    if(leds[AP_OBJ::LED_STATUS].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("status_led Init Failed!");

    cfg.gpio_port = LED_485_GPIO_Port;
    cfg.gpio_pin = LED_485_Pin;
    if(leds[AP_OBJ::LED_COMM_485].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_485_led Init Failed!");

    cfg.gpio_port = LED_CAN_GPIO_Port;
    cfg.gpio_pin = LED_CAN_Pin;
    if(leds[AP_OBJ::LED_COMM_CAN].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_can_led Init Failed!");

    cfg.gpio_port = LED_RX_GPIO_Port;
    cfg.gpio_pin = LED_RX_Pin;
    if(leds[AP_OBJ::LED_COMM_RX].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_rx_led Init Failed!");

    cfg.gpio_port = LED_TX_GPIO_Port;
    cfg.gpio_pin = LED_TX_Pin;
    if(leds[AP_OBJ::LED_COMM_TX].Init(cfg) != ERROR_SUCCESS)
      LOG_PRINT("comm_tx_led Init Failed!");
  }

  {

    enBtn::cfg_t cfg;
    cfg.gpio_port = BOOT_BTN_GPIO_Port;
    cfg.gpio_pin =BOOT_BTN_Pin;
    if(btns[AP_OBJ::BTN_BOOT].Init(cfg)!= ERROR_SUCCESS)
      LOG_PRINT("btns[AP_OBJ::BTN_BOOT] Init Failed!");

    cfg.gpio_port = BTN_S1_GPIO_Port;
    cfg.gpio_pin =BTN_S1_Pin;
    if(btns[AP_OBJ::BTN_S1].Init(cfg)!= ERROR_SUCCESS)
      LOG_PRINT("btns[AP_OBJ::BTN_S1] Init Failed!");

    cfg.gpio_port = BTN_S2_GPIO_Port;
    cfg.gpio_pin =BTN_S2_Pin;
    if(btns[AP_OBJ::BTN_S2].Init(cfg)!= ERROR_SUCCESS)
      LOG_PRINT("btns[AP_OBJ::BTN_S2] Init Failed!");

  }


  {
    cmi::uart_cmd::cfg_t cfg;

    cfg.ch = HW_UART_CH_USB;
    cfg.baud = 1000000;
    cmd_comm.Init(cfg);
  }

  {
    cmi::task_boot::cfg_t cfg;
    //cfg.boot_btn = &btns[AP_OBJ::BTN_BOOT];
    cfg.ptr_comm = &cmd_comm;
    cfg.ptr_rx_led = &leds[AP_OBJ::LED_COMM_RX];
    cfg.ptr_tx_led = &leds[AP_OBJ::LED_COMM_TX];    
    task_bootloader.Init(cfg);
  }


}


void apMain(void)
{

  uint32_t pre_time;
  pre_time = millis();

  while(1)
  {
    if (millis() - pre_time >= 500)
    {
      leds[AP_OBJ::LED_STATUS].Toggle();
    }
  }
}
