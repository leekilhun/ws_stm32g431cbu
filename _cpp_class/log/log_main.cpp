#include "mcc_log.hpp"

/*

The log class is used globally
Static members are initialized in the code main code page, 
and the necessary macros are defined and used globally.

Inheriting a log class from a class is provided separately.

*/

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




void test_log()
{
    ap_log.LogInfo("log info: [%d] timer test start '%s'.\n",5577, "filename");
    ap_log.PutLog(mcc_log::lvl_info, 001, (__FILE__), __LOGFUNC__, __LINE__,  "test %d", 100);
}

int main()
{
    test_log();

    uint32_t pre_ms = tim::millis();
    int cnt = 0;
    ap_log.Log_Print(__FILE__, __LOGFUNC__, __LINE__, "log print %d \n", 200);
    while (true)
    {

        if (tim::millis() - pre_ms > 500)
        {
            ++cnt;

            std::cerr << "\033[34m" << tim::NowStr() << "\t" << pre_ms << "\033[0m " << std::endl;
            pre_ms = tim::millis();
        }

        if (cnt > 10)
            break;

        // mcc::tim::delay(10);
    }
    if (ap_log.AvailableLog(mcc_log::lvl_info))
    {
        LOG_PRINT("available log \n");
        mcc_log::dat_st log_data = ap_log.GetLog();

        std::cout << log_data.file << std::endl;
        std::cout << log_data.func << std::endl;
        std::cout << log_data.message << std::endl;
        ap_log.writeToFile("test.log", "log file", true);
        std::cout << "finished!" << std::endl;
    }
    return 0;
}