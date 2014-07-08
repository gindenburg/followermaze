/* This file declears the BaseException class
 */
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

namespace followermaze
{

/* BaseException is a base class of followermaze exception hierarchy.
 * All exceptions thrown by followermaze classes should be inherited
 * from it. It's an exception which identifies itself and encapsulate
 * an integer error code. It also provides base for error codes. The
 * subclasses should base their specific error codes on it.
 */
class BaseException : public std::exception
{
public:
    enum
    {
        ErrGeneric = 10000, // Class is used in an inconsistent way.
    };
public:
    BaseException(int err = ErrGeneric) : m_err(err) {}
    int getErr() const { return m_err; }
    virtual const char* what() const throw() { return "followermaze::BaseException#"; }
protected:
    int m_err;
};

}

#endif // EXCEPTION_H
