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

#ifndef RCLIENT_RPACKET_H_INCLUDED
#define RCLIENT_RPACKET_H_INCLUDED

#include "config.h"
#include "qap1_header.h"
// protocol version
#include "rpacket_entry_0103.h"

#include <inttypes.h>

namespace rclient{
    
  /** R Packet corresponding to QAP1 message oriented protocol
   * Contains 16 byte header consisting of 4 ints
   * [0]  command: specifies request or response type
   * [4]  length: specifies number of bytes belonging to this message
   * [8]  offset: specifies the offset of the data, where 0 means directly after the header
   * [12] length: high bits of the length (0 if packet size is smaller than 4GB) only used if length of message is greater than 4 bytes
   * Each entry in the data section is preceeded by 4 bytes: see RPacketEntry
   */
  class RCLIENT_API RPacket{

  public:
    typedef RPacketEntry_0103 PacketEntry;

    // enum corresponding to server commands. Used within packet header.
    enum eCMD {
      CMD_login      = 0x001,
      CMD_voideval   = 0x002,
      CMD_eval       = 0x003,
      CMD_shutdown   = 0x004,

      CMD_openFile   = 0x010,
      CMD_createFile = 0x011,
      CMD_closefile  = 0x012,
      CMD_readfile   = 0x013,
      CMD_writefile  = 0x014,
      CMD_removeFile = 0x015,

      CMD_setSEXP    = 0x020,
      CMD_assignSEXP = 0x021,

      CMD_detachSession     = 0x030,
      CMD_detatchedVoidEval = 0x031,
      CMD_attachSession     = 0x032,

      CMD_setBufferSize = 0x081,
      CMD_setEncoding   = 0x082,

      // Rserve Commands Since 0.6
      CMD_ctrlEval     = 0x42,
      CMD_ctrlShutdown = 0x44,
      CMD_ctrlSource   = 0x45,

      // Rserve Commands Since 1.7
      CMD_switch   = 0x005,
      CMD_keyReq   = 0x006,
      CMD_secLogin = 0x007,
      CMD_OCcall   = 0x00f
    };

    /* enum corresponding to status codes of errors sent in command response from server.
       
       stat codes; 0-0x3f are reserved for program specific codes - e.g. for R
       connection they correspond to the stat of Parse command.
       the following codes are returned by the Rserv itself
       
       codes <0 denote Rerror as provided by R_tryEval
    */
    enum eStat {
      ERR_auth_failed    = 0x41, // authentication failed
      ERR_conn_broken    = 0x42, // connection closed or broken packet killed it
      ERR_inv_cmd        = 0x43, // unsupported/invalid command
      ERR_inv_par        = 0x44, // invalid parameter exists
      ERR_Rerror         = 0x45, // R-error occured, usually followed by closed connection
      ERR_IOerror        = 0x46, // I/O error
      ERR_notOpen        = 0x47, // attempted to read/write a closed file
      ERR_accessDenied   = 0x48, // server doesn't allow access to specified command
      ERR_unsupportedCmd = 0x49, // unsupported command
      ERR_unknownCmd     = 0x4a, // unrecognized command

      // ERR Codes since 1.23/0.1-6
      ERR_data_overflow  = 0x4b, // incoming packet is too big
      ERR_object_too_big = 0x4c, // requested object is too big to be sent
    
      // ERR Codes since 1.29/0.1-9
      ERR_out_of_mem     = 0x4d, // out of memory, connection usually closed after sending this
    
      // ERR Codes since 0.6-0
      ERR_ctrl_closed    = 0x4e, // control pipe to master process is closed/broken

      // ERR Codes since 0.4-0
      ERR_session_busy   = 0x50, // session is still busy
      ERR_detach_failed  = 0x51, // unable to detach session

      // ERR Codes since 1.7
      ERR_disabled       = 0x61, // feature is disabled
      ERR_unavailable    = 0x62, // feature is not present in this build
      ERR_cryptError     = 0x63, // crypto-system error
      ERR_securityClose  = 0x64  // server-initialized close due to security violation
    };

    // constructor for consumer
    RPacket(const eCMD &cmd, const RVECTORTYPE<PacketEntry> &entries);

    // constructor for network: will not want consumer using this one. make private and friend or something
    RPacket(const QAP1Header &header, const RVECTORTYPE<PacketEntry> &entries);

    uint32_t getCommand() const;
    RSTRINGTYPE getStatus() const;

    // getters
    RSHARED_PTR<const RVECTORTYPE<PacketEntry> > getEntries() const;
    const QAP1Header getHeader() const;

    // improvement: create enum for triboolean
    bool isOk() const;
    bool isError() const;

  private:
    QAP1Header m_qap1Header;

    RSHARED_PTR<RVECTORTYPE<PacketEntry> > m_vecEntrylist;
  };
}
#endif

