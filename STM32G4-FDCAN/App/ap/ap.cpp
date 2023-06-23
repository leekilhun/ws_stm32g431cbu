/*
 * ap.cpp
 *
 *  Created on: Apr 23, 2023
 *      Author: gns2l
 */

#include "ap.hpp"


#ifdef _USE_HW_CLI
static void cliApp(cli_args_t *args);
#endif


/****************************************************
  0. mcu data and register
 ****************************************************/



/****************************************************
  1. ap instances
 ****************************************************/

void  apInit(void)
{

#ifdef _USE_HW_CLI
  if(cliOpen(HW_UART_LCD, 115200))
    LOG_PRINT("cliOpen Success! ch[%d], baud[%d]",HW_UART_LCD, 115200);
  else
    LOG_PRINT("cliOpen Fail! ch[%d], baud[%d] \n",HW_UART_LCD, 115200);
#endif




#ifdef _USE_HW_CLI
cliAdd("app", cliApp);
#endif

}




void  apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();

  while (1)
  {

    if (millis()-pre_time >= 1000)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      //logPrintf(">>hdma_usart1_rx.Instance->CNDTR %d \n",hdma_usart1_rx.Instance->CNDTR);
    }



#ifdef _USE_HW_CLI
    cliMain();
#endif

#if 0

    if (uartAvailable(_DEF_UART1) > 0)
    {
      uint8_t rx_data;

      rx_data = uartRead(_DEF_UART1);
      uartPrintf(_DEF_UART1, "rx data : 0x%02X (%c)\n", rx_data, rx_data);
    }
#endif

  }

}




