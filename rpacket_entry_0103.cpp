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


  /** fills in the header of the entry with the given information
   * @param[out] entry vector of unsigned char to fill with header
   * @param[out] isLargeData bool to that is set if large header is used
   * @param[in] i Iterator for m_aEntry to fill in header
   * @param[in] type Entry datatype, see rpacket_enty.h for enum of datatypes
   * @param[in] length Byte length of the entry
   * @return size of header
   */
  size_t makeEntryHeader(RVECTORTYPE<unsigned char> &entry, bool &isLargeData, const size_t i, const uint32_t header_type, const uint64_t header_length){

    RVECTORTYPE<uint8_t> type, length;
    type.resize(sizeof(uint32_t));
    length.resize(sizeof(uint64_t));
    serialize<uint32_t>(type, header_type);
    serialize<uint64_t>(length, header_length);

    // if length exceeds 3 bytes, then need 8 byte large data header
    if(header_length > 0x7fffff){
      // resize entry to maximum header + rexp header + content
      entry.resize(header_length+8);
      isLargeData = true;
      // fill type and set flag for large data
      entry[i] = type[0] | rclient::RPacketEntry_0103::DT_LARGE;
      // fill in length
      memcpy(&entry[i+1], &length[0], 7);
      return i+8;
    }
    // otherwise normal 4 byte header
    else{
      entry.resize(header_length+4);
      isLargeData = false;
      // fill in type
      entry[i] = type[0];
      // fill in length
      memcpy(&entry[i+1], &length[0], 3);
      return i+4;
    }
  }


  /** Creates REXP header at location i of entry with given type and length
   * @param[out] entry vector of unsigned char to fill with REXP header
   * @param[in] i iterator for entry to fill in header
   * @param[in] rexp_type type of REXP, see rexp.h for enum eType
   * @param[in] rexp_length length of the REXP data
   */
  size_t makeREXPHeader(RVECTORTYPE<unsigned char> &entry, const size_t i, const uint32_t rexp_type, const uint64_t rexp_length){

    RVECTORTYPE<uint8_t> type, length;
    type.resize(sizeof(uint32_t));
    length.resize(sizeof(uint64_t));
    serialize<uint32_t>(type, rexp_type);
    serialize<uint64_t>(length, rexp_length);

    entry[i] = type[0];

    // remove attr flag if IncludeAttributes is not set
    if(!rclient::IncludeAttributes)
      entry[i] = entry[i] & ~rclient::REXP::XT_HAS_ATTR;

    if(rexp_type & rclient::REXP::XT_LARGE){
      // 8 byte header
      memcpy(&entry[i+1], &length[0], 7);
      return i+8;
    }
    else{
      // 4 byte header
      memcpy(&entry[i+1], &length[0], 3);
      return i+4;
    }
  }


  /** Creates REXP (including header) at location i of entry.
   * @param[out] entry vector of unsigned char to fill with REXP header
   * @param[in] i iterator for entry to fill in header
   * @param[in] rexp_type type of REXP, see rexp.h for enum eType
   * @param[in] rexp_length length of the REXP data
   */
  size_t fillREXP(RVECTORTYPE<unsigned char> &entry, const rclient::REXP &exp, size_t i){
    size_t bytelength = exp.bytelength();
    if(rclient::IncludeAttributes && exp.hasAttributes()){
      bytelength += exp.getAttributes()->bytelength() + (exp.getAttributes()->getType() & rclient::REXP::XT_LARGE ? 8:4);
    }
    // fill in REXP Header
    i = makeREXPHeader(entry, i, exp.getType(), bytelength);

    // fill in attributes if there are any
    if(rclient::IncludeAttributes && exp.hasAttributes()){
      i = fillREXP(entry, *exp.getAttributes(), i);
    }

    // special condition for REXPPairList, fill in each pair individually
    if(exp.getBaseType() == rclient::REXP::XT_LIST_TAG || exp.getBaseType() == rclient::REXP::XT_LANG_TAG){
      const rclient::REXPPairList::RPairVector data = dynamic_cast<const rclient::REXPPairList &>(exp).getData();

      for(size_t j = 0; j < data.size(); ++j){
        // fill val (REXP)
        i = fillREXP(entry, *data[j].first, i);

        // fill tag (string)
        size_t str_len = data[j].second.size()+1;
        size_t aligned_len = str_len + (str_len%4 ? (4-str_len%4) : 0);
        uint32_t str_type = rclient::REXP::XT_SYMNAME | (aligned_len > 0x7fffff ? rclient::REXP::XT_LARGE : 0);
        i = makeREXPHeader(entry, i, str_type, aligned_len);
        memcpy(&entry[i], data[j].second.c_str(), str_len);
        i += str_len;
        // align string
        if(i%4){
          memset(&entry[i], 0, 4-i%4);
          i += 4-i%4;
        }
      }
    }

    // special condition for REXPList, fill in each REXP individually
    else if(exp.getBaseType() == rclient::REXP::XT_LIST_NOTAG || exp.getBaseType() == rclient::REXP::XT_LANG_NOTAG){
      const rclient::REXPList::RVector data = dynamic_cast<const rclient::REXPList &>(exp).getData();
      for(size_t j = 0; j < data.size(); ++j){
        // fill REXP
        i = fillREXP(entry, *data[j], i);
      }
    }
    // fill in data for standard REXP types
    else{
      uint64_t len = entry.size()-i;
      bool isCopySuccess = exp.toNetworkData(&entry[i], len);
      if(!isCopySuccess)
        throw std::runtime_error("ERROR:: Failed to convert REXP to Network Data.");
      i+=bytelength;
    }
    return i;
  }

  /** Parses data from entry into a REXP
   * @param[in] entry vector of unsigned char containing a REXP at the given offset
   * @param[in] offset position in entry to parse REXP
   * @return shared pointer to a REXP created from parsing entry
   */
  RSHARED_PTR<const rclient::REXP> parseREXP(const RVECTORTYPE<unsigned char> &entry, const size_t rexp_pos){

    // determine type of REXP
    uint32_t rexp_type = entry[rexp_pos];

    RVECTORTYPE<uint8_t> serialized_rexp_length;
    serialized_rexp_length.resize(sizeof(uint64_t));

    uint32_t rexp_header_length = (rexp_type & rclient::REXP::XT_LARGE ? 8 : 4);
    uint32_t rexp_sizeof_length = rexp_header_length - 1;

    // determine length of REXP
    memcpy(&serialized_rexp_length[0], &entry[rexp_pos + 1], rexp_sizeof_length);
    memset(&serialized_rexp_length[rexp_sizeof_length], 0, sizeof(uint64_t) - rexp_sizeof_length);
    uint64_t rexp_length = deserialize<uint64_t>(serialized_rexp_length);

    uint64_t offset = rexp_pos + rexp_header_length;

    // attribute information
    bool hasAttr = rexp_type & rclient::REXP::XT_HAS_ATTR;
    uint64_t attr_length = 0;
    uint32_t attr_type = 0;
    uint32_t attr_header_length = 0;
    RSHARED_PTR<const rclient::REXPPairList> attribute;

    // if there is an attribute, skip over it (Attributes not implemented)
    if(hasAttr){

      RVECTORTYPE<uint8_t>  serialized_attr_length;
      serialized_attr_length.resize(sizeof(uint64_t));
      
      uint32_t attr_pos = rexp_pos + rexp_header_length;
      
      // determine type of attribute (should be XT_LIST_TAG)
      attr_type = entry[attr_pos];

      attr_header_length = (attr_type & rclient::REXP::XT_LARGE ? 8 : 4);
      uint32_t attr_sizeof_length = rexp_header_length - 1;

      // determine length of REXP
      memcpy(&serialized_attr_length[0], &entry[attr_pos + 1], attr_sizeof_length);
      memset(&serialized_attr_length[attr_sizeof_length], 0, sizeof(uint64_t) - attr_sizeof_length);

      attr_length = deserialize<uint64_t>(serialized_attr_length);

      if(rclient::IncludeAttributes){
        // parse the attribute

        attribute = RPTR_CAST<const rclient::REXPPairList>(parseREXP(entry,attr_pos));

        if(attribute == 0)
          // failed to parse attribute.
          hasAttr = false;
      }
      else{
        // create a REXP without the attributes
        hasAttr = false;
      }

      // adjust length and offset to read after the attribute.
      rexp_length -= attr_length + attr_header_length;
      offset += attr_length + attr_header_length;
    }

    // create appropriate REXP
    switch (rexp_type & rclient::REXP::XT_TYPE_MASK){

    case rclient::REXP::XT_INT:
    case rclient::REXP::XT_ARRAY_INT:
      {
	RVECTORTYPE<int32_t> data;
	RVECTORTYPE<uint8_t> data_point;
	data_point.resize(sizeof(uint32_t));
	for(uint64_t i=0; i<rexp_length; i+=sizeof(int32_t)){
	  memcpy(&data_point[0], &entry[i + offset], sizeof(uint32_t));
	  uint32_t val = deserialize<uint32_t>(data_point);
	  data.push_back(val);
	}
        if(hasAttr)
          return RMAKE_SHARED<rclient::REXPInteger>(data, attribute);
        else
          return RMAKE_SHARED<rclient::REXPInteger>(data);
      }

    case rclient::REXP::XT_DOUBLE:
    case rclient::REXP::XT_ARRAY_DOUBLE:
      // create vector
      {
	rclient::EndianConverter converter;
	RVECTORTYPE<double> data;
	for(uint64_t i=0; i<rexp_length; i+=sizeof(double)){
	  double val = converter.swap_endian(*(double*) &entry[i+offset]);
	  data.push_back(val);
	}
        if(hasAttr)
          return RMAKE_SHARED<rclient::REXPDouble>(data, attribute);
        else
          return RMAKE_SHARED<rclient::REXPDouble>(data);
      }
    case rclient::REXP::XT_STR:
    case rclient::REXP::XT_ARRAY_STR:
      {
	//create vector
	RVECTORTYPE<RSTRINGTYPE> data;
	uint64_t i = 0;

	if(entry[rexp_length+offset-1] > 0x1){
	  // end of REXP is not NUL or SOH, cannot safely interpret as string
	  return RMAKE_SHARED<rclient::REXPNull>();
	}

	while(i < rexp_length){
	  if (entry[i+offset] == 0x1){
	    // skip over SOH as it is buffer for the end of packet
	    ++i;
	  }
	  else{
	    // create string
	    RSTRINGTYPE str((const char*) &entry[i+offset]);
	    uint64_t len = str.size()+1;
	    i += len;
	    data.push_back(str);
	  }
	}
        if(hasAttr)
          return RMAKE_SHARED<rclient::REXPString>(data, attribute);
        else
          return RMAKE_SHARED<rclient::REXPString>(data);
      }

    case rclient::REXP::XT_LIST_TAG:
    case rclient::REXP::XT_LANG_TAG:
      {
        // create vector
        rclient::REXPPairList::RPairVector data;
        RVECTORTYPE<RSTRINGTYPE> tag_data;
        RVECTORTYPE<RSHARED_PTR<const rclient::REXP> > val_data;

        size_t i = 0;
        while(i < rexp_length){
          // parse rexp
          RSHARED_PTR<const rclient::REXP> exp = parseREXP(entry, i + offset);
          i += exp->bytelength() + (exp->getType() & rclient::REXP::XT_LARGE ? 8:4);
          
          // invalid PairList if not second member is not a string
          if(entry[i+offset] != rclient::REXP::XT_SYMNAME)
            return RMAKE_SHARED<rclient::REXPNull>();

          // parse string header
          uint32_t sizeof_str_header = (entry[i+offset] & rclient::REXP::XT_LARGE ? 8 : 4);
          RVECTORTYPE<uint8_t>  serialized_str_length;
          serialized_str_length.resize(sizeof(uint64_t));
          memcpy(&serialized_str_length[0], &entry[i+offset+1], sizeof_str_header-1);
          memset(&serialized_str_length[sizeof_str_header], 0, sizeof(uint64_t) - sizeof_str_header-1);
          uint64_t str_len = deserialize<uint64_t>(serialized_str_length);
          
          i += sizeof_str_header;

          // return REXPNull if string is not guaranteed to terminate
          if(entry[i+offset+str_len-1] > 0x1)
            // end of rexp is not NUL or SOH: invalid string. Cannot safely interpret as REXPPairList
            return RMAKE_SHARED<rclient::REXPNull>();
          
          // create string
          RSTRINGTYPE name((char*) &entry[i+offset]);
          i += str_len;
          
          // create and store pair
          rclient::REXPPairList::RPair rpair(exp,name);
          data.push_back(rpair);
          }
        if(hasAttr)
          return RMAKE_SHARED<rclient::REXPPairList>(data, attribute);
        else
          return RMAKE_SHARED<rclient::REXPPairList>(data);
      }

    case rclient::REXP::XT_LIST_NOTAG:
    case rclient::REXP::XT_LANG_NOTAG:
      {
        rclient::REXPList::RVector data;
        size_t i = 0;
        while(i < rexp_length){
          RSHARED_PTR<const rclient::REXP> exp = parseREXP(entry, i + offset);
          data.push_back(exp);
          i += exp->bytelength() + (exp->getType() & rclient::REXP::XT_LARGE ? 8:4);
        }
        if(hasAttr)
          return RMAKE_SHARED<rclient::REXPList>(data, attribute);
        else
          return RMAKE_SHARED<rclient::REXPList>(data);
      }

    default:
      if(hasAttr)
        return RMAKE_SHARED<rclient::REXPNull>(attribute);
      else
        return RMAKE_SHARED<rclient::REXPNull>();
      
    }
  }
  

} // close namespace

