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

#ifndef RCLIENT_REXP_VECTOR_H_INCLUDED
#define RCLIENT_REXP_VECTOR_H_INCLUDED

#include "config.h"
#include "rexp.h"

namespace rclient{

  /* abstract class for all vector-type R objects
   */
  class RCLIENT_API REXPVector : public REXP{

  protected:
    REXPVector(const eType type=XT_VECTOR, const size_t size=0);
    REXPVector(const RSHARED_PTR<const REXPPairList> &attr, const eType type=XT_VECTOR, const size_t size=0);
    ~REXPVector();

    virtual size_t length() const = 0; //length of the vector
  };

} // close namespace
#endif
