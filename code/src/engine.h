/* This file declears the Engine class.
 */
#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include "protocol.h"

namespace followermaze
{

namespace protocol
{

/* Engine encapsulates the business logic of the followermaze application.
 * It encapsulates the state of the application (event queue and list of users)
 * and implements the logic of registering/unregistering users and processing
 * events according to the rules specified by the protocol.
 * Events are processed in batches. A batch gets sorted to ensure that the
 * users will get events in correct order.
 * Events can generate notifications which are delivered to the users.
 */
class Engine
{
public:
    Engine();
    virtual ~Engine();

    // Parses events, sorts them, processes in order.
    void handleEvents(string &events);

    // Register the userClient to represent a user identified by
    // content of in.
    // Returns user ID if successful, Parser::INVALID_LONG otherwise.
    long registerUser(UserClient *userClient, const string &in);

    // Unregister the userClient for the user identified by the id.
    void unregisterUser(long id, UserClient *userClient);

    // Resets the event queue and cleans up all the state so the Engine is
    // ready to start again. Doesn't affect registered users.
    void resetEventQueue();

protected:
    // Handle "Follow" event
    void handleFollow(const Event& event);

    // Handle "Unfollow" event
    void handleUnfollow(const Event& event);

    // Handle "Broadcast" event
    void handleBroadcast(const Event& event);

    // Handle "Private" event
    void handlePrivate(const Event& event);

    // Handle "StatusUpdate" event
    void handleStatusUpdate(const Event& event);

    // Helper function which creates a new user and adds it to the m_users.
    User* addNewUser(long id);

    // Helper function which sends a message to all the registered clients.
    void notifyUser(User *user, const string &payload);

    // Returns true if user has no clients, no followers, and no followees.
    bool isBlankUser(const User& user);

protected:
    UserMap m_users;
    EventQueue m_events;
    long m_nextEventSeqnum;
};

} // namespace protocol

} // namespace followermaze

#endif // ENGINE_H
