#include <iostream>
#include <iterator>
#include <locale>
#include "logger.h"

namespace followermaze
{

class TimeFormat
{
public:
    TimeFormat(const string &format)
        : m_format(format)
    {
    }

    friend ostream& operator <<(ostream &, const TimeFormat&);

private:
    string m_format;
};

ostream& operator <<(ostream& os, const TimeFormat &format)
{
    ostream::sentry s(os);

    if (s)
    {
        time_t t = time(0);
        const tm *localt = localtime(&t);
        ostreambuf_iterator<char> out(os);

        use_facet< time_put<char> >(os.getloc()).put(
                    out, os, os.fill(), localt, &format.m_format[0],
                    &format.m_format[0] + format.m_format.size());
    }

    os.width(0);

    return os;
}

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
    cout << TimeFormat("%c") << " " << prefix << msg.c_str() << msg1.c_str();
    if (err != INT_MAX)
        cout << err;
    cout << endl << flush;
}

Logger& Logger::getInstance()
{
    return m_logger;
}

} // namespace followermaze
