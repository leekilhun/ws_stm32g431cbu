#pragma once
#ifndef _COMMON_MCC_LOG_HPP
#define _COMMON_MCC_LOG_HPP

#include "../def.hpp"

/*

lee kil hun
gns.lee2@samsung.com
2023.04

*/

constexpr int MCC_LOG_VLIST_BUF_MAX = 1024;
constexpr int MCC_LOG_BUFF_LIST_MAX = 4096;
constexpr const char* mcc_log_def_log_path = "./Log/";


class mcc_log
{

public:
    enum level
    {
        lvl_info,
        lvl_err,
        lvl_warning,
        lvl_max
    };
    struct dat_st
    {
        std::string level{};
        std::string date{};
        std::string time{};
        std::string message{};
        std::string file{};
        std::string func{};
        uint32_t line_no = 0;
        uint32_t obj_id = 0;

        void SetLevel(std::string_view str) { level = str; }
        void SetDate(std::string_view str) { date = str; }
        void SetTime(std::string_view str) { time = str; }
        void SetMsg(std::string_view str) { message = str; }
        void SetFile(std::string_view str) { file = str; }
        void SetFunc(std::string_view str) { func = str; }
        void SetLineNo(uint32_t ln) { line_no = ln; }
        void SetObjId(uint32_t id) { obj_id = id; }

        std::string GetLogMessage() const
        {
            std::string str = tim::NowStr() 
            +  "[(" + file + "),(" + func + "),(" + std::to_string(line_no) + ")] " 
            + level + " : " + message;          
             
            return str;
        }
    };

    static uint32_t trace_prc_pre_tick;    
    static std::mutex log_mutex;
    static TinyC::Que<mcc_log::dat_st> log_table[mcc_log::lvl_max];
    mcc_log() {

    };
    ~mcc_log() = default;

    template <typename = int>
    void LogPrint(const char *str, ...)
    {
        if (str)
        {
            va_list args;
            va_start(args, str);

            vfprintf(stdout, str, args);

            va_end(args);
        }
    }

    template <typename = int>
    void LogInfo(const char *str, ...)
    {
        if (str)
        {
            va_list args;
            va_start(args, str);

            vfprintf(stdout, str, args);

            va_end(args);
        }
    }

    inline static void Log_Print(const char *file_name, const char *func_name, uint32_t line_no, const char *str, ...)
    {
        std::lock_guard<std::mutex> safe_lock(log_mutex);
        if (str)
        {
            va_list args;
            va_start(args, str);
            std::string filename = std::filesystem::path(file_name).filename().string();

            std::string add_time = "[(" + std::string(filename) + "),(" + func_name + "),(" + std::to_string(line_no) + ")] " + tim::TimeStr() + str;

            vfprintf(stdout, (const char *)add_time.c_str(), args);
            va_end(args);
        }
    }

    inline static void trace_prc(const char *file_name, const char *func_name, uint32_t line_no, const char *str, ...)
    {
        std::lock_guard<std::mutex> safe_lock(log_mutex);
        if (str)
        {
            va_list args;
            va_start(args, str);
            std::string filename = std::filesystem::path(file_name).filename().string();

            trace_prc_pre_tick = tim::millis() - trace_prc_pre_tick;
            std::string add_time = "[(" + std::string(filename) + "),(" + func_name + "),(" + std::to_string(line_no) + ")] " + tim::TimeStr() + "[" + std::to_string(trace_prc_pre_tick) + "] " + str;

            vfprintf(stdout, (const char *)add_time.c_str(), args);
            trace_prc_pre_tick = tim::millis();
            va_end(args);
        }
    }

    inline static void trace_prc(const char *str, ...)
    {
        std::lock_guard<std::mutex> safe_lock(log_mutex);
        if (str)
        {
            va_list args;
            va_start(args, str);
            trace_prc_pre_tick = tim::millis() - trace_prc_pre_tick;
            std::string add_time = tim::TimeStr() + "[" + std::to_string(trace_prc_pre_tick) + "] " + str;
            vfprintf(stdout, (const char *)add_time.c_str(), args);
            trace_prc_pre_tick = tim::millis();
            va_end(args);
        }
    }

