#include <cstdlib>
#include <climits>
#include <sstream>
#include <assert.h>
#include "engine.h"
#include "client.h"

using namespace std;

namespace followermaze
{

using namespace Protocol;

Engine::Engine() :
    m_nextEventSeqnum(Parser::FIRST_SEQNUM)
{
}

Engine::~Engine()
{
    while (!m_events.empty())
    {
        Event *event = m_events.top();
        m_events.pop();
        delete event;
    }

    for (UserMap::iterator userIt = m_users.begin();
                           userIt != m_users.end();
                           ++userIt)
    {
        User *user = userIt->second;
        userIt->second = NULL;
        delete user;
    }
}

void Engine::handleEvents(string& events)
{
    // Parse all the messages and push valid events into the queue.
    size_t start = 0;
    string message;
    while (Parser::findMessage(events, start, message))
    {
        auto_ptr<Event> event(new Event);
        event->m_payload = message;

        Parser::parseEvent(*event);

        if (Parser::isValidEvent(*event))
        {
            m_events.push(event.get());
            event.release();
        }
    }

    if (start != string::npos)
    {
        // Remainder of the input string is not a message. Return it so the
        // caller has a chance to complete the message and try again.
        events = events.substr(start);
    }
    else
    {
        // All events consumed.
        events.clear();
    }

    // Process events in seqnum order.
    while (!m_events.empty() && m_events.top()->m_seqnum == m_nextEventSeqnum)
    {
        Event *event = m_events.top();

        switch (event->m_type)
        {
        case Parser::TYPE_FOLLOW:
            handleFollow(*event);
            break;
        case Parser::TYPE_UNFOLLOW:
            handleUnfollow(*event);
            break;
        case Parser::TYPE_BROADCAST:
            handleBroadcast(*event);
            break;
        case Parser::TYPE_PRIVATE:
            handlePrivate(*event);
            break;
        case Parser::TYPE_STATUSUPDATE:
            handleStatusUpdate(*event);
            break;
        default:
            // Unexpected event type.
            assert(0);
        }

        m_events.pop();
        m_nextEventSeqnum++;
        delete event;
    }
}

long Engine::registerUser(UserClient *userClient, const string& in)
{
    // Add client to an existing user or create new user.
    string message;
    size_t start = 0;
    if (Parser::findMessage(in, start, message))
    {
        long id = Parser::parseLong(message);
        if (id != Parser::INVALID_LONG)
        {
            User *user = NULL;
            UserMap::const_iterator userIt = m_users.find(id);
            if (userIt != m_users.end())
            {
                user = userIt->second;
            }
            else
            {
                user = addNewUser(id);
            }

            user->m_clients.push_back(userClient);
        }

        return id;
    }

    return Parser::INVALID_LONG;
}

void Engine::unregisterUser(long id, UserClient *userClient)
{
    // Remove userClient from the list of clients of the user identified by id.
    User *user = NULL;
    UserMap::iterator userIt = m_users.find(id);
    if (userIt != m_users.end())
    {
        user = userIt->second;
        for (ClientList::iterator clientIt = user->m_clients.begin();
                                  clientIt != user->m_clients.end();
                                  ++clientIt)
        {
            if (*clientIt == userClient)
            {
                user->m_clients.erase(clientIt);
                break;
            }
        }

        // Cleanup blank users.
        if (isBlankUser(*user))
        {
            m_users.erase(userIt);
            delete user;
        }
    }
}

void Engine::resetEventQueue()
{
    while (!m_events.empty())
    {
        Event *event = m_events.top();
        m_events.pop();
        delete event;
    }

    m_nextEventSeqnum = Parser::FIRST_SEQNUM;

    UserMap::iterator userIt = m_users.begin();
    while (userIt != m_users.end())
    {
        User *user = userIt->second;

        if (user != NULL)
        {
            user->m_followees.clear();
            user->m_followers.clear();

            if (user->m_clients.empty())
            {
                UserMap::iterator userToEraseIt = userIt;
                userIt++;
                m_users.erase(userToEraseIt);
                delete user;
                continue;
            }
        }

        userIt++;
    }
}

void Engine::handleFollow(const Event& event)
{
    // Notify toUser and make fromUser a follower of toUser.
    // Register toUser and fromUser if required.
    User *toUser = NULL;
    UserMap::const_iterator userIt = m_users.find(event.m_toUserId);
    if (userIt == m_users.end())
    {
        toUser = addNewUser(event.m_toUserId);
    }
    else
    {
        toUser = userIt->second;
        notifyUser(toUser, event.m_payload);
    }

    User *fromUser = NULL;
    userIt = m_users.find(event.m_fromUserId);
    if (userIt == m_users.end())
    {
        fromUser = addNewUser(event.m_fromUserId);
    }
    else
    {
        fromUser = userIt->second;
    }

    toUser->m_followers.insert(UserMap::value_type(event.m_fromUserId, fromUser));
    fromUser->m_followees.insert(UserMap::value_type(event.m_toUserId, toUser));
}

void Engine::handleUnfollow(const Event& event)
{
    // Make fromUser not to follow toUser anymore.
    UserMap::iterator toUserIt = m_users.find(event.m_toUserId);
    if (toUserIt != m_users.end())
    {
        User *toUser = toUserIt->second;
        assert(toUser != NULL);

        UserMap::iterator fromUserIt = toUser->m_followers.find(event.m_fromUserId);
        if (fromUserIt != toUser->m_followers.end())
        {
            User *fromUser = fromUserIt->second;
            assert(fromUser != NULL);

            toUser->m_followers.erase(fromUserIt);
            fromUser->m_followees.erase(toUser->m_id);

            // Cleanup blank users.
            if (isBlankUser(*toUser))
            {
                m_users.erase(toUserIt);
                delete toUser;
            }

            if (isBlankUser(*fromUser))
            {
                m_users.erase(fromUser->m_id);
                delete fromUser;
            }
        }
    }
}

void Engine::handleBroadcast(const Event& event)
{
    // Notify all connected users.
    for (UserMap::const_iterator userIt = m_users.begin();
                                 userIt != m_users.end();
                                 ++userIt)
    {
        notifyUser(userIt->second, event.m_payload);
    }
}

void Engine::handlePrivate(const Event& event)
{
    // Notify toUser
    UserMap::iterator userIt = m_users.find(event.m_toUserId);
    if (userIt != m_users.end())
    {
        notifyUser(userIt->second, event.m_payload);
    }
}

void Engine::handleStatusUpdate(const Event& event)
{
    // Notify fromUser's followers.
    UserMap::const_iterator userIt = m_users.find(event.m_fromUserId);
    if (userIt != m_users.end())
    {
        User *fromUser = userIt->second;
        assert(fromUser != NULL);

        for (UserMap::const_iterator followerIt = fromUser->m_followers.begin();
                                     followerIt != fromUser->m_followers.end();
                                     ++followerIt)
        {
            notifyUser(followerIt->second, event.m_payload);
        }
    }
}

User* Engine::addNewUser(long id)
{
    auto_ptr<User> newUser(new User);
    newUser->m_id = id;
    m_users.insert(UserMap::value_type(id, newUser.get()));
    return newUser.release();
}

void Engine::notifyUser(User *user, const string &payload)
{
    assert(user != NULL);

    string message;
    Parser::encodeMessage(payload, message);

    for (ClientList::const_iterator clientIt = user->m_clients.begin();
                                    clientIt != user->m_clients.end();
                                    ++clientIt)
    {
        (*clientIt)->send(message);
    }
}

bool Engine::isBlankUser(const User& user)
{
    return (user.m_clients.empty() && user.m_followers.empty() && user.m_followees.empty());
}

} //  namespace followermaze
