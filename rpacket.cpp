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
   * @return 32bit int corresponding to stat codes. see "rpacket.h" for eStat list
   */
  uint32_t RPacket::getStatus() const{
    return (m_qap1Header.getCommand() >> 24) & 127;
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
