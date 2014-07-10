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
  * GCC or an alternative, reasonably conformant C++ compiler.
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

  Example for Ubuntu (assuming current derectory is the project root directory):
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

5. Architecture design

6. Performance

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

  Note: available tests ensure reasonable quality, but don't provide 100% 
  coverage. In fact no effort has been made to measure test coverage due to the
  lack of time. This would have to be different if the project required a 
  lengthy maintenance and support cycle.

