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

#include "qap1_header.h"
#include <string.h> // for memcpy

namespace rclient{

  /** destructor
   */
  QAP1Header::~QAP1Header(){}

  /** Constructor takes native-endian unsigned integers to
   * @param[in] cmd Rserve command. see RPacket class for eCMD enum
   * @param[in] len low bits of packet length
   * @param[in] os offset of the data in the packet. default (and usually) 0
   * @param[in] len_high high bits of packet length. default 0. only used if packet length exceeds 4GB
   */
  QAP1Header::QAP1Header(const uint32_t cmd, const uint32_t len, const uint32_t os, const uint32_t len_high){
    m_vCommand.resize(sizeof(uint32_t));
    m_vLength.resize(sizeof(uint32_t));
    m_vOffset.resize(sizeof(uint32_t));
    m_vLength_highbits.resize(sizeof(uint32_t));

    size_t pos = 0;
    m_converter.serialize<uint32_t>(m_vCommand, pos, cmd);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vLength, pos, len);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vOffset, pos, os);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vLength_highbits, pos, len_high);
  }


  /** Constructor takes little-endian representations of unsigned integers
   * @param[in] cmd Rserve command. see RPacket class for eCMD enum
   * @param[in] len low bits of packet length
   * @param[in] os offset of the data in the packet. default (and usually) 0
   * @param[in] len_high high bits of packet length. default 0. only used if packet length exceeds 4GB
   */
  QAP1Header::QAP1Header(const RVECTORTYPE<uint8_t> &cmd, const RVECTORTYPE<uint8_t> &len, const RVECTORTYPE<uint8_t> &os, const RVECTORTYPE<uint8_t> &len_high){
    m_vCommand.resize(sizeof(uint32_t));
    m_vLength.resize(sizeof(uint32_t));
    m_vOffset.resize(sizeof(uint32_t));
    m_vLength_highbits.resize(sizeof(uint32_t));

    memcpy(&m_vCommand[0], &cmd[0], sizeof(uint32_t));
    memcpy(&m_vLength[0], &len[0], sizeof(uint32_t));
    memcpy(&m_vOffset[0], &os[0], sizeof(uint32_t));
    memcpy(&m_vLength_highbits[0], &len_high[0], sizeof(uint32_t));
  }


  /** Constructor takes all header data in one vector
   * @param[in] data little-endian representation of command, length, offset, and length_highbits
   */
  QAP1Header::QAP1Header(const RVECTORTYPE<uint8_t> &data){
    m_vCommand.resize(sizeof(uint32_t));
    m_vLength.resize(sizeof(uint32_t));
    m_vOffset.resize(sizeof(uint32_t));
    m_vLength_highbits.resize(sizeof(uint32_t));

    memcpy(&m_vCommand[0], &data[0], sizeof(uint32_t));
    memcpy(&m_vLength[0], &data[4], sizeof(uint32_t));
    memcpy(&m_vOffset[0], &data[8], sizeof(uint32_t));
    memcpy(&m_vLength_highbits[0], &data[12], sizeof(uint32_t));
  }

  /** empty constructor sets all values to 0
   */
  QAP1Header::QAP1Header(){
    m_vCommand.resize(sizeof(uint32_t));
    m_vLength.resize(sizeof(uint32_t));
    m_vOffset.resize(sizeof(uint32_t));
    m_vLength_highbits.resize(sizeof(uint32_t));

    size_t pos = 0;
    m_converter.serialize<uint32_t>(m_vCommand, pos, 0);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vLength, pos, 0);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vOffset, pos, 0);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vLength_highbits, pos, 0);
  }



  /** assigns QAP1Header values without length high bits. length high bits will be set to 0
   * @param[in] cmd Rserve command. see RPacket class for eCMD enum
   * @param[in] len low bits of packet length
   * @param[in] os offset of the data in the packet. default (and usually) 0
   * @param[in] len_high high bits of packet length. default 0. only used if packet length exceeds 4GB
   */
  void QAP1Header::setQAP1Header(const uint32_t cmd, const uint32_t len, const uint32_t os, const uint32_t len_high){
    size_t pos = 0;
    m_converter.serialize<uint32_t>(m_vCommand, pos, cmd);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vLength, pos, len);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vOffset, pos, os);
    pos = 0;
    m_converter.serialize<uint32_t>(m_vLength_highbits, pos, len_high);
  }


  /** getter for m_vCommand's native-endian value
   * @return unsigned 32bit int representing Rserve command. see RPacket class for eCMD list
   */
  uint32_t QAP1Header::getCommand() const{
    size_t pos = 0;
    return m_converter.deserialize<uint32_t>(m_vCommand, pos);
  }

  /** getter for litte-endian representation of m_vCommand
   * fills buf at pos with contents
   * @param[out] buf vector to fill with little-endian representation of m_vCommand
   * @param[in,out] pos position in vector to fill with data
   */
  void QAP1Header::getCommand(RVECTORTYPE<uint8_t> &buf, size_t &pos) const{
    memcpy(&buf[pos], &m_vCommand[0], sizeof(uint32_t));
  }

  /** getter for m_uiLength
   * @return unsigned 64bit int representing packet length
   */
  uint64_t QAP1Header::getLength() const{
    size_t pos = 0;
    RVECTORTYPE<uint8_t> ret;
    ret.resize(sizeof(uint64_t));
    memcpy(&ret[0], &m_vLength[0], sizeof(uint32_t));
    memcpy(&ret[4], &m_vLength_highbits[0], sizeof(uint32_t));
    return m_converter.deserialize<uint64_t>(ret, pos);
  }

  /** getter for litte-endian representation of m_vLength
   * fills buf at pos with contents
   * @param[out] buf vector to fill with little-endian representation of m_vLength
   * @param[in,out] pos position in vector to fill with data
   */
  void QAP1Header::getLength(RVECTORTYPE<uint8_t> &buf, size_t &pos) const{
    memcpy(&buf[pos], &m_vLength[0], sizeof(uint32_t));
  }

  /** getter for m_uiOffset
   * @return unsigned 32bit int representing offset of packet data from packet header
   */
  uint32_t QAP1Header::getOffset() const{
    size_t pos = 0;
    return m_converter.deserialize<uint32_t>(m_vOffset, pos);
  }

  /** getter for litte-endian representation of m_vOffset
   * fills buf at pos with contents
   * @param[out] buf vector to fill with little-endian representation of m_vOffset
   * @param[in,out] pos position in vector to fill with data
   */
  void QAP1Header::getOffset(RVECTORTYPE<uint8_t> &buf, size_t &pos) const{
    memcpy(&buf[pos], &m_vOffset[0], sizeof(uint32_t));
  }

  /** getter for m_uiLength_highbits
   * does not return 64bit length because the order to combine the lengths depends on the endian that they were stored
   * @return unsigned 32bit int representing high bits of packet length
   */
  uint32_t QAP1Header::getLength_highbits() const{
    size_t pos = 0;
    return m_converter.deserialize<uint32_t>(m_vLength_highbits, pos);
  }

  /** getter for litte-endian representation of m_vLength_highbits
   * fills buf at pos with contents
   * @param[out] buf vector to fill with little-endian representation of m_vLength_highbits
   * @param[in,out] pos position in vector to fill with data
   */
  void QAP1Header::getLength_highbits(RVECTORTYPE<uint8_t> &buf, size_t &pos) const{
    memcpy(&buf[pos], &m_vLength_highbits[0], sizeof(uint32_t));
  }

} // close namespace
