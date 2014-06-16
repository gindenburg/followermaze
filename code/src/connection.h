/*
 * This file describes Connection interface.
 * Connection wraps an OS specific TCP/IP network I/O resource represented
 * by a Handle. It provides a way to exchange information through this
 * resource synchronously or asynchronously (defined at construction time).
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <exception>

using namespace std;

// Let's define Handle as int. This will work nicely on POSIX platforms.
// If we wanted this to be cross-platfom we would have to re-define it.
typedef int Handle;

class Connection
{
    friend class ConnectionManager;

public:
    class Exception : public exception
    {
    public:
        enum {
            ErrAppLogic = 10000, // Connection used in an inconsistent way.
            ErrClientDisconnect  // Client closed the connection.
        };
    public:
        Exception(int err) : m_err(err) {}
        virtual const char* what() const throw() { return "Connection::Exception"; }
        int getErr() const { return m_err; }
    protected:
        int m_err;
    };

public:
    // Creates server connection which listens on the port.
    // If async creates non-blocking connection.
    // Will throw on initialization error.
    Connection(int portno, bool async = false);
    virtual ~Connection();

protected:
    // Creates a client connection to return by accept()
    Connection();

public:
    // Will throw if not listening. Otherwise accepts connection, creates a new
    // instance and returns it. If async will return a non-blocking connection.
    // If blocking will block until a client connects.
    // If non-blocking will throw if attempt is made to accept connection while
    // no client is queueing.
    virtual Connection* accept(bool async = false);

    // If connected returns received message.
    // If blocking will block until there is data.
    // If non-blocking will throw if attempt to receive is made without data waiting.
    virtual string receive();

    // Sends the message.
    // If blocking will block until data has been transferred to the transport layer.
    // If non-blocking will throw if attempt to send data is made while transport
    // layer is not accepting writes.
    virtual void send(const string &message);

    // Connection is alive if it is listening for connection or is connected to a client.
    virtual bool isAlive() const;

private:
    // Invalidate connection, cleanup, throw.
    void handleError(int err);

private:
    Handle m_handle;  // I/O handle.
    bool m_listening; // Listening for connection.
};

#endif // CONNECTION_H
