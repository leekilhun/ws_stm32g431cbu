/*
 * uart.c
 *
 *  Created on: Aug 26, 2023
 *      Author: gns2.lee
 */




#include "uart.h"

#ifdef _USE_HW_UART

#include "qbuffer.h"
#include "cli.h"
#include "cdc.h"

#define UART_MAX_CH         HW_UART_MAX_CH

#define UART_BUF_LENGTH        256



typedef struct
{
  bool is_open;
  uint32_t baud;

  uint8_t  *rx_buf;
  qbuffer_t qbuffer;
  UART_HandleTypeDef *p_huart;
  DMA_HandleTypeDef  *p_hdma_rx;

  uint32_t rx_cnt;
  uint32_t tx_cnt;
} uart_tbl_t;


typedef struct
{
  const char         *p_msg;
  USART_TypeDef      *p_uart;
  UART_HandleTypeDef *p_huart;
  DMA_HandleTypeDef  *p_hdma_rx;
  DMA_HandleTypeDef  *p_hdma_tx;
  bool                is_rs485;
} uart_hw_t;


#ifdef _USE_HW_CLI
static void cliUart(cli_args_t *args);
#endif


static bool is_init = false;
static uart_tbl_t uart_tbl[UART_MAX_CH];

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#ifdef _USE_HW_UART_1_DMA
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
static uint8_t  uart1_tx_data[UART_BUF_LENGTH];
#endif
#ifdef _USE_HW_UART_2_DMA
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
static uint8_t  uart2_tx_data[UART_BUF_LENGTH];
#endif
#ifdef _USE_HW_UART_3_DMA
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;
static uint8_t  uart3_tx_data[UART_BUF_LENGTH];
#endif


const static uart_hw_t uart_hw_tbl[UART_MAX_CH] =
  {
    {"USART1 DEBUG  ", USART1, &huart1, &hdma_usart1_rx,  &hdma_usart1_tx, false},
    {"USART2 RS485  ", USART2, &huart2, &hdma_usart2_rx,  &hdma_usart2_tx, true},
    {"USART3 EXT    ", USART3, &huart3, &hdma_usart3_rx,  &hdma_usart3_tx, true},
    {"USB    USB    ", NULL, NULL, NULL, NULL, true},
  };



static uint8_t  rx_buf_1[UART_BUF_LENGTH];
static uint8_t  rx_buf_2[UART_BUF_LENGTH];
static uint8_t  rx_buf_3[UART_BUF_LENGTH];


/////////////////////////////////////////////////////////////




bool uartInit(void)
{

  for (int i=0; i<UART_MAX_CH; i++)
    {
      uart_tbl[i].is_open = false;
      uart_tbl[i].baud = 57600;
      uart_tbl[i].rx_cnt = 0;
      uart_tbl[i].tx_cnt = 0;
    }

    uart_tbl[_DEF_UART1].rx_buf = rx_buf_1;
    uart_tbl[_DEF_UART2].rx_buf = rx_buf_2;
    uart_tbl[_DEF_UART3].rx_buf = rx_buf_3;

    is_init = true;

  #ifdef _USE_HW_CLI
    cliAdd("uart", cliUart);
  #endif
    return true;
}


bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;
  HAL_StatusTypeDef ret_hal;


  if (ch >= UART_MAX_CH) return false;

  if (uart_tbl[ch].is_open == true && uart_tbl[ch].baud == baud)
  {
    return true;
  }


  switch(ch)
  {
    case _DEF_UART1:
    case _DEF_UART2:
    case _DEF_UART3:
      uart_tbl[ch].baud      = baud;

      uart_tbl[ch].p_huart   = uart_hw_tbl[ch].p_huart;
      uart_tbl[ch].p_hdma_rx = uart_hw_tbl[ch].p_hdma_rx;
      uart_tbl[ch].p_huart->Instance = uart_hw_tbl[ch].p_uart;

      uart_tbl[ch].p_huart->Init.BaudRate       = baud;
      uart_tbl[ch].p_huart->Init.WordLength     = UART_WORDLENGTH_8B;
      uart_tbl[ch].p_huart->Init.StopBits       = UART_STOPBITS_1;
      uart_tbl[ch].p_huart->Init.Parity         = UART_PARITY_NONE;
      uart_tbl[ch].p_huart->Init.Mode           = UART_MODE_TX_RX;
      uart_tbl[ch].p_huart->Init.HwFlowCtl      = UART_HWCONTROL_NONE;
      uart_tbl[ch].p_huart->Init.OverSampling   = UART_OVERSAMPLING_16;
      uart_tbl[ch].p_huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
      uart_tbl[ch].p_huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
      uart_tbl[ch].p_huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


      qbufferCreate(&uart_tbl[ch].qbuffer, &uart_tbl[ch].rx_buf[0], UART_BUF_LENGTH);


      __HAL_RCC_DMAMUX1_CLK_ENABLE();
      __HAL_RCC_DMA1_CLK_ENABLE();


      HAL_UART_DeInit(uart_tbl[ch].p_huart);

      if (uart_hw_tbl[ch].is_rs485 == true)
      {
        ret_hal = HAL_RS485Ex_Init(uart_tbl[ch].p_huart, UART_DE_POLARITY_HIGH, 0, 0);
      }
      else
      {
        ret_hal = HAL_UART_Init(uart_tbl[ch].p_huart);
      }

      if (ret_hal == HAL_OK)
      {
        ret = true;
        uart_tbl[ch].is_open = true;
#ifdef _USE_HW_UART_1_DMA
        if(HAL_UART_Receive_DMA(uart_tbl[ch].p_huart, (uint8_t *)&uart_tbl[ch].rx_buf[0], UART_BUF_LENGTH) != HAL_OK)
        {
          ret = false;
        }

        uart_tbl[ch].qbuffer.in  = uart_tbl[ch].qbuffer.len - ((DMA_Channel_TypeDef *)uart_tbl[ch].p_huart->hdmarx->Instance)->CNDTR;
        uart_tbl[ch].qbuffer.out = uart_tbl[ch].qbuffer.in;

#else
      if (HAL_UART_Receive_IT(uart_tbl[ch].p_huar, (uint8_t *)&uart_tbl[ch].rx_buf, 1) != HAL_OK)
      {
        ret = false;
      }
#endif
      }
      break;

    case _DEF_UART4:
      uart_tbl[ch].baud    = baud;
      uart_tbl[ch].is_open = true;
      ret = true;
      break;

    default:
      break;
  }
  // end of switch

  return ret;
}