#ifdef _USE_HW_CLI
void cliApp(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1)
  {
    if (args->isStr(0, "info") == true)
    {
      cliPrintf("motor X [%d] \n", &moons_motors[0]);
      cliPrintf("motor Y [%d] \n", &moons_motors[1]);
      cliPrintf("motor R [%d] \n", &moons_motors[2]);
      ret = true;

    }
    else if (args->isStr(0, "io_in") == true)
    {
      cliPrintf("mcu io in [%d] \n", mcu_io.m_in.data);
      ret = true;
    }
  }
  else if (args->argc == 2)
  {
    
    int result = 0;
    if (args->isStr(0, "mt_on") == true)
    {
      uint8_t axis_id = constrain((uint8_t)args->getData(1), 1, 4);
      result = moons_motors[(axis_id-1)].MotorOnOff(true);
      if (result == ERROR_SUCCESS)
      {
        cliPrintf("motor Axis[%d] On \n", axis_id);
        ret = true;
      }
    }
    else if (args->isStr(0, "mt_off") == true)
    {
      uint8_t axis_id = constrain((uint8_t)args->getData(1), 1, 4);
      result = moons_motors[(axis_id-1)].MotorOnOff(false);
      if (result == ERROR_SUCCESS)
      {
        cliPrintf("motor Axis[%d] Off \n", axis_id);
        ret = true;
      }
    }
    else if (args->isStr(0, "mt_state") == true)
    {
      uint8_t axis_id = constrain((uint8_t)args->getData(1), 1, 4);
      motors.GetMotorState((AP_OBJ::MOTOR)(axis_id-1));

      cliPrintf("motor GetMotorState[%d] \n", (axis_id-1));
      ret = true;
    }
    else if (args->isStr(0, "mt_show") == true)
    {
      uint8_t axis_id = constrain((uint8_t)args->getData(1), 1, 4);
      MOTOR::enMotor_moons::moons_data_t* ptr_data =  &moons_motors[(axis_id-1)].m_motorData;
      cliPrintf( "motor id[%d] \n", (axis_id));
      cliPrintf( "motor al_code [%d] \n",ptr_data->al_code.al_status );
      cliPrintf( "motor drv_status [%d] \n",ptr_data->drv_status.sc_status);
      cliPrintf( "motor driver_board_inputs [%d] \n",ptr_data->al_code.al_status);
      cliPrintf( "motor driver_board_inputs [%d] \n",ptr_data->driver_board_inputs);
      cliPrintf( "motor encoder_position [%d] \n",ptr_data->encoder_position);
      cliPrintf( "motor immediate_abs_position [%d] \n",ptr_data->immediate_abs_position);
      cliPrintf( "motor abs_position_command [%d] \n",ptr_data->abs_position_command);
      ret = true;
    }
    else if (args->isStr(0, "line_run") == true)
    {
      uint16_t line_idx = args->getData(1);
      if (task_data.ReadData())
      {
        tasks.RunLineTask(line_idx);
        for (auto&[i, dat]  : task_data.task_dat)
        {
          cliPrintf("line : idx[%d]\tnext_line[%d]\tline_type[%d]\tpos_idx[%d]\ten_out[%d]\tex_out\n"
              ,  dat.idx, dat.next_line, dat.line_type, dat.pos_data_idx, dat.entry_setout, dat.exit_setout );

        }
       /* for (auto& elm : task_data.task_dat)
        {
          cliPrintf("line : idx[%d]\tnext_line[%d]\tline_type[%d]\tpos_idx[%d]\ten_out[%d]\tex_out\n"
              ,  elm.line_data.idx, elm.line_data.next_line, elm.line_data.line_type, elm.line_data.pos_data_idx, elm.line_data.entry_setout, elm.line_data.exit_setout );

        }*/

        cliPrintf("line_run [%d] \n", line_idx);
        ret = true;
      }
      else
      {
        cliPrintf("line_run ReadData fail \n");
      }
         }
    else if (args->isStr(0, "io_on") == true)
    {
      uint8_t idx = (uint8_t)args->getData(1);
         
      if (mcu_io.OutputOn(idx + AP_DEF_START_OUT_ADDR)== ERROR_SUCCESS)
      {
        cliPrintf("io_on [%d] success! \n", idx + AP_DEF_START_OUT_ADDR);
        ret = true;
      }
    }
    else if (args->isStr(0, "io_off") == true)
    {
      uint8_t idx = (uint8_t)args->getData(1);
      if (mcu_io.OutputOff(idx + AP_DEF_START_OUT_ADDR) == ERROR_SUCCESS)
      {
        cliPrintf("io_off [%d] success! \n", idx + AP_DEF_START_OUT_ADDR);
        ret = true;
      }
    }
  }
  else if (args->argc == 3)
  {
    uint8_t lamp_idx = constrain((uint8_t)args->getData(1),(uint8_t)enOp::lamp_e::LAMP_START,(uint8_t)enOp::lamp_e::LAMP_RESET);
    bool on_off = args->getData(2)==0?false:true;
    if (args->isStr(0, "oplamp") == true)
    {
      op_panel.LampOnOff((enOp::lamp_e)lamp_idx, on_off);
      {
        cliPrintf("LampOnOff index[%d][%d]\n",lamp_idx, on_off);
        ret = true;
      }
    }

  }
  else if (args->argc == 4)
  {
    uint8_t id = (uint8_t)args->getData(1);
    uint32_t speed = (uint32_t)args->getData(2);
    int step_pulse = (uint32_t)args->getData(3);
    if (args->isStr(0, "run") == true)
    {
      //motor.SetStep(step_pulse, speed);
      //motor.RunAndWait();
      {
        cliPrintf("ID[%d] run step[%d] speed[%d] : OK\n",id, step_pulse, speed);
        ret = true;
      }
    }
    if (args->isStr(0, "rel") == true)
    {
      //motor.SetRel(step_pulse, speed);
      //motor.RunAndWait();
      {
        cliPrintf("ID[%d] rel step[%d] speed[%d] : OK\n",id, step_pulse, speed);
        ret = true;
      }
    }


  }


  if (ret == false)
  {
    cliPrintf( "app info \n");
    cliPrintf( "app io_in \n");
    cliPrintf( "app io_on [0:7] \n");
    cliPrintf( "app io_off [0:7] \n");
    cliPrintf( "app mt_on [axis] \n");
    cliPrintf( "app mt_off [axis] \n");
    cliPrintf( "app mt_state [axis] \n");
    cliPrintf( "app mt_show [axis] \n");
    cliPrintf( "app line_run [idx] \n");
    cliPrintf( "app oplamp [0:2] [0:1 0 off, 1 on] \n");
    cliPrintf( "app run [axis][100:0 speed][step] \n");
    cliPrintf( "app rel [axis][100:0 speed][step] \n");
  }


}
#endif


