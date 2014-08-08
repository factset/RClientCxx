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

#include "rpacket_entry_0103.h"
#include "rexp_class_hierarchy.h"
#include "endian_converter.h"

#include <string.h>
#include <stdexcept>

namespace{
  
  /** Copies data from the provided entry to the given buffer at position i
   * @param[out] buf vector of unsigned char to copy entry data to
   * @param[in] i position in buf to copy data
   * @param[in] entry RPacketEntry_0103 to copy data from 
   * @return number of bytes copied
   */
  size_t copyMember(RVECTORTYPE<unsigned char> &buf, const size_t i, const rclient::RPacketEntry_0103 &entry){
    RVECTORTYPE<unsigned char> entry_ch = entry.getEntry();
    size_t entry_length = entry.getLength();
    memcpy(&buf[i], &entry_ch[0], entry_length);
    return entry_length;
  }

  /** Serializes integer into little-endian byte order
   * @param[out] buf To be filled with the little-endian serialized version of val
   * @param[in] val Integer to be serialized
   */
  template <typename IntType>
    inline void serialize(RVECTORTYPE<uint8_t> &buf, const IntType val){
    size_t pos = 0;
    rclient::EndianConverter converter;
    converter.serialize<IntType>(buf, pos, val);
  }

  /** Deerializes little-endian byte order into integer
   * @param[out] buf containing serialized integer
   * @return Integer from deserialized buffer
   */
  template <typename IntType>
    inline IntType deserialize(const RVECTORTYPE<uint8_t> &buf){
    size_t pos = 0;
    rclient::EndianConverter converter;
    return converter.deserialize<IntType>(buf, pos);
  }
} // close namespace

namespace rclient{

  /** empty constructor
   */
  RPacketEntry_0103::RPacketEntry_0103():m_isLargeData(false){}


  /** fills in the header of the entry with the given information
   * @param[in] i Iterator for m_aEntry to fill in header
   * @param[in] type Entry datatype, see rpacket_enty.h for enum of datatypes
   * @param[in] length Byte length of the entry
   * @return size of header
   */
  size_t RPacketEntry_0103::makeEntryHeader(const size_t i, const uint32_t header_type, const size_t header_length){
    RVECTORTYPE<uint8_t> type, length;
    type.resize(sizeof(uint32_t));
    length.resize(sizeof(uint64_t));
    serialize<uint32_t>(type, header_type);
    serialize<uint64_t>(length, header_length);

    // if length exceeds 3 bytes, then need 8 byte large data header
    if(header_length > 0x7fffff){
      // resize entry to maximum header + rexp header + content
      m_aEntry.resize(header_length+8);
      m_isLargeData = true;
      // fill type and set flag for large data
      m_aEntry[i] = type[0] | DT_LARGE;
      // fill in length
      memcpy(&m_aEntry[i+1], &length[0], 7);
      return i+8;
    }
    // otherwise normal 4 byte header
    else{
      m_aEntry.resize(header_length+4);
      m_isLargeData = false;
      // fill in type
      m_aEntry[i] = type[0];
      // fill in length
      memcpy(&m_aEntry[i+1], &length[0], 3);
      return i+4;
    }
  }

  /** Creates REXP header at location i of entry with given type and length
   * @param[in] i iterator for m_aEntry to fill in header
   * @param[in] rexp_type type of REXP, see rexp.h for enum eType
   * @param[in] rexp_length length of the REXP data
   */
  size_t RPacketEntry_0103::makeREXPHeader(const size_t i, const uint32_t rexp_type, const size_t rexp_length){

    RVECTORTYPE<uint8_t> type, length;
    type.resize(sizeof(uint32_t));
    length.resize(sizeof(uint64_t));
    serialize<uint32_t>(type, rexp_type);
    serialize<uint64_t>(length, rexp_length);

    m_aEntry[i] = type[0];
    if(rexp_type & REXP::XT_LARGE){
      // 8 byte header
      memcpy(&m_aEntry[i+1], &length[0], 7);
      return i+8;
    }
    else{
      // 4 byte header
      memcpy(&m_aEntry[i+1], &length[0], 3);
      return i+4;
    }
  }
  