namespace rclient{

  /** empty constructor
   */
  RPacketEntry_0103::RPacketEntry_0103():m_isLargeData(false){}

  /** constructor of entry for REXP.
   * eDataType is DT_SEXP
   * @param[in] exp REXP to convert into an RPacket data entry
   */
  RPacketEntry_0103::RPacketEntry_0103(const REXP &exp){
    size_t bytelength = exp.bytelength() + (exp.getType() & REXP::XT_LARGE ? 8:4);
    if(IncludeAttributes && exp.hasAttributes()){
      bytelength += exp.getAttributes()->bytelength() + (exp.getAttributes()->getType() & REXP::XT_LARGE ? 8:4);
    }
    // fill in entry header
    size_t i = makeEntryHeader(m_aEntry, m_isLargeData, 0, DT_SEXP, bytelength);
    // fill in rexp
    i = fillREXP(m_aEntry, exp, i);
  }

  /** constructor of entry for string
   * eDataType is DT_STRING
   * @param[in] str string to convert into RPacket entry
   */
  RPacketEntry_0103::RPacketEntry_0103(const RSTRINGTYPE &str){
    size_t len = str.size() + 1;
    size_t align = (len%4 ? 4-len%4 : 0);
    // create header and retrieve iterator
    int i = makeEntryHeader(m_aEntry, m_isLargeData, 0, DT_STRING, len+align);
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
    // too small to be a rexp
    if (m_aEntry.size() < 8)
      return RMAKE_SHARED<REXPNull>();

    // first, confirm that this entry is a REXP
    uint32_t entry_type = m_aEntry[0];
    
    if((entry_type & DT_TYPE_MASK) != DT_SEXP){
      // entry is not a REXP
      return RMAKE_SHARED<REXPNull>();
    }

    return parseREXP(m_aEntry, (m_isLargeData ? 8:4));
  }

} // close namespace
