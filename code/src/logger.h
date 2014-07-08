/* This class declears the Logger class.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <climits>

using namespace std;

namespace followermaze
{

/* Logger is a naive implementation of a logging facility which simply
 * writes into standard ouput stream.
 * In a real-world application a mature logging framework (e.g. log4cpp)
 * should be used.
 * Logger is a singletone accessed through a class method.
 */
class Logger
{
public:
    enum LogLevel
    {
        LvlError,
        LvlInfo,
        LvlDebug
    };

    void setLogLevel(LogLevel level);

    void debug(const string &msg);
    void debug(const string &msg, const string &msg1);
    void debug(const string &msg, int err);
    void debug(const string &msg, const string &msg1, int err);

    void info(const string &msg);
    void info(const string &msg, const string &msg1);
    void info(const string &msg, int err);
    void info(const string &msg, const string &msg1, int err);

    void error(const string &msg);
    void error(const string &msg, const string &msg1);
    void error(const string &msg, int err);
    void error(const string &msg, const string &msg1, int err);

    static Logger& getInstance();

protected:
    Logger() : m_level(LvlDebug)
    {
    }

protected:
    void message(const char *prefix, const string &msg, const string &msg1, int err = INT_MAX);

protected:
    LogLevel m_level;

    static Logger m_logger;
};

} // namespace followermaze

#endif // LOGGER_H
