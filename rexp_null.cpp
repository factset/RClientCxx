/*  REXP Null: R Object Representing NULL
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

#include "rexp_null.h"

namespace rclient{

  /** empty constructor sets type to XT_NULL
   */
  REXPNull::REXPNull():REXP(XT_NULL){}

  /** fills in provided array with unsigned chars containing R contents to be used by the network
   * since this represents null, there are no contents
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of array
   * @return true since there is no data to add
   */
  bool REXPNull::toNetworkData(unsigned char *buf, size_t &length) const{
    return true;
  }

  /** returns number of bytes of the content that is sent over the network
   * used by RPacketEntry to create packet entry header
   * @return 0, REXPNull has no data
   */
  size_t REXPNull::bytelength() const{
    return 0;
  }

} // close namespace
