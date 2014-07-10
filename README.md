===============================================================================
================= README == followermaze - the challenge ======================
===============================================================================

 1.) Prerequisites
 2.) Project structure
 3.) Building
 4.) Installing
 5.) Architecture design
 6.) Performance
 7.) Testing

1. Prerequisites

 This project requires:
  * Cross-platform Make (CMake) v2.8.12+
  * GNU Make or equivalent.
  * GCC or an alternative, reasonably conforming C++ compiler.
  * UnitTest++ (http://unittest-cpp.sourceforge.net/)

  Example for Ubuntu:
    $ sudo apt-get install build-essential
    $ sudo apt-get install cmake
    $ sudo apt-get install libunittest++-dev

2. Project structure
  ./README               - this document
  ./blog.md              - project log
  ./spec/instructions.md - project specification
  ./testsuite            - test application provided by the customer
  ./code/src             - source code
  ./code/tests/unit      - unit tests
  ./code/tests/apps      - test applications
  ./code/tests/runner    - wrapper template for the automated test
 
3. Building

  This project uses the Cross-platform Make (CMake) build system.
  Out of source build is recommended.

  Example for Ubuntu (assuming current directory is the project root directory):
    $ mkdir build
    $ cd build
    $ cmake ../code
    $ make

4. Installing

  This project's build system has the install target which installs the
  followermaze executable to the default (e.g. /usr/local/bin on Linux)
  location.

  Example for Ubuntu:
    $ sudo make install

  To uninstall one could use 'install_manifest.txt' created in the root build
  directory.

  Example for Ubuntu:
    $ sudo xargs rm < install_manifest.txt

  To get help on how to use followermaze run:
    $ followermaze -h

  WARNING: followermaze has a hard-coded limit (1024) on number of connected
  clients. See Performance section for more information on that.

5. Architecture design
  The following design concerns have been identified:
  - Portability
  - Performance
  - Scalability
  - Testability

  The following design decisions have been taken to address the concerns or
  to enable future improvements:
  - Focus on separation of concerns. The essential subsystems (stream sockets,
    network I/O handling, object model of the problem domain, protocol parser,
    and business logic) should be isolated in parallel class hierarchies and
    communicate over semantically clean interfaces. This addresses Portability,
    Extensibility, and Testability.
  - Use facade wrappers to abstract the platform dependent code. This addresses
    Portability.
  - Compact memory management strategy. Objects should be created and destroyed
    in controllable manner in as few places as possible. Object ownership should
    be well defined and concentrated in as few places as possible. This addresses
    Performance, Scalability, and Testability.
  - Use non-blocking I/O and synchronous event demultiplexing. Consequently the
    network I/O handling subsystem can run in a single thread. This approach
    could negatively impact Performance and Scalability. Solutions exist to
    overcome this (e.g. by implementing multiple threads which take care of a
    portion of clients). This is acceptable in the context of followermaze
    application since all the clients but *event source* are passive.
    Alternative approaches include:
      - using blocking I/O and multi-threading. This affects Scalability and
        Performance due to higher memory requirements)
      - using event based I/O handling. This is considered to be the preferable
        solution for building high performance servers however it is less
        portable and requires more complicated event handling code which affects
        Portability and Testability.
    This addresses Performance, Scalability, and Testability.
 
  The following classes and collaborations :
  Framework:
  - Connection - facade wrapper for stream sockets. Owns an I/O Handle.
  - EventHandler - abstract class defining the call back interface for handling
    I/O events on a resource represented by a Handle.
  - EventHandlerFactory - base factory used to instantiate EventHadlers.
  - Client - EventHandler specialization which encapsulates a Connection and
    implements the common logic (error handling and cleanup) for Connection based
    handlers.
  - Admin - Client specialization which is used to interrupt the main event
    loop from another process.
  - Acceptor - EventHandler which owns a listening (server) Connection, accepts
    client connection requests and creates appropriate clients using concrete
    EventHandlerFactory.
  - Reactor - implements poll based synchronous event demultiplexing and
    dispatching of events to the appropriate EventHandlers. Reactor also owns all
    EventHandlers in the system and makes sure they are disposed of.
  - Server - implements Reactor based event loop.
  - Logger, BaseException - tools for logging and exception handling.
 
  followermaze application logic:
  - Event - an event which is sent by the *event source*.
  - User - represent a user. Tracks user's followers, followees, and *user clients*.
  - EventQueue - a vector of Events which is sorted so that the Event with lowest
    sequence number is in the back.
  - EventSource - Client representing *event source*
  - UserClient - Client representing *user client*
  - Parser - implements application protocol parser
  - Engine - implements business logic (handling of *user clients*, event
    processing rules). Engine owns all the domain data model objects and makes
    sure they are disposed of.
  - EngineDrivenClientFactory - concrete factory to create clients which use
    Engine for business logic (that is EventSource and UserClient).
  - SimpleServer - a Server which implements followermaze application logic.
    This includes:
       - server configuration (via CLI).
       - create required Acceptors (for Admin, EventSource, and UserClient) to
         seed the Reactor.
       - server shut down using Admin (via CLI). This uses system() call to
         'echo' and 'nc' so it's a bit of a hack :-)
       - error handling.

  NOTE: This design doesn't address all the concerns fully. Some of the design
  decisions have not been fully implemented in favour of development speed.

