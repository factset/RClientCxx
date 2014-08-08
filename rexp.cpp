/*  REXP: Base class for R Expressions. Container for data.
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

#include "rexp.h"

#include <stdexcept>

namespace rclient{

  /** basic constructor, sets type to XT_S4
   */
  REXP::REXP(const eType type, const size_t size):m_eType(type){
    if(size > 0x7fffff) m_eType = type | XT_LARGE;
  }

  /** destructor
   */
  REXP::~REXP(){}

  /** Returns enum value corresponding to the REXP type
   * @return enum REXP data type
   */
  uint32_t REXP::getType() const{
    return m_eType;
  }

  /** Returns value corresponding to the REXP type with no flags included
   * @return enum REXP data type
   */
  uint32_t REXP::getBaseType() const{
    return m_eType & 63;
  }
} // close namespace
