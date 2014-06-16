/*
 * This file describes Connection interface.
 * Connection wraps an OS specific TCP/IP network I/O resource represented
 * by a Handle. It provides an interface to exchange information through this
 * resource in blocking manner.
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
public:
    class Exception : public exception
    {
    public:
        enum {
            ErrConnectionState = 10000,
            ErrClientDisconnect
        };
    public:
        Exception(int err) : m_err(err) {}
        virtual const char* what() const throw() { return "Connection::Exception"; }
        int getErr() const { return m_err; }
    protected:
        int m_err;
    };

public:
    // Creates server connection which listens on the port
    Connection(int portno);
    virtual ~Connection();

protected:
    // Creates a client connection to return by accept()
    Connection();

public:
    // If listening, accepts connection, creates a new one and returns it. Will block until a client connects.
    virtual Connection* accept();

    // If connected, return received message. Will block until there is data.
    virtual string receive();

    // Sends the message.
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
