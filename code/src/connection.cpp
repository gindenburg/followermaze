/*
 * This file contains implementation of Connection based on Berkley sockets API.
 * This is a very simple implementation :-)
 */

#include "connection.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

Connection::Connection(int portno, bool async)
{
    // Create socket (we assume TCP/IP with IPv4 for simplicity)
    m_handle = socket(AF_INET, SOCK_STREAM, 0);
    if (m_handle < 0)
    {
        throw Exception(errno);
    }

    // Initialize socket structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portno);

    // Now bind the host address using bind() call.
    if (0 != bind(m_handle, (struct sockaddr *) &serverAddr, sizeof(serverAddr)))
    {
        handleError(errno);
    }

    if (async)
    {
        // Set socket to non-blocking
        int flags = 0;

        if ((flags = fcntl(m_handle, F_GETFL, 0)) < 0)
        {
            handleError(errno);
        }

        if (fcntl(m_handle, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            handleError(errno);
        }
    }

    if (0 != listen(m_handle, 5))
    {
        handleError(errno);
    }

    // We've created a socket and are listening for a connection.
    m_listening = true;
}

Connection::Connection()
{
    m_handle = -1;
    m_listening = false;
}

Connection::~Connection()
{
    if (m_handle >= 0)
    {
        close(m_handle);
    }
}

Connection* Connection::accept(bool async)
{
    if (!m_listening)
    {
        // Application logic error - we're not listening for clients.
        throw Exception(Exception::ErrAppLogic);
    }

    Connection* clientConnection = new Connection();

    struct sockaddr clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int flags = async ? SOCK_NONBLOCK : 0;
    int sockfd = ::accept4(m_handle, (struct sockaddr *)&clientAddr, &clientAddrLen, flags);
    if (sockfd < 0)
    {
        delete clientConnection;
        throw Exception(errno);
    }

    clientConnection->m_handle = sockfd;

    return clientConnection;
}

string Connection::receive()
{
    if (m_listening || m_handle < 0)
    {
        // Application logic error - we're not connected to a client.
        throw Exception(Exception::ErrAppLogic);
    }

    ssize_t bytesRecieved;
    char incomingDataBuffer[4096]; // TODO: make it server configuration parameter?
    bytesRecieved = recv(m_handle, incomingDataBuffer, 4096, 0);

    if (bytesRecieved <= 0)
    {
        // Client closed the connection or some error has happened.
        handleError(bytesRecieved < 0 ? errno : Exception::ErrClientDisconnect);
    }

    incomingDataBuffer[bytesRecieved] = 0; // zterminate
    return string(incomingDataBuffer);
}

void Connection::send(const string &message)
{
    if (m_listening || m_handle < 0)
    {
        // Application logic error - we're not connected to a client.
        throw Exception(Exception::ErrAppLogic);
    }

    if (message.empty())
    {
        return;
    }

    int flags = MSG_NOSIGNAL;
    if (::send(m_handle, message.c_str(), message.length(), flags) < 0)
    {
        // Client closed the connection or some error has happened.
        // Report actual error or 0 in case socket has been closed by the client.
        handleError(errno == EPIPE ? Exception::ErrClientDisconnect : errno);
    }
}

bool Connection::isAlive() const
{
    return m_listening || m_handle >= 0;
}

void Connection::handleError(int err)
{
    close(m_handle);
    m_handle = -1;

    // Report actual error or 0 in case socket has been closed by the client.
    throw Exception(err);
}
