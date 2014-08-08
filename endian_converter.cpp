/*  Endian Converter: If computer is big-endian, EndianConverter swaps the bytes of the input
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

#include "endian_converter.h"
#include "boost_endian.hpp"

#include <climits> // for CHAR_BIT

namespace{
  template <typename T>
    union bytes{
    T num;
    unsigned char ch[sizeof(T)];
  };
}

namespace rclient{

  /** constructor
   */
  EndianConverter::EndianConverter(){}

  /** destructor
   */
  EndianConverter::~EndianConverter(){}


  /** stores contents of integer into little-endian byte vector
   * @param[out] buf vector of 1-byte ints to serialize data
   * @param[in,out] pos position in vector to begin filling data
   * @param[in] x unsigned integer to serialize into little-endian vector
   */
  template <typename IntType>
  void EndianConverter::serialize(RVECTORTYPE<uint8_t> &buf, size_t &pos, IntType x){
    for (size_t i = 0; i < sizeof(IntType); ++i)
      buf[pos++] = (uint8_t) (x >> (i * CHAR_BIT)) & 0xFF;
  }
  template RCLIENT_API void EndianConverter::serialize(RVECTORTYPE<uint8_t> &buf, size_t &pos, uint64_t x);
  template RCLIENT_API void EndianConverter::serialize(RVECTORTYPE<uint8_t> &buf, size_t &pos, uint32_t x);
  template RCLIENT_API void EndianConverter::serialize(RVECTORTYPE<uint8_t> &buf, size_t &pos, uint16_t x);


  /** deserializes contents of 1-byte integer vector into native-endian integer
   * @param[in] buf vector of 1-byte ints to deserialize
   * @param[in,out] pos position in vector to read serialized data from
   * @return templated IntType that is in native endian
   */
  template <typename IntType>
  IntType EndianConverter::deserialize(const RVECTORTYPE<uint8_t> &buf, size_t &pos) const{
    IntType ret = 0;
    for (size_t i = 0; i < sizeof(IntType); ++i)
      ret |= IntType( (IntType) (buf[pos++]) << (i * CHAR_BIT));
    return ret;
  }
  template RCLIENT_API uint64_t EndianConverter::deserialize(const RVECTORTYPE<uint8_t> &buf, size_t &pos) const;
  template RCLIENT_API uint32_t EndianConverter::deserialize(const RVECTORTYPE<uint8_t> &buf, size_t &pos) const;
  template RCLIENT_API uint16_t EndianConverter::deserialize(const RVECTORTYPE<uint8_t> &buf, size_t &pos) const;


  /** converts vector of doubles from client machine's endian to little-endian (or vice versa)
   * If the client is running in little-endian, the return value is the same as the input
   * @param[in] swap vector<double> to be converted between client endian and little-endian
   * @return copy of swap. if client is big-endian, then each entry of swap's bytes have been reversed
   */
  RVECTORTYPE<double> EndianConverter::swap_endian(const RVECTORTYPE<double> &swap) const{
#if defined(BOOST_BIG_ENDIAN)
    RVECTORTYPE<double> retval;
    retval.resize(swap.size());
    for(size_t i = 0; i < swap.size(); ++i){
      retval[i] = swap_endian(swap[i]);
    }
    return retval;
#else
    return swap;
#endif
  }
  
  /** converts double from client machine's endian to little-endian (or vice versa)
   * If the client is running in little-endian, the return value is the same as the input
   * @param[in] swap double to be converted between client endian and little-endian
   * @return copy of swap. if client is big-endian, then swap's bytes have been reversed
   */
  double EndianConverter::swap_endian(const double &swap) const{
#if defined(BOOST_BIG_ENDIAN)
    bytes<double> c, r;
    c.num = swap;
    r.ch[0] = c.ch[7];
    r.ch[1] = c.ch[6];
    r.ch[2] = c.ch[5];
    r.ch[3] = c.ch[4];
    r.ch[4] = c.ch[3];
    r.ch[5] = c.ch[2];
    r.ch[6] = c.ch[1];
    r.ch[7] = c.ch[0];
    return r.num;
#else
    return swap;
#endif
  }
  
} // close namespace
