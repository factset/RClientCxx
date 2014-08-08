/*  Configuration file
 *  Copyright 2014 FactSet Research Systems Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef RCLIENT_CONFIG_H_INCLUDED
#define RCLIENT_CONFIG_H_INCLUDED


#ifndef RCLIENT_API
// add other defaults if you wish, like checking for __GNUC__, etc. Important thing is to make sure it gets defined to an empty string in a fallback scenario
#define RCLIENT_API
#endif

// RClient's vector type
#include <vector>
#define RVECTORTYPE std::vector

// RClient's string type
#include <string>
typedef std::string RSTRINGTYPE;

// Shared Pointer and make_shared
#include <boost/shared_ptr.hpp>
#define RSHARED_PTR boost::shared_ptr

#include <boost/make_shared.hpp>
#define RMAKE_SHARED boost::make_shared


// typedef addrinfo to compile on both VMS and Linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#if defined __VMS && __INITIAL_POINTER_SIZE == 64 && __CRTL_VER >= 70311000
typedef __addrinfo64 rclient_addrinfo;
#else
typedef struct addrinfo rclient_addrinfo;
#endif


#endif