  /** constructor of entry for REXP.
   * eDataType is DT_SEXP
   * @param[in] exp REXP to convert into an RPacket data entry
   */
  RPacketEntry_0103::RPacketEntry_0103(const REXP &exp){
    size_t bytelength = exp.bytelength();
    // fill in entry header
    size_t i = makeEntryHeader(0, DT_SEXP, bytelength + (exp.getType() & REXP::XT_LARGE ? 8:4)) ; // iterator for array
    // fill in REXP Header
    i = makeREXPHeader(i, exp.getType(), bytelength);
    // fill in data for standard REXP types
    size_t len = m_aEntry.size()-i;
    bool isCopySuccess = exp.toNetworkData(&m_aEntry[i], len);
    if(!isCopySuccess)
      throw std::runtime_error("ERROR:: Failed to convert REXP to Network Data.");
    i+=bytelength;
  }

  /** constructor for string
   * eDataType is DT_STRING
   * @param[in] str string to convert into RPacket entry
   */
  RPacketEntry_0103::RPacketEntry_0103(const RSTRINGTYPE &str){
    size_t len = str.size() + 1;
    size_t align = (len%4 ? 4-len%4 : 0);
    // create header and retrieve iterator
    int i = makeEntryHeader(0,DT_STRING, len+align);
    // fill rest of data
    memcpy(&m_aEntry[i], str.c_str(), len);
    i+=len;
    // make sure entry is quadaligned
    memset(&m_aEntry[i], 0x1, m_aEntry.size()-i);
  }


  /** constructor for what would effectively be a cast. Copies contents of vector into own entry field.
   * Should only be used by network manager for receiving packets
   * @param data vector of unsigned chars holding contents of a valid RPacketEntry as defined by RServe
   */
  RPacketEntry_0103::RPacketEntry_0103(const RVECTORTYPE<unsigned char> &data):m_aEntry(data),m_isLargeData(data[0] & DT_LARGE){}


  /** Retrieves entry data prepared to be sent over the network
   * @return vector of unsigned chars containing headers and contents of the data entry
   */
  const RVECTORTYPE<unsigned char> & RPacketEntry_0103::getEntry() const{
    return m_aEntry;
  }

  /** retrieves number of bytes in the entry, including headers
   * @return number of bytes in the RPacketEntry data, including headers
   */
  const uint32_t RPacketEntry_0103::getLength() const{
    return m_aEntry.size();
  }

  /** retrieves eDataType value of the entry, see RPacketEntry class for corresponding enums
   * @return eDataType enum corresponding to entry data type
   */
  const uint32_t RPacketEntry_0103::getDataType() const{
    return m_aEntry[0];
  }

  /** Retrieves size of the entry header.
   * @return number of bytes in the entry header. 8 if the entry contains large data, otherwise 4
   */
  const uint32_t RPacketEntry_0103::getHeaderLength() const{
    return (m_isLargeData ? 8:4);
  }


