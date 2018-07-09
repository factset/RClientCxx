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

#include "network_manager.h"
#include "endian_converter.h"
#include "network_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <inttypes.h>
#include <sstream>
#include <string.h>
#include <cerrno> // for errno

// MSG_NOSIGNAL is not defined on VMS, so define it to be an empty flag
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

namespace{
 
  /** traverses possible IPs in addrinfo, attempting to connect to one.
   * @param[in] host addrinfo containing host IPs
   * @param[in] sock socket to establish connection
   */
  bool findConnection(rclient_addrinfo *host, const int sock){
    rclient_addrinfo *res = NULL;
    // traverse possible IP's
    for(res = host; res != NULL; res = res->ai_next){
      // attempt to connect
      if(! ::connect(sock, res->ai_addr, res->ai_addrlen)){
	// connection succeeded
	return true;
      }
    }
    return false;
  }

  /** Custom deleter for rclient_addrinfo calls freeaddrinfo
   * @param[in] host addrinfo to be freed
   */
  void addrinfo_deleter(rclient_addrinfo *host){
    freeaddrinfo(host);
  }

  /** size of string initially sent from RServe
   */
  const int RserveIDLength = 32;


  /** Parse ID String sent from RServe and compare it to RClient's compatibility.
   * A flag can be set to accept any protocol, but the ID must contain "RSrv" and "QAP1"
   * @param[in] version 32-byte ID String sent from RServe declaring version
   * @param[in] anyVersion bool indicating whether or not to accept any RServe version protocol (e.g. 0103, 0102...)
   * @return TRUE if RServe version is compatible with RClient. False otherwise.
   */
  bool versionMatch(const RSTRINGTYPE version, const bool anyVersion){
    // check that first 4 chars are Rsrv
    if (strncmp(&version.c_str()[0], "Rsrv", 4) != 0) return false;

    // check protocol version # if anyVersion is not set
    if(!anyVersion)
      if(strncmp(&version.c_str()[4], "0103", 4) != 0) return false;

    // check for QAP1
    if (strncmp(&version.c_str()[8], "QAP1", 4) != 0) return false;

    return true;
  }

} // close namespace


namespace rclient{

  /** Constructor stores host IP and port and will connect on first submit call
   * @param[in] host Rserve IP address
   * @param[in] port Port that Rserve is listening on
   * @param[in] allowAnyVersion Whether or not to allow connection to any version of RServe. Otherwise only version 0103 is permitted.
   */
  NetworkManager::NetworkManager(const RSTRINGTYPE &server_host, const int server_port, const bool allowAnyVersion): m_sHost(server_host), m_iPort(server_port), m_iSock(-1), m_bAnyVersion(allowAnyVersion) {}


  /** Destructor attempts to disconnect from Rserve using disconnect()
   */
  NetworkManager::~NetworkManager(){
    disconnect();
  }


  /** Disconnects from the Rserver if the NetworkManager is connected
   * If NetworkManager is connected and fails to disconnect, a NetworkError is thrown
   */
  void NetworkManager::disconnect(){
    // return if already disconnected
    if (m_iSock < 0) return;
    
    // close socket
    close(m_iSock);
    m_iSock = -1;
    // NetworkManager is disconnected
    return;
  }

  /** Helper function for network failures. Disconnects from RServe and throws exception
   * Prevents manually calling disconnect wherever an error can occur.
   * @param[in] description String explaining how the error occured
   * @param[in] error_num Integer corresponding to errno or a returned error value.
   * @param[in] error_str String corresponding to error_num (used if error_num is not errno)
   */
  void NetworkManager::throw_network_error(const std::string &description, const int error_num, const RSTRINGTYPE &error_str){
    disconnect();
    if(error_str.empty())
      throw NetworkError(description, error_num, error_str);
    throw NetworkError(description, error_num);
  }


  /** sends data over connected socket and throws exception if bytes sent does not match expected size
   * @param[in] buf data to be sent to RServe
   * @param[in] len length of buf to be sent
   * @param[in] flags flags to use for send()
   * @param[in] description string explaning what is being sent. Used for throwing exception if an error occurs.
   */
  void NetworkManager::send_to_rserve(const unsigned char *buf, const size_t len, const int flags, const std::string &description){
    ssize_t netStatus;
    size_t i = 0;
    while(i < len){
      netStatus = ::send(m_iSock, &buf[i], len-i, flags);
      if(netStatus <= 0){
        // if netStatus is 0, then errno was not set. Assume connection was reset by peer
        if(netStatus == 0)
          throw_network_error(std::string("Error occured while trying to send: " + description), ECONNRESET);
        // error occured, try again if it was EINTR
        if(errno == EINTR) continue;
        // otherwise throw with errno
        throw_network_error(std::string("Error occured while trying to send: " + description), errno);
      }
      i += netStatus;
    }
  }