    inline static uint32_t PutLog(level loglevel, const int obj, const char *file, const char *func, const int line, const char *fmt, ...)
    {
        std::lock_guard<std::mutex> safe_lock(log_mutex);
        dat_st data{};
        if (fmt)
        {
            char buf[MCC_LOG_VLIST_BUF_MAX];
            va_list args;
            va_start(args, fmt);
            std::string filename = std::filesystem::path(file).filename().string();

            std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] " + fmt;
            data.SetDate(tim::NowStr());
            data.SetFile(filename);
            data.SetFunc(func);
            data.SetLineNo(line);

            std::vsnprintf(buf, sizeof(buf), fmt, args);
            data.SetMsg(std::string(buf));

            va_end(args);
        }
        else
        {
            return 0;
        }

        switch (loglevel)
        {
        case lvl_err:
            data.SetLevel("[ERR]");
            break;
        case lvl_warning:
            data.SetLevel("[WAR]");
            break;
        default: // info
            data.SetLevel("[INF]");
            break;
        }
        log_table[loglevel].Put(data);

        return (uint32_t)log_table[loglevel].Available();
    }

    inline static dat_st GetLog(level loglevel = lvl_info)
    {
        dat_st data{};
        log_table[loglevel].Get(&data);
        return data;
    }

    inline static dat_st PopLog(uint32_t addr, level loglevel = lvl_info)
    {
        dat_st data{};
        log_table[loglevel].Pop(addr, &data);
        return data;
    }

    inline static uint32_t AvailableLog(level loglevel = lvl_info)
    {
        return (uint32_t)log_table[loglevel].Available();
    }

    inline static std::string getLogFileName(const std::string &file)
    {
        // Get file name
        std::string filename = std::filesystem::path(file).filename().string();
        // ".cpp" delete
        size_t found = filename.find(".cpp"); // ".cpp" 
        if (found != std::string::npos)
        {                             
            filename.erase(found, 4); 
        }

        // Create log file name
        std::stringstream ss;
        ss << mcc_log_def_log_path << tim::DateStr().c_str() << "_" << filename ;//<< ".log";

        // Check if the log directory exists
        if (!std::filesystem::is_directory(mcc_log_def_log_path))
        {
            std::filesystem::create_directory(mcc_log_def_log_path);
        }

        // Check if the log file exist
        if (std::filesystem::exists(ss.str()+".log"))
        {
            file::FileCopy(std::string(ss.str()+"_old.log").c_str(),
                std::string((ss.str() + ".log")).c_str());          
        }

        ss << ".log";
        return ss.str();
    }

    static void writeToFile(const std::string &file, const std::string &message, bool prepend = false)
    {
        std::ofstream ofs(file, std::ios::out | std::ios::app); // Keep existing file content and open in append mode
        if (ofs.good())
        {
            if (prepend)
            {
                ofs.seekp(0, std::ios::beg); // Go to the beginning of the file
                ofs << ofs.rdbuf();          // Append the rest of the file after the inserted content
                ofs << message << std::endl; // add to front
            }
            else
            {
                ofs << message << std::endl; // add to back
            }
        }
        ofs.close();
    }

    inline static bool WriteLog(
        level loglevel,
        const int obj,
        const char *file_info,
        const char *file,
        const char *func,
        const int line,
        const char *fmt, ...)
    {
        std::lock_guard<std::mutex> safe_lock(log_mutex);
        if (fmt)
        {
            dat_st data{};

            char buf[MCC_LOG_VLIST_BUF_MAX];
            va_list args;
            va_start(args, fmt);

            std::string filename = std::filesystem::path(file).filename().string();

            std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] " + fmt;
            
            data.SetDate(tim::NowStr());
            data.SetFile(filename);
            data.SetFunc(func);
            data.SetLineNo(line);

            std::vsnprintf(buf, sizeof(buf), fmt, args); // snprintf(buf, sizeof(buf), fmt, args);

            std::string message = std::string(buf);
            data.SetMsg(message);

            va_end(args);

            // Get log file name
            std::string log_file_name = getLogFileName(file);
            std::string ctg_str = loglevel == lvl_info ? "[INF]" : (loglevel == lvl_warning ? "[WAR]" : "[ERR]");
       
            data.SetLevel(ctg_str);

            // put que buffer
            log_table[loglevel].Put(data);

            // Write log message to file
            writeToFile(log_file_name, data.GetLogMessage());
        }
        return true;
    }
};
// end of mcc_log

