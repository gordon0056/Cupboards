#include "core/Logger.hpp"

namespace core
{

Logger& Logger::instance()
{
    static Logger inst;
    return inst;
}

Logger::~Logger()
{
    if (fileStream_.is_open())
    {
        fileStream_.close();
    }
}

void Logger::setLogFile(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (fileStream_.is_open())
    {
        fileStream_.close();
    }
    fileStream_.open(path, std::ios::app);
}

void Logger::log(Level level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string levelStr = levelToString(level);
    std::string timestamp = getTimestamp();
    std::string fullMessage =
        "[" + timestamp + "] [" + levelStr + "] " + message;

    if (level == Level::Error)
    {
        std::cerr << fullMessage << std::endl;
    }
    else
    {
        std::cout << fullMessage << std::endl;
    }

    if (fileStream_.is_open())
    {
        fileStream_ << fullMessage << std::endl;
    }
}

void Logger::debug(const std::string& msg) { log(Level::Debug, msg); }
void Logger::info(const std::string& msg) { log(Level::Info, msg); }
void Logger::warning(const std::string& msg) { log(Level::Warning, msg); }
void Logger::error(const std::string& msg) { log(Level::Error, msg); }

std::string Logger::levelToString(Level level)
{
    switch (level)
    {
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO";
        case Level::Warning: return "WARNING";
        case Level::Error:   return "ERROR";
        default:             return "UNKNOWN";
    }
}

std::string Logger::getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&time));
    return std::string(buf);
}

}
