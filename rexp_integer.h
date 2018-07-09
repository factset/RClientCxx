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

#ifndef RCLIENT_REXP_INTEGER_H_INCLUDED
#define RCLIENT_REXP_INTEGER_H_INCLUDED

#include "config.h"
#include "rexp_vector.h"

namespace rclient{

  /** R vector with values of type: Integer
   */
  class RCLIENT_API REXPInteger : public REXPVector {

  private:
    static const int32_t NA;

  public:
    REXPInteger();
    ~REXPInteger();
    REXPInteger(const REXPInteger &exp);
    REXPInteger& operator=(REXPInteger exp);
    explicit REXPInteger(const int32_t &val, const int32_t &consumerNAValue = NA);
    explicit REXPInteger(const RVECTORTYPE<int32_t> &vals, const int32_t &consumerNAValue = NA);
    REXPInteger(const RVECTORTYPE<int32_t> &vals, const RSHARED_PTR<const REXPPairList> &attr, const int32_t &consumerNAValue = NA);
    void swap(REXPInteger &exp);

    virtual size_t length() const;
    virtual bool isNA(const int32_t &val);
    virtual int32_t getNARepresentation() const;
    virtual RVECTORTYPE<int32_t> getData(const int32_t &consumerNAValue = NA) const;
    virtual void fillData(RVECTORTYPE<int32_t> &buf, const int32_t &consumerNAValue = NA) const;

    // for network packet entries
    // dont want consumer to have access to these, but needed by rpacket_entry
    // possible use of private + friend here
    virtual bool toNetworkData(unsigned char *buf, const size_t &length) const;
    virtual size_t bytelength() const;
  
  private:
    RVECTORTYPE<int32_t> m_vecData;
    void initData(const RVECTORTYPE<int32_t> &vals, const int32_t &consumerNAValue);

  };

} // close namespace
#endif
