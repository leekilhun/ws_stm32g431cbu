#pragma once
#ifndef _IF_ICOMM_HPP
#define _IF_ICOMM_HPP


#include "ap_def.h"

struct IComm
{
  virtual errno_t Open() = 0;
  virtual bool Recovery() = 0;
  virtual bool IsOpen() const = 0;
  virtual bool IsAvailableComm() const = 0;
  virtual void AttCallbackFunc(void *obj, evt_cb cb) = 0;
  virtual bool ReceiveProcess() = 0;
  virtual errno_t Send(uint8_t *ptr_data, uint32_t size) = 0;
  virtual errno_t SendRxResp(uint8_t *ptr_data, uint32_t size, uint32_t timeout = 100) = 0;
  //virtual errno_t Send(uint8_t type, uint16_t cmd, uint16_t err_code, uint8_t *ptr_data, uint32_t size) = 0;
  //virtual errno_t SendResp(uint16_t cmd, uint16_t err_code, uint8_t *ptr_data, uint32_t size, uint32_t timeout = 100) = 0;
  //virtual errno_t SendCmd(uint16_t cmd, uint8_t *ptr_data,  uint32_t size) = 0;
  //virtual errno_t SendCmdRxResp(uint16_t cmd, uint8_t *ptr_data,  uint32_t size, uint32_t timeout = 100) = 0;
};

#endif