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

#ifndef RCLIENT_ENDIAN_CONVERTER_H_INCLUDED
#define RCLIENT_ENDIAN_CONVERTER_H_INCLUDED

#include "config.h"

#include <inttypes.h>

namespace rclient{

  /** Rserve is little-endian.
   * This contains the functions to convert between client and Rserve endians for ints and doubles.
   */
  class RCLIENT_API EndianConverter{
  public:
    
    EndianConverter();
    ~EndianConverter();

    // serialize integers
    template <typename IntType>
      void serialize(RVECTORTYPE<uint8_t> &buf, size_t &pos, IntType x);

    // deserialize integers
    template <typename IntType>
      IntType deserialize(const RVECTORTYPE<uint8_t> &buf, size_t &pos) const;

    // swap doubles
    double swap_endian(const double &swap) const;
    RVECTORTYPE<double> swap_endian(const RVECTORTYPE<double> &swap) const;

  };
}
#endif
