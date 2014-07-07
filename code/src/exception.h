#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

namespace followermaze
{

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
    virtual const char* what() const throw() { return "followermaze::BaseException"; }
protected:
    int m_err;
};

}

#endif // EXCEPTION_H