6. Performance
  Derived from the specification essential non-functional requirements for the
  followermaze application are:
  - *user client* shouldn't time out (the default timeout is 20 sec).
  - followermaze should be able to handle arbitrary number of events.

  The following parameters mostly affect followermaze's performance:
  - **maxEventSourceBatchSize**
   
    Events are supposed to be delivered in order so they must be sorted before
    the application can start notifying users. This assumes logarithmic
    complexity.
   
    Events need to be parsed which assumes linear complexity.
    In order to be sorted all (worst case) events in a batch have to be stored
    in memory.
   
    Consequently, it's always possible to choose a value for this parameter
    which would force the application run out of memory or become so slow that
    the clients would start to time out.   
   
    followermaze uses sorted vector to store events. Sorting has logarithmic
    complexity.

    followermaze iterates over the list of events to process them so
    the overall handling is linear.
   
    This should have very noticable effect on performance.
   
  - **totalEvents**
    This should not affect followermaze's performance (unless there is a memory
    management bug) since it only deals with one batch at a time.

  - **concurrencyLevel**
    followermaze calls poll and iterates the file descriptors to demultiplex the
    I/O events. This assumes linear complexity.
   
    followermaze has a hard-coded limit of 1024 connected clients and will refuse
    to accept more connections. There is no particular reason for this limitation
    for there is no hard-coded limitation for poll. However, overhead for passing
    the data structures to the kernel and back as well as iterating the array
    becomes significant when the number gets to several thousands. Different
    design (see Architecture design section) should be used to support large
    amounts of clients. This is a big discussion which is out of scope for this
    project (see http://www.kegel.com/c10k.html). It would be interesting to
    test how many clients followermaze can actually support, but maybe later :-)
   
    In order to notify users followermaze needs to find a user by ID while
    processing events (except Unfollow). followermaze uses std::map to store the
    users so the complexity of search is logarithmic (except Broadcast which is
    obviously linear).

    Since the *user clients* are passive most of the time, this parameter is
    unlikely to significantly affect the system (unless pushed to multiple
    thousands when the above mentioned factors will start to be noticed).

  - **numberOfUsers**
    followermaze tracks the follower<->followee relationship for the users. It can
    happen that this relationship is mentioned in the event stream even though the
    participating users are not connected. This means that in the worst case
    scenario the application would have to allocate significant amount of memory
    and also spend some time searching for users in the internal data structures
    to send notifications.

    followermaze stores followers and followees in std::map. This assumes
    logarithmic search.

    This could affect performance in case a lot of F or U events will come for
    unconnected users. However, this doesn't seem to be a very meaningful use case.

  Some tests have been executed to collect evidence in support of the analysis
  above. These tests should be taken with a pinch of salt for following reasons:
    - provided test suite randomizes the load and also seems to get quite slow
      when executed with high level of concurrency and large or batch size.
    - development environment is a virtual machine with one core :-)
    - no statistical analysis has been done on the results due to lack of time.

  callgrind function profiler has been used to run several tests with different
  combinations of these parameters. Following observations have been made:
    - about 99% of the time is spent (as expected) processing input from the
      *event source*.
    - about 60% of the processing is spent parsing the input.
    - about 10% of the processing is spent managing the event queue
    - the rest of the processing is actually spent handling events
    - these results were not noticeably affected by changing the key
      parameters except **maxEventSourceBatchSize**. When increasing the latter
      time spent managing the event queue increased, too.
 
  valgrind memory profiler has been used to ensure lack of memory leaks.

  All in all followermaze performs reasonably well and according to expectation.
  Having said that the current bottleneck (parsing) can definitely be improved.
  std::stringstream and std::getline are used for the sake of development speed
  and code readability. Should be possible to do it better.

7. Testing
  The following testing facilities have been implemented:

  a) Unit tests. Simple unit test framework (UnitTest++) has been used. A
  target has been added to the build system to run unit tests for every build.

  Example for Ubuntu:
    $ make check

  b) Automated tests. The project uses CTest (delivered with CMake) to
  implement test automation. Project's build system generates a wrapper shell
  script which starts the followermaze server in the background, starts the
  test application (provided with the challenge), and finally stops the server.
  The tests are defined in ./code/tests/runner/CMakeLists.txt.
 
  WARNING: one of the tests checks the acceptance criteria for the project
  (running provided test application with all default parameters). It can take
  several minutes to complete.

  To display the list of available tests run:
    $ ctest -N

  To execute tests run (test's output is being swallowed):
    $ make test

  To execute tests verbosely run:
    $ ctest -V

  c) Two test applications which have been used during development to test
  parts of the system manually are provided as reference implementation and for
  manual testing.

  Additionally valgrind has been used to test memory management.

  Note: available tests ensure reasonable quality, but don't provide 100%
  coverage. In fact no effort has been made to measure test coverage due to the
  lack of time. This would have to be different if the project required a
  lengthy maintenance and support cycle.


