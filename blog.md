followermaze concerns/TODOs
===============================================================================
- specification is confusing sometimes
  - no clear specification of "current follower". Guessing :-)
  - "The *event source* **connects on port 9090**", "The many *user clients* 
    will **connect on port 9099**", "**clientListenerPort** - Default: 9090"
    Is it legit to connect both event source and clients on the same port?
    Technically you could parse the message and figure out if this is a user 
    client or the event source, but this is not cool. Assuming it's a typo :-)

followermaze log
===============================================================================

25.06.2014
- back on the project. was busy for a few days.
- some design descisions for the first implementation:
  - Value speed of development and runtime performance over portability.
    Define portable interfaces, but don't isolate platform specific 
    implementations. If portability is required it can be achieved by making
    interface classes pure virtual and subclssing the implementations. Another 
    option would be to use pimpl.
  - single threaded Reactor for synchronouse event demultiplexing (if needed 
    scale adding thread/reactor pairs. 
  - Acceptor for registering clients.
  - Reactor owns all event handlers (acceptors and clients) and disposes of 
    them on disconnect or error.
  - Enforce dynamic allocation for all event handlers.
  - Admin port to control (shutdown) server.
- implemented Reactor pattern for event demultiplexing
- implemented Acceptor pattern to connect clients
- implemented multiecho application (multiple clients, admin client)

15.06.2014
- implemented Connection as a wrapper for clent and server sockets
- implemented simple (one client) echo server for test.

14.06.2014
- set up dev environment: Ubuntu x86_64, g++, make, cmake, UnitTest++
- will use Qt Creator as IDE
- set up the project skeleton
- got a response from the customer:
*******************************************************************************
About the questions: this is something you should be able to get from the
overview of the challenge. If you have a look over this again, it might
become clearer.
*******************************************************************************
  I guess the answer to both is "yes".

12.06.2014
- got the project, created a repository on github, analysed the specification,
  sent a few questions to the customer:
*******************************************************************************
1. Can a user be represented by multiple clients (i.e. should the server handle
 multiple clients for the same user ID)? The specification doesn't seem to tell
 otherwise, but I'd like to be sure.
2. The specification doesn't seem to define explicitly what "current follower"
 means. The term is only used to describe the **Status Update** event. I assume
 that after receiving Follow(from, to) user "from" becomes a follower of the
 user "to" until receiving Unfollow(from, to). Is this correct?"
*******************************************************************************
- will implement in C++ for GNU/Linux
- started to think about design

