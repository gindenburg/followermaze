#include <iostream>
#include <ctime>
#include "logger.h"

namespace followermaze
{

Logger Logger::m_logger;

void Logger::setLogLevel(LogLevel level)
{
    m_level = level;
}

void Logger::debug(const string &msg)
{
    if (m_level == LvlDebug)
    {
        message("[DEBUG] ", msg, "");
    }
}

void Logger::debug(const string &msg, const string &msg1)
{
    if (m_level == LvlDebug)
    {
        message("[DEBUG] ", msg, msg1);
    }
}

void Logger::debug(const string &msg, int err)
{
    if (m_level == LvlDebug)
    {
        message("[DEBUG] ", msg, "", err);
    }
}

void Logger::debug(const string &msg, const string &msg1, int err)
{
    if (m_level == LvlDebug)
    {
        message("[DEBUG] ", msg, msg1, err);
    }
}

void Logger::info(const string &msg)
{
    if (m_level >= LvlInfo)
    {
        message("[INFO] ", msg, "");
    }
}

void Logger::info(const string &msg, const string &msg1)
{
    if (m_level >= LvlInfo)
    {
        message("[INFO] ", msg, msg1);
    }
}

void Logger::info(const string &msg, int err)
{
    if (m_level >= LvlInfo)
    {
        message("[INFO] ", msg, "", err);
    }
}

void Logger::info(const string &msg, const string &msg1, int err)
{
    if (m_level >= LvlInfo)
    {
        message("[INFO] ", msg, msg1, err);
    }
}

void Logger::error(const string &msg)
{
    message("[ERROR] ", msg, "");
}

void Logger::error(const string &msg, const string &msg1)
{
    message("[ERROR] ", msg, msg1);
}

void Logger::error(const string &msg, int err)
{
    message("[ERROR] ", msg, "", err);
}

void Logger::error(const string &msg, const string &msg1, int err)
{
    message("[ERROR] ", msg, msg1, err);
}

void Logger::message(const char *prefix, const string &msg, const string &msg1, int err)
{
    time_t now = time(0);
    struct tm *gmtm = gmtime(&now);
    char timestr[32];

    if (strftime(timestr, 32, "%a %b %d %T %Y ", gmtm) > 0)
    {
        cout << timestr;
    }

    cout << prefix << msg.c_str() << msg1.c_str();

    if (err != INT_MAX)
        cout << err;
    cout << endl << flush;
}

Logger& Logger::getInstance()
{
    return m_logger;
}

} // namespace followermaze