  /** converts entry contents into the appropriate REXP
   * @return pointer to REXP contained in this packet OR REXPNull if packet is not an REXP
   */
  RSHARED_PTR<const REXP> RPacketEntry_0103::toREXP() const{
    // too small to be a rexp    // too small to be a rexp
    if (m_aEntry.size() < 8)
      return RMAKE_SHARED<REXPNull>();

    // first, confirm that this entry is a REXP
    RVECTORTYPE<uint8_t> serialized_rexp_length;
    serialized_rexp_length.resize(sizeof(uint64_t));
    uint32_t entry_type = m_aEntry[0];
    
    if((entry_type & DT_TYPE_MASK) != DT_SEXP){
      // entry is not an REXP
      return RMAKE_SHARED<REXPNull>();
    }

    uint32_t rexp_pos = (m_isLargeData ? 8:4);

    // determine type of REXP
    uint32_t rexp_type = m_aEntry[rexp_pos];

    uint32_t rexp_header_length = (rexp_type & REXP::XT_LARGE ? 8 : 4);
    uint32_t rexp_sizeof_length = rexp_header_length - 1;

    // determine length of REXP
    memcpy(&serialized_rexp_length[0], &m_aEntry[rexp_pos + 1], rexp_sizeof_length);
    memset(&serialized_rexp_length[rexp_sizeof_length], 0, sizeof(uint64_t) - rexp_sizeof_length);
    uint64_t rexp_length = deserialize<uint64_t>(serialized_rexp_length);

    // attribute information
    uint64_t attr_length = 0;;
    uint32_t attr_type = 0;
    uint32_t attr_header_length = 0;

    // if there is an attribute, skip over it (Attributes not implemented)
    if(rexp_type & REXP::XT_HAS_ATTR){
      RVECTORTYPE<uint8_t>  serialized_attr_length;
      serialized_attr_length.resize(sizeof(uint64_t));

      uint32_t attr_pos = rexp_pos + rexp_header_length;
      
      // determine type of attribute (should be XT_LIST_TAG)
      attr_type = m_aEntry[attr_pos];

      attr_header_length = (attr_type & REXP::XT_LARGE ? 8 : 4);
      uint32_t attr_sizeof_length = rexp_header_length - 1;

      // determine length of REXP
      memcpy(&serialized_attr_length[0], &m_aEntry[attr_pos + 1], attr_sizeof_length);
      memset(&serialized_attr_length[attr_sizeof_length], 0, sizeof(uint64_t) - attr_sizeof_length);

      attr_length = deserialize<uint64_t>(serialized_attr_length);
    }

    uint32_t offset = rexp_pos + rexp_header_length;

    if(attr_type){
      rexp_length -= attr_length + attr_header_length;
      offset += attr_length + attr_header_length;
    }


    // create appropriate REXP
    switch (rexp_type & REXP::XT_TYPE_MASK){

    case REXP::XT_INT:
    case REXP::XT_ARRAY_INT:
      {
	RVECTORTYPE<int32_t> data;
	RVECTORTYPE<uint8_t> data_point;
	data_point.resize(sizeof(uint32_t));
	for(uint64_t i=0; i<rexp_length; i+=sizeof(int32_t)){
	  memcpy(&data_point[0], &m_aEntry[i + offset], sizeof(uint32_t));
	  uint32_t val = deserialize<uint32_t>(data_point);
	  data.push_back(val);
	}
	return RMAKE_SHARED<REXPInteger>(data);
      }

    case REXP::XT_DOUBLE:
    case REXP::XT_ARRAY_DOUBLE:
      // create vector
      {
	EndianConverter converter;
	RVECTORTYPE<double> data;
	for(uint64_t i=0; i<rexp_length; i+=sizeof(double)){
	  double val = converter.swap_endian(*(double*) &m_aEntry[i+offset]);
	  data.push_back(val);
	}
	return RMAKE_SHARED<REXPDouble>(data);
      }
    case REXP::XT_STR:
    case REXP::XT_ARRAY_STR:
      {
	//create vector
	RVECTORTYPE<RSTRINGTYPE> data;
	uint64_t i = 0;

	if(m_aEntry[rexp_length+offset-1] > 0x1){
	  // end of REXP is not NUL or SOH, cannot interpret as string
	  return RMAKE_SHARED<REXPNull>();
	}

	while(i < rexp_length){
	  if (m_aEntry[i+offset] == 0x1){
	    // skip over SOH as it is buffer for the end of packet
	    ++i;
	  }
	  else{
	    // create string
	    RSTRINGTYPE str((const char*) &m_aEntry[i+offset]);
	    uint64_t len = str.size()+1;
	    i += len;
	    data.push_back(str);
	  }
	}
	return RMAKE_SHARED<REXPString>(data);
      }
    default:
      return RMAKE_SHARED<REXPNull>();
    }
  }

} // close namespace
