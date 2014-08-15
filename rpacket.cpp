/*  RPacket: Network Packet sent to and received from server
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

#include "rpacket.h"
#include <sstream>

namespace rclient{

  /** constructor builds QAP1 header with provided rserve command and data
   * @param[in] cmd Rserve command. see "rpacket.h" for eCMD enums
   * @param[in] entries vector of RPacketEntry. RPacket content to be sent
   */
  RPacket::RPacket(const eCMD &cmd, const RVECTORTYPE<PacketEntry> &entries):m_vecEntrylist(RMAKE_SHARED<RVECTORTYPE<PacketEntry> >(entries)){

    // union to divide low and high bits of length
    union {uint64_t ui64; uint32_t ui32[2];} length;

    // determine packet data length
    length.ui64 = 0;
    for(size_t i = 0; i < entries.size(); ++i){
      length.ui64 += entries[i].getLength();
    }

    // fill in QAP1Header
    m_qap1Header.setQAP1Header(cmd, length.ui32[0], 0, length.ui32[1]);
  }


  /** Constructor for NetworkManager to create Rpacket out of server response
   * @param[in] header QAP1 protocol RPacket header
   * @param[in] entries vector of RPacketEntry
   */
  RPacket::RPacket(const QAP1Header &header, const RVECTORTYPE<PacketEntry> &entries):m_qap1Header(header), m_vecEntrylist(RMAKE_SHARED<RVECTORTYPE<PacketEntry> >(entries)){}


  /** Retrieves shared pointer to vector of this packet's entries
   * @return RSHARED_PTR<PacketEntry> this packet's data section. see "rpacket_entry_0103.h"
   */
  RSHARED_PTR<const RVECTORTYPE<RPacket::PacketEntry> > RPacket::getEntries() const{
    return m_vecEntrylist;
  }

  /** Retrieves QAP1Header of the packet. see "qap1_header.h"
   * @return QAP1Header containing rserve cmd, packet length, and data offset
   */
  const QAP1Header RPacket::getHeader() const{
    return m_qap1Header;
  }

  /** Retrieves Rserve command from this packet's QAP1Header
   * @return enum representing rserve command. see "rpacket.h" for eCMD list
   */
  uint32_t RPacket::getCommand() const{
    return m_qap1Header.getCommand();
  }

  /** Retrieves status that was set in the response command. Only applicable to packets from server
   * @return String corresponding to stat codes. see "rpacket.h" for eStat list
   */
  RSTRINGTYPE RPacket::getStatus() const{
    if(isOk())
      return RSTRINGTYPE("OK");

    int error_code = (m_qap1Header.getCommand() >> 24) & 127;

    std::stringstream msg;
    // reserved for R errors.
    if(error_code >= 0 && error_code <= 0x3f){
      msg << "R encountered error ";
      msg << "0x" << std::hex << error_code << " while trying to execute function.";
      return RSTRINGTYPE(msg.str());
    }

    // RServe errors.
    msg << "RServe Error ";
    msg << "0x" << std::hex << error_code << ": ";
    switch(error_code){
    case ERR_auth_failed:
      {
        msg << "Authentication failed or was not attempted.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_conn_broken:
      {
        msg << "Connection closed or a broken packet killed it.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_inv_cmd:
      {
        msg << "Unsupported or invalid command.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_inv_par:
      {
        msg << "Invalid parameter exists.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_Rerror:
      {
        msg << "R-error occured.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_IOerror:
      {
        msg << "I/O error.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_notOpen:
      {
        msg << "Attempted to read/write a closed file.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_accessDenied:
      {
        msg << "RServe does not allow access to specified command.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_unsupportedCmd:
      {
        msg << "Unsupported command.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_unknownCmd:
      {
        msg << "Unrecognized command.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_data_overflow:
      {
        msg << "Incoming Packet is too big.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_object_too_big:
      {
        msg << "Requested object is too big to be sent.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_out_of_mem:
      {
        msg << "Out of memory.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_ctrl_closed:
      {
        msg << "Control pip to master process is closed/broken.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_session_busy:
      {
        msg << "Session is still busy.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_detach_failed:
      {
        msg << "Unable to detatch session.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_disabled:
      {
        msg << "Feature is disabled.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_unavailable:
      {
        msg << "Feature is not present in this build.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_cryptError:
      {
        msg << "Crypto-system error.";
        return RSTRINGTYPE(msg.str());
      }
    case ERR_securityClose:
      {
        msg << "Server initiated close due to security violation.";
        return RSTRINGTYPE(msg.str());
      }
    default:
      {
        msg << "An unknown error had occured.";
        return RSTRINGTYPE(msg.str());
      }
    }
  }

  /** checks packet cmd to see if command succeeded. Only applicable to packets from server
   * @return true if first bit is set to 1 (signifying command successful), false otherwise
   */
  bool RPacket::isOk() const{
    // check first bit, if it is set to 1, then response is okay
    return m_qap1Header.getCommand() & 0x1;
  }

  /** checks packet cmd to see if command succeeded. Only applicable to packets from server
   * @return true if second bit is set to 1 (signifying error), false otherwise
   */
  bool RPacket::isError() const{
    // check second bit, if it is set to 1, then there is an error
    return m_qap1Header.getCommand() & 0x2;
  }
}
