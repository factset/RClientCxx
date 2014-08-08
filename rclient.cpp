/*  RClient: Core object included by consumer. Handles RServe commands.
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

#include "rclient.h"

namespace rclient{

  /** On initialization, the RClient creates a NetworkManager with provided IP and port.
   * If consumers want to access multiple server, they must declare an RClient for each one.
   * @param[in] host IP address of the Rserve
   * @param[in] port Port that the Rserve is listening for new connections on (default 6311)
   * @param[in] allowAnyVersion Whether or not to allow connection to any version of RServe. Otherwise only version 0103 is permitted.
   */
  RClient::RClient(const RSTRINGTYPE &host, const int port, const bool allowAnyVersion):m_NetMan(host,port, allowAnyVersion){}

  /** Sends request to server to shut down the server.
   * @param[in] key session key (!!! need to confirm purpose)
   * @return TRUE if request successful, FALSE if the request failed 
   */
  bool RClient::shutdown(const RSTRINGTYPE &key){
    RVECTORTYPE<RPacket::PacketEntry> entrylist;
    entrylist.resize(1);
    entrylist[0] = RPacket::PacketEntry(key);
    // make RPacket to be sent
    RPacket toSend(RPacket::CMD_shutdown, entrylist);
    // submit packet and receive the response

    RSHARED_PTR<const RPacket> response = m_NetMan.submit(toSend);
    // store response in RClient
    m_pLast_response = response;
    // return whether or not response was successful
    return response->isOk();
  }


  /** Sends request to server to set the given symbol
   * @param[in] sym symbol to have R expression assigned to
   * @param[in] expr R expression to be assigned to sym
   * @return TRUE if assignment was successful, FALSE if the request failed
   */
  bool RClient::assign(const RSTRINGTYPE &sym, const REXP &expr){
    RVECTORTYPE<RPacket::PacketEntry> entrylist;
    entrylist.resize(2);
    entrylist[0] = RPacket::PacketEntry(sym);
    entrylist[1] = RPacket::PacketEntry(expr);

    // make RPacket to be sent
    RPacket toSend(RPacket::CMD_setSEXP, entrylist);

    // submit packet and receive the response
    RSHARED_PTR<const RPacket> response = m_NetMan.submit(toSend);
    // store response in RClient
    m_pLast_response = response;
    // return whether or not response was successful
    return response->isOk();
  }


  /** templated overload for assign to improve versatility, creates REXP to be used by assign(sym,REXP)
   * @param[in] sym symbol to have R expression assigned to
   * @param[in] expr R expression to be assigned to sym
   * @return TRUE if assignment was successful, FALSE if the request failed
   */
  template<typename T_VAL, typename T_REXP>
  bool RClient::assign(const RSTRINGTYPE &sym, const T_VAL &expr){
    T_REXP rexp(expr);
    return assign(sym, rexp);
  }
  template RCLIENT_API bool RClient::assign<double, REXPDouble>(const RSTRINGTYPE &sym, const double &expr);
  template RCLIENT_API bool RClient::assign<int32_t, REXPInteger>(const RSTRINGTYPE &sym, const int32_t &expr);
  template RCLIENT_API bool RClient::assign<RSTRINGTYPE, REXPString>(const RSTRINGTYPE &sym, const RSTRINGTYPE &expr);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<double>, REXPDouble>(const RSTRINGTYPE &sym, const RVECTORTYPE<double> &expr);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<float>, REXPDouble>(const RSTRINGTYPE &sym, const RVECTORTYPE<float> &expr);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<int32_t>, REXPInteger>(const RSTRINGTYPE &sym, const RVECTORTYPE<int32_t> &expr);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<RSTRINGTYPE>, REXPString>(const RSTRINGTYPE &sym, const RVECTORTYPE<RSTRINGTYPE> &expr);

  /** templated overload for assign to improve versatility, creates REXP to be used by assign(sym,REXP)
   * @param[in] sym symbol to have R expression assigned to
   * @param[in] expr R expression to be assigned to sym
   * @param[in] consumerNAValue NA representation used by consumer
   * @return TRUE if assignment was successful, FALSE if the request failed
   */
  template<typename T_VAL, typename T_REXP, typename T_NA>
  bool RClient::assign(const RSTRINGTYPE &sym, const T_VAL &expr, const T_NA &consumerNAValue){
    T_REXP rexp(expr, consumerNAValue);
    return assign(sym, rexp);
  }
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<double>, REXPDouble, double>(const RSTRINGTYPE &sym, const RVECTORTYPE<double> &expr, const double &consumerNAValue);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<float>, REXPDouble, float>(const RSTRINGTYPE &sym, const RVECTORTYPE<float> &expr, const float &consumerNAValue);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<int32_t>, REXPInteger, int32_t>(const RSTRINGTYPE &sym, const RVECTORTYPE<int32_t> &expr, const int32_t &consumerNAValue);
  template RCLIENT_API bool RClient::assign<RVECTORTYPE<RSTRINGTYPE>, REXPString, RSTRINGTYPE >(const RSTRINGTYPE &sym, const RVECTORTYPE<RSTRINGTYPE> &expr, const RSTRINGTYPE &consumerNAValue);


  /** Sends request to server to evaluate the provided string
   * @param[in] expr R expression to be evaulated on the server
   * @return return value of the executed R expression
   */
  RSHARED_PTR<const REXP> RClient::eval(const RSTRINGTYPE &expr){

    // make RPacket entries
    RVECTORTYPE<RPacket::PacketEntry> entrylist;
    entrylist.resize(1);
    entrylist[0] = RPacket::PacketEntry(expr+"\n");
    // make RPacket to be sent
    RPacket toSend(RPacket::CMD_eval, entrylist);
    // submit packet and receive the response
    RSHARED_PTR<const RPacket> response = m_NetMan.submit(toSend);
    // store response in client
    m_pLast_response = response;
    // return first entry
    return response_REXPAt(0);
  }

  /** Checks the QAP1Header of the most recent server response to see if the previous server command was successful
   * @return True if first bit is set in the command, representing a successful request. False otherwise
   */
  bool RClient::response_isSuccessful() const{
    return m_pLast_response->isOk();
  }

  /** Checks the QAP1Header of the most recent server response to see what (if any) error status is set. 
   * @return eStat corresponding to error type received from the server, or 0 if no error
   * see rpacket.h for list of errors
   */
  uint32_t RClient::response_errorStatus() const{
    return m_pLast_response->getStatus();
  }

  /** Retrieves number of entries in the most recent server response
   * @return Number of entries in the RPacket most recently received from server
   */
  size_t RClient::response_entryCount() const{
    return m_pLast_response->getEntries()->size();
  }

  /** Retrieves the datatype for the entry at designated position in the vector of packet entries of the most recent server response.
   * @param[in] pos position in vector of RPacketEntries in m_pLast_response
   * @return eDataType corresponding to entry type, or -1 if pos is out of bounds.
   * See rpacket_entry.h for list of eDataTypes
   */
  int RClient::response_getType(const size_t pos) const{
    if(pos >= response_entryCount()){
      // pos out of bounds
      return -1;
    }
    return (*m_pLast_response->getEntries())[pos].getDataType();
  }

  /** Retrieves string from the entry at designated position in the vector of packet entries of the most recent server response.
   * @param[in] pos position in vector of RPacketEntries in m_pLast_response
   * @return string containing requested packet entry, or empty string if pos is out of bounds or not of type string.
   */
  RSTRINGTYPE RClient::response_stringAt(const size_t pos) const{
    if(pos >= response_entryCount() || response_getType(pos) != RPacket::PacketEntry::DT_STRING){
      // pos out of bounds
      return RSTRINGTYPE(); // or throw exception
    }

    const RPacket::PacketEntry &entry = (*m_pLast_response->getEntries())[pos];

    // check for terminator at end of entry
    size_t len = entry.getLength();
    if(entry.getEntry()[len] ){
      // end of entry is not null, return empty string
      return RSTRINGTYPE(); // or throw exception
    }

    // string at 8 bytes or 4 bytes depending on header size
    return RSTRINGTYPE((char*) &entry.getEntry()[(entry.getHeaderLength() == 8 ? 8:4)]);
  }
  

  /** Retrieves REXP from the entry at designated position in the vector of packet entries of the most recent server response.
   * @param[in] pos position in vector of RPacketEntries in m_pLast_response
   * @return REXP containing requested packet entry data, or REXPNull if pos is out of bounds or not of type string.
   */
  RSHARED_PTR<const REXP> RClient::response_REXPAt(const size_t pos) const{
    if(pos >= response_entryCount()){
      // pos out of bounds
      return RMAKE_SHARED<REXPNull>(); // does not compile with -o2 and higher
    }
    const RPacket::PacketEntry &entry = (*m_pLast_response->getEntries())[pos];
    return entry.toREXP();    
  }

  /** Retrives server version
   * @return string displaying server version
   */
  const RSTRINGTYPE RClient::getRserveVersion(){
    return m_NetMan.getVersion();
  }

} // close namespace
