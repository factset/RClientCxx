/*  RPacketEntry 0103: Individual entry within an RPacket corresponding to RServe version 0103
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


#ifndef RCLIENT_RPACKET_ENTRY_0103_H_INCLUDED
#define RCLIENT_RPACKET_ENTRY_0103_H_INCLUDED

#include "config.h"
#include "rexp_null.h"

namespace rclient{

  /** An entry in the data section of an RPacket
   * Contains a 4 byte header:
   *  - 1 byte:  parameter type
   *  - 3 bytes: length of entry
   * Except if the 7th bit is set in the Type:
   * In which case, length of entry becomes 7 bytes and the total header is 8 bytes.
   */
  class RCLIENT_API RPacketEntry_0103{

  public:

    enum eDataType {
      DT_INT = 1,         // int
      DT_CHAR = 2,        // character
      DT_DOUBLE = 3,      // double
      DT_STRING = 4,      // null terminated string
      DT_BYTESTREAM = 5,  // stream of bytes (may contain null within stream)
      DT_SEXP = 10,       // encoded SEXP  (REXP)
      DT_ARRAY = 11,      // array of objects, first 4 bytes specify how many subsequent objects are part of the array. 0 is valid
      DT_CUSTOM = 32,     // custom type not defined in protocol but used in applications

      DT_TYPE_MASK = 63,   // MASK to show type without any flag
      DT_LARGE = 64       // FLAG: if set then the header is 8 bytes and the length of the entry is a 56-bit integer
    };
    
    
    RPacketEntry_0103();
    explicit RPacketEntry_0103(const REXP &expr);
    explicit RPacketEntry_0103(const RSTRINGTYPE &str);
    explicit RPacketEntry_0103(const RVECTORTYPE<unsigned char> &data); //copy data, used by NetworkManager

    // getters
    const RVECTORTYPE<unsigned char> & getEntry() const;
    const uint32_t getLength() const;
    const uint32_t getDataType() const;
    const uint32_t getHeaderLength() const;

    // Treat contents as REXP...
    RSHARED_PTR<const REXP> toREXP() const;

  private:
    RVECTORTYPE<unsigned char> m_aEntry;
    bool m_isLargeData;

    size_t makeEntryHeader(const size_t i, const uint32_t header_type, const size_t header_length);
    size_t makeREXPHeader(const size_t i, const uint32_t rexp_type, const size_t rexp_length);
    void fillWithNUL(size_t i);
  };

} // close namespace

#endif

