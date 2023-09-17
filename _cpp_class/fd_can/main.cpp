#include "../def.hpp"
#include "../cli/mcc_cli.hpp"
#include "../log/mcc_log.hpp"

/****************************************************
 *	cli 
 ****************************************************/
TinyC::Cli ap_cli;

// initializer static memvers
bool TinyC::Cli::trd_life = true;
HANDLE TinyC::Cli::hConsole = nullptr;
std::string TinyC::Cli::c_input{};
std::map<std::string, std::function<int(int, char**)>> TinyC::Cli::callbacks{};

///////////////////////////////////////////////////////////////////


/****************************************************
 *	log  global
 ****************************************************/
mcc_log ap_log;

// initializer static memvers
uint32_t mcc_log::trace_prc_pre_tick = 0;    
std::mutex mcc_log::log_mutex {};
TinyC::Que<mcc_log::dat_st> mcc_log::log_table[mcc_log::lvl_max] = { TinyC::Que<mcc_log::dat_st>(4096), TinyC::Que<mcc_log::dat_st>(4096), TinyC::Que<mcc_log::dat_st>(4096)};

#define SYSLOGFILE_INFO   "./Log/system.log"
#define SYSLOGFILE_ERR    "./Log/sysError.log"

#define __LOGFUNC__				__FUNCTION__
#define SYSLOG_INFO(obj,fmt, ...)		ap_log.PutLog(mcc_log::lvl_info,obj, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)
#define LOG_PRINT(fmt, ...)		ap_log.WriteLog(mcc_log::lvl_info, 0,SYSLOGFILE_INFO, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)
#define ERR_PRINT(fmt, ...)		ap_log.WriteLog(mcc_log::lvl_err, 0,SYSLOGFILE_ERR, (__FILE__), __LOGFUNC__, __LINE__,  fmt, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////



#include "../hw/serial_comm.hpp"
#include "../json/mcc_json.hpp"
#include "fd_can.hpp"




int main()
{

    /* "cmd_str" "func_name" .... */
    ap_cli.registerCallback("help", ap_cli.cli_show_list);

    // Create a serial communication object
    std::shared_ptr<serial_comm> uart = std::make_shared<serial_comm>();

    serial_comm::cfg_t cfg{};
    cfg.port_name = "COM11";
    cfg.baudrate = 115200;
     if (uart->Init(cfg) != ERROR_SUCCESS)
    {
        ERR_PRINT("uart->Init(cfg) Fail!");
        return 0;
    }
    else
    {
        LOG_PRINT("uart->Init(cfg) Success!");
    }


    // Create a peeler object
     std::shared_ptr<msc::fd_can> fdcan_module = std::make_shared<msc::fd_can>();
    {
        msc::fd_can::cfg_t cfg;
        cfg.ptr_uart_comm = uart.get();
        if (fdcan_module->Init(cfg) != ERROR_SUCCESS)
        {
            ERR_PRINT("fdcan_module->Init(cfg) Fail!");
            return 0;
        }
        else
        {
            LOG_PRINT("fdcan_module->Init(cfg) Success!");
        }
    }
    // Initialize the peeler object

    // Run the peeler object
    ap_cli.registerCallback("uart", std::bind(&serial_comm::testCB_func, uart.get(), std::placeholders::_1, std::placeholders::_2));

    ap_cli.registerCallback("fdcan_module", std::bind(&msc::fd_can::testCB_func, fdcan_module.get(), std::placeholders::_1, std::placeholders::_2));

    // cli loop
    std::cout << "start " << std::endl;
    while (1)
    {
        ap_cli.cli_main();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "terminate " << std::endl;

    return 0;
}