  /** sends data over connected socket and throws exception if bytes sent does not match expected size
   * @param[out] buf buffer to fill with data from RServe
   * @param[in] len length of buf to be sent
   * @param[in] flags flags to use for send()
   */
  size_t NetworkManager::recv_from_rserve(unsigned char *buf, const size_t len, const int flags, const std::string &description){
    ssize_t netStatus;
    size_t i = 0;
    while (i < len){
      netStatus = ::recv(m_iSock, &buf[i], len-i, flags);
      if(netStatus <= 0){
        // if netStatus is 0, then errno was not set. Assume connection was reset by peer
        if(netStatus == 0)
          throw_network_error(std::string("Error occured while receiving: " + description), ECONNRESET);
        // error occured, try again if it was EINTR
        if(errno == EINTR) continue;
        // otherwise throw with errno
        throw_network_error(std::string("Error occured while receiving: " + description), errno);
      }
      i += netStatus;
    }
    return i;
  }


  /** Establishes a connection with a Rserve
   * If connection fails, sock is set to -1 and a NetworkError is thrown
   * Otherwise, the sock is set accordingly
   */
  void NetworkManager::connect_to_rserve(){
    // return if already connected
    if(m_iSock >= 0) return;

    // bind socket
    m_iSock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(m_iSock < 0){
      throw_network_error("ERROR:: Failed to obtain socket.\n", errno);
    }

    // resolve hostname and establish connection
    rclient_addrinfo *host = NULL;
    rclient_addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::stringstream port;
    port << m_iPort;
    int stat = getaddrinfo(m_sHost.c_str(), port.str().c_str(), &hints, &host);
    if(stat != 0){
      // error occured
      throw_network_error("ERROR:: Failed to obtain host address.\n", stat, gai_strerror(stat));
    }
    // create wrapper for host to make sure it is freed using freeaddrinfo().
    RSHARED_PTR<rclient_addrinfo> tofreehost(host, addrinfo_deleter);
    
    // try to connect
    if (!findConnection(host, m_iSock)){
      // error occured
      throw_network_error("ERROR:: Failed to connect to host.\n", errno);
    }

    // connection was successful and socket is established

    // read 32-byte header sent by the server
    
    unsigned char serverID[RserveIDLength+1];
    recv_from_rserve(serverID, RserveIDLength, 0, "RServe ID");

    serverID[RserveIDLength] = 0;
    m_sRserve_version = RSTRINGTYPE((char*) serverID);

    // check version
    if(!versionMatch(m_sRserve_version, m_bAnyVersion)){
      // incompatible server
      throw_network_error("ERROR:: RServe version is incompatible with RClient.\n"); // close socket
    }
  }


  /** retrieves Rserve server information from initial connection
   * If the client is not connected yet, then it will connect to retrieve version information
   * getVersion() will throw a NetworkError if the connection fails
   * @return string displaying Rserve server version info
   */
  const RSTRINGTYPE& NetworkManager::getVersion(){
    if(m_iSock < 0){
      // if not connected, try to establish connection
      connect_to_rserve();
    }
    // connection succeeded, return version info
    return m_sRserve_version;
  }


  /** Checks RServe version information to determine if authentication is required
   * If the client is not connected yet, then it will connect to retrieve Authentication type
   * @return True if authentication is required, False otherwise
   */
  bool NetworkManager::isAuthorizationRequired(){
    if(m_iSock < 0){
      // if not connected, try to establish connection
      connect_to_rserve();
    }
    // start looking for authentication type at beginning of optional attributes
    for(size_t i = 12; i < m_sRserve_version.size(); i += 4){
      if(m_sRserve_version.c_str()[i] == 'A' &&
         m_sRserve_version.c_str()[i+1] == 'R'){
        return true;
      }
    }
    return false;
  }


  /** Checks RServe version information to determine if the provided authentication version is supported
   * If the client is not connected yet, then it will connect to retrieve Authentication type
   * @param[in] has_type 2-character string for which authentication type is supported. "uc" for crypt. "pt" for plain text
   * @return True if the provided authentication type is supported. False otherwise.
   */
  bool NetworkManager::hasAuthorizationType(const RSTRINGTYPE &has_type){
    if(m_iSock < 0){
      // if not connected, try to establish connection
      connect_to_rserve();
    }
    char type[3] = {'n', 'a', 0};
    // start looking for authentication type at beginning of optional attributes
    for(size_t i = 12; i < m_sRserve_version.size(); i += 4){
      if(m_sRserve_version.c_str()[i] == 'A' &&
         m_sRserve_version.c_str()[i+1] == 'R'){
        memcpy(type, &m_sRserve_version.c_str()[i+2], 2);
        if(RSTRINGTYPE(type) == has_type)
          return true;
      }
    }
    return false;
  }
  

  /** Retrieves key for authentication (default being "rs")
   * If the client is not connected yet, then it will connect to retrieve key
   * @return String representing authentication key
   */
  RSTRINGTYPE NetworkManager::getKey(){
    if(m_iSock < 0){
      // if not connected, try to establish connection
      connect_to_rserve();
    }
    char key[3] = {'r', 's', 0};
    // start looking for key at beginning of optional attributes
    for(size_t i = 12; i < m_sRserve_version.size(); i += 4){
      if(m_sRserve_version.c_str()[i] == 'K'){
        memcpy(key, &m_sRserve_version.c_str()[i+1], 2);
        break;
      }
    }
    return RSTRINGTYPE(key);
  }


