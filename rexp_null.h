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

#ifndef RCLIENT_REXP_NULL_H_INCLUDED
#define RCLIENT_REXP_NULL_H_INCLUDED

#include "config.h"
#include "rexp.h"

namespace rclient{
  /** R Object representing null
   */
  class RCLIENT_API REXPNull : public REXP{
  public:
    REXPNull();
    REXPNull(const REXPNull &exp);
    REXPNull& operator=(REXPNull exp);
    explicit REXPNull(const RSHARED_PTR<const REXPPairList> &attr);
    void swap(REXPNull &exp);

    // for network packet entries
    virtual bool toNetworkData(unsigned char *buf, const size_t &length) const;
    virtual size_t bytelength() const;
  };

} // close namespace

#endif
