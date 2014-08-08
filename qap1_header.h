/*  QAP1Header: R Packet 16 byte header according to QAP1 protocol
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

#ifndef RCLIENT_QAP1HEADER_H_INCLUDED
#define RCLIENT_QAP1HEADER_H_INCLUDED

#include "config.h"
#include "endian_converter.h"
#include <inttypes.h>

namespace rclient{

  /** Header of an R Packet following QAP1 message oriented protocol
   */
  class RCLIENT_API QAP1Header{

  public:
    // constructors
    QAP1Header();

    QAP1Header(const uint32_t cmd,
	       const uint32_t len,
	       const uint32_t os = 0,
	       const uint32_t len_high = 0);

    QAP1Header(const RVECTORTYPE<uint8_t> &cmd,
	       const RVECTORTYPE<uint8_t> &len,
	       const RVECTORTYPE<uint8_t> &os,
	       const RVECTORTYPE<uint8_t> &len_high);

    QAP1Header(const RVECTORTYPE<uint8_t> &header);
  
    // destructor
    ~QAP1Header();


    // getters
    uint32_t getCommand() const;
    uint64_t getLength() const;
    uint32_t getOffset() const;
    uint32_t getLength_highbits() const;


    // network getters
    void getCommand(RVECTORTYPE<uint8_t> &buf, size_t &pos) const;
    void getLength(RVECTORTYPE<uint8_t> &buf, size_t &pos) const;
    void getOffset(RVECTORTYPE<uint8_t> &buf, size_t &pos) const;
    void getLength_highbits(RVECTORTYPE<uint8_t> &buf, size_t &pos) const;


    // setters
    void setQAP1Header(const RVECTORTYPE<uint8_t> &cmd,
		       const RVECTORTYPE<uint8_t> &len,
		       const RVECTORTYPE<uint8_t> &os,
		       const RVECTORTYPE<uint8_t> &len_high);

    void setQAP1Header(const uint32_t cmd,
		       const uint32_t len,
		       const uint32_t os = 0,
		       const uint32_t len_high = 0);

  private:
    EndianConverter m_converter;
    RVECTORTYPE<uint8_t> m_vCommand;
    RVECTORTYPE<uint8_t> m_vLength;
    RVECTORTYPE<uint8_t> m_vOffset;
    RVECTORTYPE<uint8_t> m_vLength_highbits;
  };
} // close namespace

#endif
