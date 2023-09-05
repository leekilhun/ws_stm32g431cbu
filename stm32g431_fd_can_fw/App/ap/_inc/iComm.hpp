#pragma once
#ifndef _IF_ICOMM_HPP
#define _IF_ICOMM_HPP


#include "ap_def.h"

struct IComm
{
  virtual errno_t Open() = 0;
  virtual bool Recovery() = 0;
  virtual bool IsConnected() const = 0;
  virtual bool IsAvailableComm() const = 0;
  virtual void AttCallbackFunc(void *obj, evt_cb cb) = 0;
  virtual bool ReceiveProcess() = 0;
  virtual errno_t SendCmd(uint8_t *ptr_data, uint32_t size) = 0;
  virtual errno_t SendCmdRxResp(uint8_t *ptr_data, uint32_t size, uint32_t timeout) = 0;
};

#endif