#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>

namespace core
{

class Logger
{
public:
    enum class Level
    {
        Debug,
        Info,
        Warning,
        Error
    };

    static Logger& instance();

    void setLogFile(const std::string& path);
    void log(Level level, const std::string& message);

    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(Level level);
    std::string getTimestamp();

    std::mutex mutex_;
    std::ofstream fileStream_;
};

}

#define LOG_DEBUG(msg)   core::Logger::instance().debug(msg)
#define LOG_INFO(msg)    core::Logger::instance().info(msg)
#define LOG_WARNING(msg) core::Logger::instance().warning(msg)
#define LOG_ERROR(msg)   core::Logger::instance().error(msg)
