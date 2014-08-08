/*  REXP Double: R Object Containing Vector of Doubles
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

#include "rexp_double.h"
#include "endian_converter.h"

#include <string.h>

namespace{

  /* RServe's double NA representation is NaN and must be declared using a union.
   * @return Rserve's double NA representation.
   */
  double rserve_NA(){
    union {uint64_t ui64; double d;} NA_u;
    NA_u.ui64 = 0x7ff00000000007a2L;
    return NA_u.d;
  }

  /* Converts provided double into long unsigned int to compare the bits and determine if it is RServe's NA value.
   * Removes concern of handling NaN.
   * @param[in] val value to compare to RServe's NA value.
   * @return True if val matches RServe's NA value, False otherwise.
   */
  bool isRserve_NA(const double &val){
    union {uint64_t ui64; double d;} val_u;
    val_u.d = val;
    // mask to prevent the conversion to QNaN
    val_u.ui64 = val_u.ui64 & 0xfff7ffffffffffffL;
    return ( val_u.ui64 == 0x7ff00000000007a2L);    
  }


  /* Converts provided doubles into long unsigned int to compare the bits and determine if they are equal.
   * Removes concern of handling NaN.
   * @param[in] val value to compare to the provided na value.
   * @param[in] na consumer na value.
   * @return True if val and na are the same, False otherwise.
   */
  bool isConsumer_NA(const double &val, const double &NA){
    union {uint64_t ui64; double d;} val_u, NA_u;
    val_u.d = val;
    NA_u.d = NA;
    return (val_u.ui64 == NA_u.ui64);    
  }  

} // close namespace


namespace rclient{
  
  /** R representation of NA
   * Note that NA is NaN, but not all NaN are NA
   */
  const double REXPDouble::NA(rserve_NA());


  /** Initializes vector of doubles with conversion from consumer's NA representation to R's NA representation
   * @param[in] vals vector of doubles or floats to copy into REXP
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   */
  template<typename DoubleType>
  void REXPDouble::initData(const RVECTORTYPE<DoubleType> &vals, const double &consumerNAValue){
    m_vecData.reserve(vals.size());
    for(size_t i = 0; i < vals.size(); ++i){
      if(isConsumer_NA(vals[i], consumerNAValue))
	m_vecData.push_back(getNARepresentation());
      else
	m_vecData.push_back(vals[i]);
    }
  }
  template void REXPDouble::initData<double>(const RVECTORTYPE<double> &vals, const double &consumerNAValue);
  template void REXPDouble::initData<float>(const RVECTORTYPE<float> &vals, const double &consumerNAValue);


  /** basic constructor initializes an empty vector
   */
  REXPDouble::REXPDouble():REXPVector(XT_ARRAY_DOUBLE),m_vecData(0){}

  /** destructor
   */ 
  REXPDouble::~REXPDouble(){}

  /** constructor takes 1 double and puts it into a vector of size 1
   * @param[in] val double value to populate m_vecData, REXPDouble's contents
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   */
  REXPDouble::REXPDouble(const double &val, const double &consumerNAValue):REXPVector(XT_ARRAY_DOUBLE,sizeof(double)){
    m_vecData.resize(1);
    if(isConsumer_NA(val, consumerNAValue))
      m_vecData[0] = getNARepresentation();
    else
      m_vecData[0] = val;
  }
  
  /** constructor copies provided vector<double> into itself
   * @param[in] vals vector<double> to copy into m_vecData, REXPDouble's contents
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   */
  REXPDouble::REXPDouble(const RVECTORTYPE<double> &vals, const double &consumerNAValue):REXPVector(XT_ARRAY_DOUBLE, vals.size()*sizeof(double)){
    initData(vals, consumerNAValue);
  }

  /** constructor copies provided vector<float> into itself
   * @param[in] vals vector<float> to copy into m_vecData, REXPDouble's contents
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   */
  REXPDouble::REXPDouble(const RVECTORTYPE<float> &vals, const double &consumerNAValue):REXPVector(XT_ARRAY_DOUBLE, vals.size()*sizeof(double)){
    initData(vals, consumerNAValue);
  }

  /** Retrieve the size of the m_vecData vector<double>
   * @return length of vector m_vecData
   */
  const size_t REXPDouble::length() const{
    return m_vecData.size();
  }


  /** Retrieve contents of the m_vecData vector<double>
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   * @return const vector<double> m_vecData
   */
  const RVECTORTYPE<double> REXPDouble::getData(const double &consumerNAValue) const{
    RVECTORTYPE<double> retval;
    retval.reserve(m_vecData.size());
    for(size_t i = 0; i < m_vecData.size(); ++i){
      if(isRserve_NA(m_vecData[i]))
	retval.push_back(consumerNAValue);
      else
	retval.push_back(m_vecData[i]);
    }
    return retval;
  }

  /** Fills provided vector with contents of the m_vecData vector<double>
   * @param[out] buf buffer to fill with data
   * @param[in] consumerNAValue NA representation for doubles used by the consumer
   */
  void REXPDouble::fillData(RVECTORTYPE<double> &buf, const double &consumerNAValue) const{
    buf.resize(m_vecData.size());
    for(size_t i = 0; i < m_vecData.size(); ++i){
      if(isRserve_NA(m_vecData[i]))
	buf[i] = consumerNAValue;
      else
	buf[i] = m_vecData[i];
    }
  }


  /** Compares the provided double to the R interpretation of NA to determine if it is NA
   * @param[in] val double to be compared to m_dNA to determine if it is considered NA
   * @return true if val is NA, false otherwise
   */
  bool REXPDouble::isNA(const double &val){
    return isRserve_NA(val);
  }

  /** Returns R interpretation of NA double
   * @return R interpretation of NA double
   */
  double REXPDouble::getNARepresentation() const{
    return NA;
  }


  /** fills in provided array with unsigned chars containing R contents to be used by the network
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of array
   * @return bool indicating if the network data fits in the provided array
   */
  bool REXPDouble::toNetworkData(unsigned char *buf, size_t &length) const{
    EndianConverter converter;
    for(size_t i = 0; i < m_vecData.size(); ++i){
      size_t buf_i = i*sizeof(double);
      if(buf_i+sizeof(double) > length) return false;
      double network_double = converter.swap_endian(m_vecData[i]);
      memcpy(&buf[buf_i], &network_double, sizeof(double));
    }
    return true;
  }

  /** returns number of bytes of data being passed over the network
   * used by RPacketEntry in the entry header
   * @return number of bytes in m_vecData
   */
  size_t REXPDouble::bytelength() const{
    size_t length = sizeof(double) * m_vecData.size();
    return length;
  }

} // close namespace