bool uartIsOpen(uint8_t ch)
{
  bool ret = false;

  switch(ch)
  {
    case _DEF_UART1:
    case _DEF_UART2:
    case _DEF_UART3:
      ret = uart_tbl[ch].is_open = true;
      break;

    case _DEF_UART4:
      ret = cdcIsConnect();
      break;
  }
  return ret;
}

bool uartClose(uint8_t ch)
{
  if (ch >= UART_MAX_CH) return false;

  uart_tbl[ch].is_open = false;

  return true;
}



uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  HAL_StatusTypeDef hal_ret = HAL_OK;
  switch(ch)
  {
    case _DEF_UART1:
    {
#ifdef _USE_HW_UART_1_DMA
      uint32_t pre_time;
      pre_time = millis();
      while(uart_tbl[ch].p_huart->gState != HAL_UART_STATE_READY)
      {
        if (millis()-pre_time >= 100)
        {
          break;
        }
      }
      if (uart_tbl[ch].p_huart->gState == HAL_UART_STATE_READY)
      {
        memset(&uart1_tx_data[0],0,UART_BUF_LENGTH);
        memcpy(&uart1_tx_data[0],p_data,length);
        hal_ret = HAL_UART_Transmit_DMA(uart_tbl[ch].p_huart, &uart1_tx_data[0], length);
      }
#else
      hal_ret = HAL_UART_Transmit(uart_tbl[ch].p_huart, p_data, length, 100);
#endif
      if (hal_ret == HAL_OK)
        ret = length;
    }
    break;
    case _DEF_UART2:
    {
#ifdef _USE_HW_UART_2_DMA
      uint32_t pre_time;
      pre_time = millis();
      while(uart_tbl[ch].p_huart->gState != HAL_UART_STATE_READY)
      {
        if (millis()-pre_time >= 100)
        {
          break;
        }
      }
      if (uart_tbl[ch].p_huart->gState == HAL_UART_STATE_READY)
      {
        memset(&uart2_tx_data[0],0,UART_BUF_LENGTH);
        memcpy(&uart2_tx_data[0],p_data,length);
        hal_ret = HAL_UART_Transmit_DMA(uart_tbl[ch].p_huart, &uart2_tx_data[0], length);
      }
#else
      hal_ret = HAL_UART_Transmit(uart_tbl[ch].p_huart, p_data, length, 100);
#endif
      if (hal_ret == HAL_OK)
        ret = length;
    }
    break;
    case _DEF_UART3:
    {
#ifdef _USE_HW_UART_3_DMA
      uint32_t pre_time;
      pre_time = millis();
      while(uart_tbl[ch].p_huart->gState != HAL_UART_STATE_READY)
      {
        if (millis()-pre_time >= 100)
        {
          break;
        }
      }
      if (uart_tbl[ch].p_huart->gState == HAL_UART_STATE_READY)
      {
        memset(&uart3_tx_data[0],0,UART_BUF_LENGTH);
        memcpy(&uart3_tx_data[0],p_data,length);
        hal_ret = HAL_UART_Transmit_DMA(uart_tbl[ch].p_huart, &uart3_tx_data[0], length);
      }
#else
      hal_ret = HAL_UART_Transmit(uart_tbl[ch].p_huart, p_data, length, 100);
#endif
      if (hal_ret == HAL_OK)
        ret = length;
    }
    break;
    case _DEF_UART4:
      ret = cdcWrite(p_data, length);
      break;
    default:
      break;
  }

  uart_tbl[ch].tx_cnt += ret;
  return ret;
}



uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;
  switch(ch)
  {
    case _DEF_UART1:
#ifdef _USE_HW_UART_1_DMA
      uart_tbl[ch].qbuffer.in = (uart_tbl[ch].qbuffer.len - hdma_usart1_rx.Instance->CNDTR);
#endif
      ret = qbufferAvailable(&uart_tbl[ch].qbuffer);
      break;

    case _DEF_UART2:
#ifdef _USE_HW_UART_2_DMA
      uart_tbl[ch].qbuffer.in = (uart_tbl[ch].qbuffer.len - hdma_usart1_rx.Instance->CNDTR);
#endif
      ret = qbufferAvailable(&uart_tbl[ch].qbuffer);
      break;

    case _DEF_UART3:
#ifdef _USE_HW_UART_3_DMA
      uart_tbl[ch].qbuffer.in = (uart_tbl[ch].qbuffer.len - hdma_usart1_rx.Instance->CNDTR);
#endif
      ret = qbufferAvailable(&uart_tbl[ch].qbuffer);
      break;

    case _DEF_UART4:
      ret = cdcAvailable();
      break;

    default:
      break;
  }
  return ret;
}


bool uartFlush(uint8_t ch)
{
  uint32_t pre_time;


   pre_time = millis();
   while(uartAvailable(ch))
   {
     if (millis()-pre_time >= 10)
     {
       break;
     }
     uartRead(ch);
   }

   return true;
}


uint8_t  uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
    case _DEF_UART2:
    case _DEF_UART3:
      qbufferRead(&uart_tbl[ch].qbuffer, &ret, 1);
      break;

    case _DEF_UART4:
      ret = cdcRead();
      break;
  }
  uart_tbl[ch].rx_cnt++;

  return ret;
}


uint32_t uartVPrintf(uint8_t ch, const char *fmt, va_list arg)
{
  uint32_t ret = 0;
  char print_buf[256];


  int len;
  len = vsnprintf(print_buf, 256, fmt, arg);

  if (len > 0)
  {
    ret = uartWrite(ch, (uint8_t *)print_buf, len);
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, const char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}



uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  if (ch >= UART_MAX_CH) return 0;

  if (ch == HW_UART_CH_USB)
    ret = cdcGetBaud();
  else
    ret = uart_tbl[ch].baud;

  return ret;
}

uint32_t uartGetRxCnt(uint8_t ch)
{
  if (ch >= UART_MAX_CH) return 0;

  return uart_tbl[ch].rx_cnt;
}

uint32_t uartGetTxCnt(uint8_t ch)
{
  if (ch >= UART_MAX_CH) return 0;

  return uart_tbl[ch].tx_cnt;
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if (huart->Instance == USART1)
  {
#ifdef _USE_HW_UART_1_DMA
#else
    qbufferWrite(&qbuffer[_DEF_UART1], &uart_tbl[_DEF_UART1].rx_data, 1);
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart_tbl[_DEF_UART1].rx_data, 1);
#endif
  }
  else if (huart->Instance == USART2)
  {
#ifdef _USE_HW_UART_2_DMA
#else
    qbufferWrite(&qbuffer[_DEF_UART2], &uart_tbl[_DEF_UART2].rx_data, 1);
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&uart_tbl[_DEF_UART2].rx_data, 1);
#endif
  }
  else if (huart->Instance == USART3)
  {
#ifdef _USE_HW_UART_3_DMA
#else
    qbufferWrite(&qbuffer[_DEF_UART3], &uart_tbl[_DEF_UART3].rx_data, 1);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&uart_tbl[_DEF_UART3].rx_data, 1);
#endif

  }
  /*else if (huart->Instance == USART4)
  {
    qbufferWrite(&qbuffer[_DEF_UART4], &uart_tbl[_DEF_UART4].rx_data, 1);
    HAL_UART_Receive_IT(&huart4, (uint8_t *)&uart_tbl[_DEF_UART4].rx_data, 1);
  }*/
}



#ifdef _USE_HW_CLI
void cliUart(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    for (int i=0; i<UART_MAX_CH; i++)
    {
      cliPrintf("_DEF_UART%d : %s, %d bps\n", i+1, uart_hw_tbl[i].p_msg, uartGetBaud(i));
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "test"))
  {
    uint8_t uart_ch;

    uart_ch = constrain(args->getData(1), 1, UART_MAX_CH) - 1;

    if (uart_ch != cliGetPort())
    {
      uint8_t rx_data;

      while(1)
      {
        if (uartAvailable(uart_ch) > 0)
        {
          rx_data = uartRead(uart_ch);
          cliPrintf("<- _DEF_UART%d RX : 0x%X\n", uart_ch + 1, rx_data);
        }

        if (cliAvailable() > 0)
        {
          rx_data = cliRead();
          if (rx_data == 'q')
          {
            break;
          }
          else
          {
            uartWrite(uart_ch, &rx_data, 1);
            cliPrintf("-> _DEF_UART%d TX : 0x%X\n", uart_ch + 1, rx_data);
          }
        }
      }
    }
    else
    {
      cliPrintf("This is cliPort\n");
    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("uart info\n");
    cliPrintf("uart test ch[1~%d]\n", HW_UART_MAX_CH);
  }
}
#endif

#endif
