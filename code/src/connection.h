/*
 * This file describes Connection interface.
 * Connection wraps an OS specific TCP/IP network I/O resource represented
 * by a Handle. It provides a way to exchange information through this
 * resource synchronously or asynchronously (defined at construction time).
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include "exception.h"

using namespace std;

namespace followermaze
{

typedef int Handle;
static const int INVALID_HANDLE = -1;

class Connection
{
public:
    class Exception : public BaseException
    {
    public:
        enum
        {
            ErrClientDisconnect = BaseException::ErrGeneric + 1 // Client closed the connection.
        };

    public:
        Exception(int err = BaseException::ErrGeneric) : BaseException(err) {}
        virtual const char* what() const throw() { return "Connection::Exception"; }
    };

public:
    // Creates server connection which listens on the port.
    // If async creates non-blocking connection.
    // Will throw on initialization error.
    Connection(int portno, bool async = false);
    // Creates invalid connection. Used by accept().
    Connection();
    virtual ~Connection();

private:
    // Make non-copyable.
    Connection(const Connection&);
    Connection& operator=(const Connection&);

public:
    // Will throw if not listening. Otherwise accepts connection, creates a new
    // instance and returns it. If async will return a non-blocking connection.
    // If blocking will block until a client connects.
    // If non-blocking will throw if attempt is made to accept connection while
    // no client is queueing.
    virtual Connection* accept(bool async = false);

    // If connected returns received message.
    // If blocking will block until there is data.
    // If non-blocking will throw if attempt to receive is made with no data waiting.
    virtual string receive();

    // Sends the message.
    // If blocking will block until data has been transferred to the transport layer.
    // If non-blocking will throw if attempt to send data is made while transport
    // layer is not accepting writes.
    virtual void send(const string &message);

    // Getter for the handle.
    Handle getHandle() const;

private:
    Handle m_handle;  // I/O handle.
};

} // namespace followermaze

#endif // CONNECTION_H
