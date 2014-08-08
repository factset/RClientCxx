/*  REXP Vector: Abstract R Object Containing a vector
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

#include "rexp_vector.h"

namespace rclient{

  /** basic constructor sets REXP type to XT_VECTOR
   */
  REXPVector::REXPVector(const eType type, const size_t size):REXP(type, size){}

  /** destructor
   */
  REXPVector::~REXPVector(){}

} // close namespace
