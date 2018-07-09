/*  REXP String: R Object Containing vector of strings
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

#include "rexp_string.h"

#include <string.h>

namespace{

  /** Calculates number of bytes in provided vector of strings
   * This is not included in REXPString so that is may be used in REXPString's constructor
   * @param stringlist vector of strings to determine bytelength of
   * @return number of bytes that are in stringlist
   */
  size_t getBytelength(const RVECTORTYPE<RSTRINGTYPE> &stringlist){
    size_t size = 0;
    for(size_t i = 0; i < stringlist.size(); ++i){
      size += stringlist[i].size() + 1;
    }
    // quadalign string data
    size += (size%4 ? 4-size%4:0);
    return size;
  }

  // character pattern for R representation of NA
  const unsigned char na_ch[2] = {255,0};
}


namespace rclient{

  // R string representation for NA
  const RSTRINGTYPE REXPString::NA((char*) na_ch);


  /** Initializes vector of strings with conversion from consumer's NA representation to R's NA representation
   * @param[in] vals vector of strings to copy into REXP
   * @param[in] consumerNAValue NA representation for strings used by the consumer
   */
  void REXPString::initData(const RVECTORTYPE<RSTRINGTYPE> &vals, const RSTRINGTYPE &consumerNAValue){
    m_vecData.reserve(vals.size());
    for(size_t i = 0; i < vals.size(); ++i){
      if (vals[i] == consumerNAValue)
	m_vecData.push_back(getNARepresentation());
      else
	m_vecData.push_back(vals[i]);
    }
  }

  /** basic constructor initializes an empty vector
   */
  REXPString::REXPString():REXPVector(XT_ARRAY_STR),m_vecData(0){}

  /** destructor
   */
  REXPString::~REXPString(){}

  /** Copy constructor.
   * @param[in] exp REXPString to copy data from
   */
  REXPString::REXPString(const REXPString &exp):REXPVector(XT_ARRAY_STR, getBytelength(exp.getData())), m_vecData(exp.getData()){
    if(exp.hasAttributes())
      REXP::setAttributes(exp.getAttributes());
  }

  /** constructor takes 1 string and puts it into a vector of size 1
   * @param[in] str String to populate m_vecData, REXPString's contents
   * @param[in] consumerNAValue NA representation for strings used by the consumer
   */
  REXPString::REXPString(const RSTRINGTYPE &str, const RSTRINGTYPE &consumerNAValue):REXPVector(XT_ARRAY_STR, str.size()){
    m_vecData.resize(1);
    if (str == consumerNAValue)
      m_vecData[0] = getNARepresentation();
    else
      m_vecData[0]= str;
  }

  /** constructor copies provided vector<RSTRINGTYPE> into its data
   * @param[in] strVec vector of strings to copy into m_vecData, REXPString's contents
   * @param[in] consumerNAValue NA representation for strings used by the consumer
   */
  REXPString::REXPString(const RVECTORTYPE<RSTRINGTYPE> &strVec, const RSTRINGTYPE &consumerNAValue):REXPVector(XT_ARRAY_STR, getBytelength(strVec)){
    initData(strVec, consumerNAValue);
  }

  /** constructor copies provided vector<RSTRINGTYPE> into its data. Constructor for REXP with attributes
   * @param[in] strVec vector of strings to copy into m_vecData, REXPString's contents
   * @param[in] consumerNAValue NA representation for strings used by the consumer
   */
  REXPString::REXPString(const RVECTORTYPE<RSTRINGTYPE> &strVec, const RSHARED_PTR<const REXPPairList> &attr, const RSTRINGTYPE &consumerNAValue):REXPVector(attr, XT_ARRAY_STR, getBytelength(strVec)){
    initData(strVec, consumerNAValue);
  }

  /** Retrieve the size of REXPString's data - vector of strings (m_vecData)
   * @return length of vector m_vecData
   */
  size_t REXPString::length() const{
    return m_vecData.size();
  }

  
  /** Retrive contents of REXPString's data - vector of strings (m_vecData)
   * @param[in] consumerNAValue NA representation for strings used by the consumer
   * @return vector of strings, m_vecData
   */
  RVECTORTYPE<RSTRINGTYPE> REXPString::getData(const RSTRINGTYPE &consumerNAValue) const{
    RVECTORTYPE<RSTRINGTYPE> retval;
    retval.reserve(m_vecData.size());
    for(size_t i = 0; i < m_vecData.size(); ++i){
      if (m_vecData[i] == getNARepresentation())
	retval.push_back(consumerNAValue);
      else
	retval.push_back(m_vecData[i]);
    }
    return retval;
  }

  /** fills buffer with contents of REXPString's data - vector of strings (m_vecData)
   * @param[out] buf buffer to fill with data
   * @param[in] consumerNAValue NA representation for strings used by the consumer
   * @return vector of strings, m_vecData
   */
  void REXPString::fillData(RVECTORTYPE<RSTRINGTYPE> &buf, const RSTRINGTYPE &consumerNAValue) const{
    buf.resize(m_vecData.size());
    for(size_t i = 0; i < m_vecData.size(); ++i){
      if (m_vecData[i] == getNARepresentation())
	buf[i] = consumerNAValue;
      else
	buf[i] = m_vecData[i];
    }
  }



  /** Compares the provided string to the R interpretation of NA to determine if it is NA
   * @param[in] str string to be compared to determine if it is considered NA
   * @return true if val is NA, false otherwise
   */
  bool REXPString::isNA(const RSTRINGTYPE &str){
    return str == NA;
  }

  /** Returns R interpretation of NA string
   * @return R interpretation of NA string
   */
  RSTRINGTYPE REXPString::getNARepresentation() const{
    return NA;
  }

  /** fills in provided array with unsigned chars containing R contents to be used by the network
   * quad-aligns each string in the vector to be sent
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of array
   * @return bool indicating if the network data fits in the provided array
   */
  bool REXPString::toNetworkData(unsigned char *buf, const size_t &length) const{
    // fill in array with data
    size_t buf_i = 0;
    for(size_t i = 0; i < m_vecData.size(); ++i){
      size_t len = m_vecData[i].size() + 1;
      if(buf_i+len > length) return false;
      memcpy(&buf[buf_i], m_vecData[i].c_str(), len);
      buf_i += len;
    }
    // quadalign
    if(buf_i%4) memset(&buf[buf_i], 1, 4-buf_i%4);
    return true;
  }

  /** returns number of bytes of data that would be passed over the network by this REXPString's data
   * used by RPacketEntry in the entry header
   * @return number of bytes in m_vecData
   */
  size_t REXPString::bytelength() const{
    return getBytelength(m_vecData);
  }

} // close namespace
