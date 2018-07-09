/*  NetworkManager: Handles all connection and communication with the server.
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

#ifndef RCLIENT_NETWORK_MANAGER_H_INCLUDED
#define RCLIENT_NETWORK_MANAGER_H_INCLUDED

#include "config.h"
#include "rpacket.h"
#include <string>


namespace rclient{

  /** NetworkManager handles all network traffic between client and R server.
   * Constructor stores host IP and port.
   * On first request to access the server (submit), the NetworkManager establishes the connection.
   * The connection is closed when the NetworkManager is deleted
   */
  class RCLIENT_API NetworkManager{

  public:
    explicit NetworkManager(const RSTRINGTYPE &server_host, const int server_port = 6311, const bool allowAnyVersion = false);
    ~NetworkManager(); // make sure disconnected when destroyed

    const RSTRINGTYPE& getVersion();
    bool isAuthorizationRequired();
    bool hasAuthorizationType(const RSTRINGTYPE &has_type);
    RSTRINGTYPE getKey();
    RSHARED_PTR<const RPacket> submit(RPacket &packet);
  
  private:
    const RSTRINGTYPE m_sHost; // Rserve IP
    const int m_iPort; // Rserve port
    int m_iSock; // connection socket
    bool m_bAnyVersion; // whether or not to allow connection to any version of RServe

    RSTRINGTYPE m_sRserve_version; // string response from server upon connecting
    void send_to_rserve(const unsigned char *buf, const size_t len, const int flags, const std::string &description);
    size_t recv_from_rserve(unsigned char *buf, const size_t len, const int flags, const std::string &description);

    void connect_to_rserve();
    void disconnect();
    void throw_network_error(const std::string &description, const int error_num = 0, const RSTRINGTYPE &error_str = "");
  };
} // close namespace
#endif
