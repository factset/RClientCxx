   Copyright 2014 FactSet Research Systems Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


# C++ RClient#

C++ client for communication with RServe
Information pertaining to RServe and existing RServe clients can be found at http://rforge.net/Rserve/

###Prerequisites###

RClient uses the boost library (v1.41) for shared pointers.
The boost library can be downloaded at http://www.boost.org/users/download/.

Boost Shared Pointers can be replaced with std shared pointers in C++11 by changing RSHARED_PTR and RMAKE_SHARED in the config.h file

RClient's EndianConverter uses boost/detail/endian.hpp. This file has been included with RClient in the event that the consumer opts not to use the boost library.

###Building the demo###

A makefile is included with RClient.
- 'make' will build RClient
- 'make DEBUG=1' will build RClient with optimization -O0 and the -g flag set
- 'make clean' will remove the executable and .o files

###Running the demo###

Once demo has been built, make sure that there is an RServe to connect to. See http://rforge.net/Rserve/ to set up a server.
- './demo' will run the demo with default settings: connecting to RServe at 'localhost' on port 6311
- './demo help' will print the usage for the demo.
- './demo -h host' will run the demo, connecting to <host> at default port 6311
- './demo -p port' will run demo, connecting at port <port>
- './demo -l username password' will attempt to login to RServe if authentication is required. Otherwise the username and password are ignored.
- './demo -s' sets flag to tell server to shutdown instead of calling assign and eval
- './demo -a' sets flag for RClient to connect to any version of RServe. Otherwise, RClient will only connect to version 0103

Flags '-h', '-p', '-l', '-s', and '-a' can be combined.

- host: string - either the hostname or IP address of RServe
- port: integer - port that RServe is bound to
- username: string - valid username for the RServe getting connected to
- password: string - valid password for corresponding username

If there is not a server listening at the given host and port, RClient will fail with a runtime exception, declaring that it cannot connect.

###Additional Information###

RClient was written to execute on Unix and VMS. It has not been tested on other systems and behavior is unknown.

If a network error occurs and a runtime_error is thrown, then the connection is closed and the session is lost. Following calls to RClient will attempt to establish a new connection.

Implemented RServe Commands:
- login
- assign
- eval
- shutdown

Implemented REXP Types:
- REXPDouble
- REXPInteger
- REXPString
- REXPNull
- REXPList
- REXPPairList
