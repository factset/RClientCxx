/*  REXP Integer: R Object Containing Vector of 32bit Integers
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

#include "rexp_integer.h"
#include "endian_converter.h"

#include <string.h>
#include <limits>

namespace rclient{


  // R representation of NA: -2147483648
  const int32_t REXPInteger::NA(std::numeric_limits<int32_t>::min());

  /** Initializes vector of integers with conversion from consumer's NA representation to R's NA representation
   * @param[in] vals vector of integers to copy into REXP
   * @param[in] consumerNAValue NA representation for integers used by the consumer
   */
  void REXPInteger::initData(const RVECTORTYPE<int32_t> &vals, const int32_t &consumerNAValue){
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
  REXPInteger::REXPInteger():REXPVector(XT_ARRAY_INT),m_vecData(0){}

  /** destructor
   */ 
  REXPInteger::~REXPInteger(){}


  /** Copy constructor.
   * @param[in] exp REXPInteger to copy data from
   */
  REXPInteger::REXPInteger(const REXPInteger &exp):REXPVector(XT_ARRAY_INT, exp.getData().size() * sizeof(int32_t)), m_vecData(exp.getData()){
    if(exp.hasAttributes())
      REXP::setAttributes(exp.getAttributes());
  }

  /** constructor takes 1 integer and puts it into a vector of size 1
   * @param[in] val int value to populate m_vecData, REXPInteger's contents
   * @param[in] consumerNAValue NA representation for integers used by the consumer
   */
  REXPInteger::REXPInteger(const int32_t &val, const int32_t &consumerNAValue):REXPVector(XT_ARRAY_INT,sizeof(int32_t)){
    m_vecData.resize(1);
    if (val == consumerNAValue)
      m_vecData[0] = getNARepresentation();
    else
      m_vecData[0] = val;
  }

  /** constructor copies provided vector<int32_t> into itself
   * @param[in] vals vector<int32_t> to copy into m_vecData, REXPInteger's contents
   * @param[in] consumerNAValue NA representation for integers used by the consumer
   */
  REXPInteger::REXPInteger(const RVECTORTYPE<int32_t> &vals, const int32_t &consumerNAValue):REXPVector(XT_ARRAY_INT, vals.size()*sizeof(int32_t)){
    initData(vals, consumerNAValue);
  }

  /** constructor copies provided vector<double> into itself. Constructor for REXP with Attributes
   * @param[in] vals vector<double> to copy into m_vecData, REXPDouble's contents
   * @param[in] attr pointer to REXPPairList containing this REXP's attributes
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   */
  REXPInteger::REXPInteger(const RVECTORTYPE<int32_t> &vals, const RSHARED_PTR<const REXPPairList> &attr, const int32_t &consumerNAValue):REXPVector(attr, XT_ARRAY_INT, vals.size()*sizeof(int32_t)){
    initData(vals, consumerNAValue);
  }


  /** Retrieve the size of the m_vecData vector<int32_t>
   * @return length of vector m_vecData
   */
  const size_t REXPInteger::length() const{
    return m_vecData.size();
  }


  /** Retrieve contents of the m_vecData vector<int32_t>
   * @param[in] consumerNAValue NA representation for integers used by the consumer
   * @return const vector<int32_t> m_vecData
   */
  const RVECTORTYPE<int32_t> REXPInteger::getData(const int32_t &consumerNAValue) const{
    RVECTORTYPE<int32_t> retval;
    retval.reserve(m_vecData.size());
    for(size_t i = 0; i < m_vecData.size(); ++i){
      if (m_vecData[i] == getNARepresentation())
	retval.push_back(consumerNAValue);
      else
	retval.push_back(m_vecData[i]);
    }
    return retval;
  }


  /** Fills provided vector with contents of the m_vecData vector<int>
   * @param[out] buf buffer to fill with data
   * @param[in] consumerNAValue NA representation for integers used by the consumer
   */
  void REXPInteger::fillData(RVECTORTYPE<int32_t> &buf, const int32_t &consumerNAValue) const{
    buf.resize(m_vecData.size());
    for(size_t i = 0; i < m_vecData.size(); ++i){
      if (m_vecData[i] == getNARepresentation())
	buf[i] = consumerNAValue;
      else
	buf[i] = m_vecData[i];
    }
  }


  /** Compares the provided int to the R representation of NA to determine if it is NA
   * @param[in] val integer to be compared to determine if it is considered NA
   * @return true if val is NA, false otherwise
   */
  bool REXPInteger::isNA(const int32_t &val){
    return val == NA; //-2147483648
  }

  /** Returns R interpretation of NA integer
   * @return R interpretation of NA integer
   */
  int32_t REXPInteger::getNARepresentation() const{
    return NA; //-2147483648
  }

  /** fills in provided array with unsigned chars containing R contents to be used by the network
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of array
   * @return bool indicating if the network data fits in the provided array
   */
  bool REXPInteger::toNetworkData(unsigned char *buf, const size_t &length) const{
    EndianConverter converter;

    size_t pos = 0;
    RVECTORTYPE<uint8_t> serialized_int;
    serialized_int.resize(sizeof(uint32_t));

    for(size_t i = 0; i < m_vecData.size(); ++i){
      size_t buf_i = i*sizeof(uint32_t);
      if(buf_i + sizeof(uint32_t) > length) return false;
      converter.serialize<uint32_t>(serialized_int, pos, m_vecData[i]);
      memcpy(&buf[buf_i], &serialized_int[0], sizeof(uint32_t));
      pos = 0;
    }
    return true;
  }


  /** returns number of bytes of data being passed over the network
   * used by RPacketEntry in the entry header
   * @return number of bytes in m_vecData
   */
  size_t REXPInteger::bytelength() const{
    size_t length = sizeof(int32_t) * m_vecData.size();
    return length;
  }

}// close namespace
