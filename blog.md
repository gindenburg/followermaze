followermaze
===============================================================================

15.04.2012
- implemented Connection as a wrapper for clent and server sockets

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

