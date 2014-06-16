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

Connection::Connection(int portno)
{
    // Create socket (we assume TCP/IP with IPv4 for simplicity)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        throw Exception(errno);
    }

    /* Initialize socket structure */
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portno);

    // Now bind the host address using bind() call.
    if (0 != bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)))
    {
        handleError(errno);
    }

    if (0 != listen(sockfd, 5))
    {
        handleError(errno);
    }

    // We've created a socket and are listening for a connection.
    m_handle = sockfd;
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

Connection* Connection::accept()
{
    if (!m_listening)
    {
        // Application logic error - we're not listening for clients.
        throw Exception(Exception::ErrConnectionState);
    }

    Connection* clientConnection = new Connection();

    struct sockaddr clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int sockfd = ::accept(m_handle, (struct sockaddr *)&clientAddr, &clientAddrLen);
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
        throw Exception(Exception::ErrConnectionState);
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
        throw Exception(Exception::ErrConnectionState);
    }

    if (message.empty())
    {
        return;
    }

    if (write(m_handle, message.c_str(), message.length()) <= 0)
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
