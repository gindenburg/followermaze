#include <iostream>
#include "logger.h"

namespace followermaze
{

Logger Logger::m_logger;

void Logger::message(const string &msg)
{
    cout << "MSG: " << msg.c_str() << endl << flush;
}

void Logger::message(const string &msg, int number)
{
    cout << "MSG: " << msg.c_str() << number << "." << endl << flush;
}

void Logger::error(const string &msg, int err)
{
    cout << "ERR: " << msg.c_str() << "(" << err << ")." << endl << flush;
}

Logger& Logger::getInstance()
{
    return m_logger;
}

} // namespace followermaze
