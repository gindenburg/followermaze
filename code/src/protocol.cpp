#include <cstdlib>
#include <climits>
#include <sstream>
#include "protocol.h"
#include "reactor.h"
#include "engine.h"
#include "logger.h"

namespace followermaze
{

namespace protocol
{

/*----------------------------------------------------------------------------*/

EventSource::EventSource(auto_ptr<Connection> connection, Reactor &reactor, Engine &engine) :
    Client(connection, reactor),
    m_engine(engine)
{
    Logger::getInstance().info("EventSource connected.");
}

EventSource::~EventSource()
{
    Logger::getInstance().info("EventSource disconnected.");
}

void EventSource::handleClose(int hint)
{
    Logger::getInstance().info("EventSource closed.");
    m_engine.resetEventQueue();
    Client::handleClose(hint);
}

void EventSource::handleError(int hint)
{
    Logger::getInstance().error("EventSource error.");
    m_engine.resetEventQueue();
    Client::handleError(hint);
}

void EventSource::doHandleInput(int /*hint*/)
{
    m_buffer += m_connection->receive();
    m_engine.handleEvents(m_buffer);
}

/*----------------------------------------------------------------------------*/

UserClient::UserClient(auto_ptr<Connection> connection, Reactor &reactor, Engine &engine) :
    Client(connection, reactor),
    m_engine(engine),
    m_userId(Parser::INVALID_LONG),
    m_hint(-1)
{
    Logger::getInstance().info("UserClient connected.");
}

void UserClient::send(const string& message)
{
    m_messageOut += message;
    m_reactor.resetHandler(m_hint, Reactor::EvntWrite);
}

UserClient::~UserClient()
{
    Logger::getInstance().info("UserClient disconnected.");
}

void UserClient::doHandleInput(int hint)
{
    m_hint = hint;
    m_messageIn += m_connection->receive();
    m_userId = m_engine.registerUser(this, m_messageIn);
    if (m_userId != protocol::Parser::INVALID_LONG)
    {
        Logger::getInstance().info("User authenticated: ", m_userId);
        m_messageIn.clear();
    }
}

void UserClient::doHandleOutput(int hint)
{
    m_connection->send(m_messageOut);
    m_messageOut.clear();
    m_reactor.resetHandler(hint, Reactor::EvntRead);
}

void UserClient::handleClose(int hint)
{
    reset(hint);
    Client::handleClose(hint);
}

void UserClient::handleError(int hint)
{
    reset(hint);
    Client::handleError(hint);
}

void UserClient::reset(int /*hint*/)
{
    m_engine.unregisterUser(m_userId, this);
    m_userId = Parser::INVALID_LONG;
    m_messageOut.clear();
    m_messageIn.clear();
}

/*----------------------------------------------------------------------------*/

const long Parser::FIRST_SEQNUM;
const long Parser::INVALID_LONG;
const char *Parser::CRLF = "\r\n";
const char Parser::DELIMITER;
const char Parser::TYPE_FOLLOW;
const char Parser::TYPE_UNFOLLOW;
const char Parser::TYPE_BROADCAST;
const char Parser::TYPE_PRIVATE;
const char Parser::TYPE_STATUSUPDATE;
const char Parser::TYPE_INVALID;

bool Parser::findMessage(const string &str, size_t &start, string &message)
{
    // Find CRLF (any of them)
    size_t pos = str.find_first_of(CRLF, start);

    if (pos != string::npos)
    {
        message = str.substr(start, pos - start);
        start = pos;

        // Skip CRLF.
        while (start < str.length() && (str[start] == CR || str[start] == LF))
        {
            start++;
        }

        if (start >= str.length())
        {
            // Indicate that there is no next message.
            start = string::npos;
        }

        return true;
    }

    return false;
}

long Parser::parseLong(const string &str)
{
    long res = strtol(str.c_str(), NULL, 10);
    if (res == 0 || res == LONG_MAX || res == LONG_MIN)
    {
        res = INVALID_LONG;
    }

    return res;
}

void Parser::parseEvent(Event &event)
{
    event.m_seqnum = INVALID_LONG;
    event.m_type = TYPE_INVALID;
    event.m_fromUserId = INVALID_LONG;
    event.m_toUserId = INVALID_LONG;

    stringstream ss(event.m_payload);
    string token;
    unsigned int tokenNum = 0;

    while (getline(ss, token, DELIMITER) && tokenNum < 4)
    {
        switch (tokenNum++)
        {
        case 0:
            event.m_seqnum = parseLong(token);
            break;
        case 1:
            event.m_type = token.empty() ? TYPE_INVALID : token[0];
            break;
        case 2:
            event.m_fromUserId = parseLong(token);
            break;
        case 3:
            event.m_toUserId = parseLong(token);
            break;
        default:
            break;
        }
    }
}

bool Parser::isValidEvent(const Event &event)
{
    if (event.m_seqnum == Parser::INVALID_LONG ||
        event.m_type == Parser::TYPE_INVALID)
    {
        // An event must have a valid sequence # and type.
        return false;
    }

    if (event.m_type == TYPE_BROADCAST)
    {
        if (event.m_fromUserId != INVALID_LONG ||
            event.m_toUserId != INVALID_LONG)
        {
            // Broadcast requires no user IDs.
            return false;
        }

    }
    else if (event.m_type == TYPE_FOLLOW ||
             event.m_type == TYPE_UNFOLLOW ||
             event.m_type == TYPE_PRIVATE)
    {
        if (event.m_fromUserId == INVALID_LONG ||
            event.m_toUserId == INVALID_LONG)
        {
            // Follow, Unfollow, and Private events require both user IDs.
            return false;
        }
    }
    else if (event.m_type == TYPE_STATUSUPDATE)
    {
        if (event.m_fromUserId == INVALID_LONG ||
            event.m_toUserId != INVALID_LONG)
        {
            // Status update event requires fromUser ID and no toUser ID.
            return false;
        }
    }
    else
    {
        // Unknown type
        return false;
    }

    return true;
}

void Parser::encodeMessage(const string &payload, string &message)
{
    message = payload;
    message.push_back(LF);
}

} // namespace protocol

} // namespace followerspace