namespace TinyC
{
    /* 클래스 객체 */
    class Log
    {
    public:
        enum level
        {
            lvl_info,
            lvl_err,
            lvl_warning,
            lvl_max
        };

    private:
        std::vector<std::string> m_logTable[lvl_max];
        std::mutex m_mutex;

    public:
        Log():m_logTable{},m_mutex{}{};
        ~Log() = default;

    private:
        inline void writeToFile(const std::string &file, const std::string &message, bool prepend = false)
        {
            std::ofstream ofs(file, std::ios::out | std::ios::app); // Keep existing file content and open in append mode
            if (ofs.good())
            {
                if (prepend)
                {
                    ofs.seekp(0, std::ios::beg); // Go to the beginning of the file
                    ofs << message;              //<< std::endl; // add to front
                }
                else
                {
                    ofs << message; //<< std::endl; // add to back
                }
            }
            ofs.close();
        }

        inline std::string getLogFileName(const std::string &file)
        {
            // Get file name
            std::string filename = std::filesystem::path(file).filename().string();
            // ".cpp" delete
            size_t found = filename.find(".cpp"); // ".cpp"
            if (found != std::string::npos)
            {
                filename.erase(found, 4);
            }

            // Create log file name
            std::stringstream ss;
            ss << mcc_log_def_log_path << tim::DateStr().c_str() << "_" << filename; //<< ".log";

            // Check if the log directory exists
            if (!std::filesystem::is_directory(mcc_log_def_log_path))
            {
                std::filesystem::create_directory(mcc_log_def_log_path);
            }

            // Check if the log file exist
            if (std::filesystem::exists(ss.str() + ".log"))
            {
                file::FileCopy(std::string((ss.str() + ".log")).c_str(), std::string(ss.str() + "_old.log").c_str());
            }

            ss << ".log";
            return ss.str();
        }

    public:
        inline bool WriteLog(
            level loglevel,
            const int obj,
            const char *file_info,
            const char *file,
            const char *func,
            const int line,
            const char *fmt, ...)
        {
            std::lock_guard<std::mutex> safe_lock(m_mutex);
            if (fmt)
            {
                mcc_log::dat_st data{};

                char buf[MCC_LOG_VLIST_BUF_MAX];
                va_list args;
                va_start(args, fmt);

                std::string filename = std::filesystem::path(file).filename().string();

                std::string add_time = "[(" + std::string(filename) + "),(" + func + "),(" + std::to_string(line) + ")] " + tim::NowStr() + "[ ] " + fmt;

                data.SetDate(tim::NowStr());
                data.SetFile(filename);
                data.SetFunc(func);
                data.SetLineNo(line);

                std::vsnprintf(buf, sizeof(buf), fmt, args); // snprintf(buf, sizeof(buf), fmt, args);

                std::string message = std::string(buf);
                data.SetMsg(message);

                va_end(args);

                // Get log file name
                std::string log_file_name = getLogFileName(file);
                std::string ctg_Str = loglevel == lvl_info ? "[INF]" : (loglevel == lvl_warning ? "[WAR]" : "[ERR]");
                // Write log message to file
                message = ctg_Str + message;
                m_logTable[loglevel].emplace_back(message);
                // Write log message to file
                writeToFile(log_file_name, data.GetLogMessage());
            }
            return true;
        }
    };
}
#endif
// end of _COMMON_MCC_LOG_HPP