  /** Sends Rpacket to connected Rserve and then waits for response
   * If connection is not yet established, then it will do so before making the request
   * submit can throw a NetworkError if the connection failed 
   * Reads the 16 byte header to know the size of the packet
   * Reads the remainder of the packet based on length designated by header
   * Parses server's response into a new Rpacket to be returned
   * @param[in] Rpacket to be sent to the Rserve
   * @return Rpacket response sent back from the server
   */
  RSHARED_PTR<const RPacket> NetworkManager::submit(RPacket &packet){
    if(m_iSock < 0){
      // if not connected, try to establish connection
      connect_to_rserve();
    }
    
    EndianConverter converter;

    // send QAP1Header
    QAP1Header qap1_request = packet.getHeader();
    RVECTORTYPE<uint8_t> networkHeader;
    int header_size = sizeof(uint32_t) * 4;
    networkHeader.resize(header_size);
    size_t pos = 0;
    qap1_request.getCommand(networkHeader, pos);
    pos += sizeof(uint32_t);
    qap1_request.getLength(networkHeader, pos);
    pos += sizeof(uint32_t);
    qap1_request.getOffset(networkHeader, pos);
    pos += sizeof(uint32_t);
    qap1_request.getLength_highbits(networkHeader, pos);

    send_to_rserve(&networkHeader[0], header_size, MSG_NOSIGNAL, "QAP1Header.");

    // send each entry in the data
    for(size_t i = 0; i < packet.getEntries()->size(); ++i){
      RPacket::PacketEntry entry = (*packet.getEntries())[i];
      send_to_rserve(&entry.getEntry()[0], entry.getLength(), MSG_NOSIGNAL, "RPacket Entry Data.");
    }
 
    // read QAP1Header
    recv_from_rserve(&networkHeader[0], header_size, 0, "Response QAP1Header.");

    // parse response into QAP1Header
    QAP1Header qap1_response(networkHeader); 

    // swap endian of length to be readable by client
    size_t responseLength = qap1_response.getLength();

    // entry variables
    RVECTORTYPE<RPacket::PacketEntry> entrylist;
    RVECTORTYPE<unsigned char> responseBuffer;
    size_t bytesRead = 0;

    // read + parse each entry
    while( bytesRead < responseLength){
      responseBuffer.resize(sizeof(uint32_t)+1); // resize to header
      // read 4 byte entry header
      bytesRead += recv_from_rserve( &responseBuffer[0], sizeof(uint32_t), 0, "4-byte response entry header");

      // if 8 byte header, read another 4 bytes, then rest of entry
      if(responseBuffer[0] & RPacket::PacketEntry::DT_LARGE){
	responseBuffer.resize(sizeof(uint64_t)+1); // resize to large header
	// read another 4 bytes for header
	bytesRead += recv_from_rserve(&responseBuffer[4], sizeof(uint32_t), 0, "8-byte response entry header");

	// convert entry length to client endian to be readable
	RVECTORTYPE<uint8_t> serialized_entry_length;
	serialized_entry_length.resize(sizeof(uint64_t));
	pos = 0;
	memcpy(&serialized_entry_length[0], &responseBuffer[1], sizeof(uint64_t)-1);
	memset(&serialized_entry_length[7], 0, 1);
	size_t entryLength = converter.deserialize<uint64_t>(serialized_entry_length, pos);
	responseBuffer.resize(sizeof(uint64_t) + entryLength); // resize to entry size

	// read rest of entry
	bytesRead += recv_from_rserve(&responseBuffer[8], entryLength, 0, "Response entry data (for DT_LARGE)");

	// create RPacketEntry
	entrylist.push_back(RPacket::PacketEntry(responseBuffer));
      }

      // otherwise 4 byte header, read rest of entry
      else {
	responseBuffer[4] = 0;
	// determine 3byte entry size
	RVECTORTYPE<uint8_t> serialized_entry_length;
	serialized_entry_length.resize(sizeof(uint32_t));
	pos = 0;
	memcpy(&serialized_entry_length[0], &responseBuffer[1], sizeof(uint32_t)-1);
	memset(&serialized_entry_length[3], 0, 1);
	uint32_t entryLength = converter.deserialize<uint32_t>(serialized_entry_length, pos);
	responseBuffer.resize(sizeof(uint32_t) + entryLength); // resize to entry size

	// read rest of entry
	bytesRead += recv_from_rserve(&responseBuffer[4], entryLength, 0, "Response entry data");

	// create RPacketEntry
	entrylist.push_back(RPacket::PacketEntry(responseBuffer));
      }
    }
  
    // return RPacket created out of entries
    return RMAKE_SHARED<RPacket>(qap1_response, entrylist);
  }

  // may need to add a submit_disconnect for detachedVoidEval in the future

} // close namespace
