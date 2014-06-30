#ifndef LOGGER_H
#define LOGGER_H

#include <string>

using namespace std;

namespace followermaze
{

class Logger
{
public:
    void message(const string &msg);

    void message(const string &msg, int number);

    void error(const string &msg, int err);

    static Logger& getInstance();

protected:
    static Logger m_logger;
};

} // namespace followermaze

#endif // LOGGER_H
