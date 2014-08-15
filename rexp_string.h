/*  REXP String: R Object Containing vector of strings
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

#ifndef RCLIENT_REXP_STRING_H_INCLUDED
#define RCLIENT_REXP_STRING_H_INCLUDED

#include "config.h"
#include "rexp_vector.h"

namespace rclient{

  /** R vector with values of type: String
   */
  class RCLIENT_API REXPString : public REXPVector {
  
  private:
    static const RSTRINGTYPE NA;

  public:
    REXPString();
    ~REXPString();
    REXPString(const REXPString &exp);
    explicit REXPString(const RSTRINGTYPE &str, const RSTRINGTYPE &consumerNAValue = NA);
    explicit REXPString(const RVECTORTYPE<RSTRINGTYPE> &strVec, const RSTRINGTYPE &consumerNAValue = NA);
    explicit REXPString(const RVECTORTYPE<RSTRINGTYPE> &strVec, const RSHARED_PTR<const REXPPairList> &attr, const RSTRINGTYPE &consumerNAValue = NA);

    virtual const size_t length() const;
    virtual const RVECTORTYPE<RSTRINGTYPE> getData(const RSTRINGTYPE &consumerNAValue = NA) const;
    virtual void fillData(RVECTORTYPE<RSTRINGTYPE> &buf, const RSTRINGTYPE &consumerNAValue = NA) const;

    virtual bool isNA(const RSTRINGTYPE &str);
    virtual RSTRINGTYPE getNARepresentation() const;
    

    // for network packet entries
    // don't want consumer to have access to these, but they are needed by rpacket_entry
    // possible use of private + friend here
    virtual bool toNetworkData(unsigned char *buf, const size_t &length) const;
    virtual size_t bytelength() const;

  private:
    RVECTORTYPE<RSTRINGTYPE> m_vecData;
    void initData(const RVECTORTYPE<RSTRINGTYPE> &vals, const RSTRINGTYPE &consumerNAValue);
  };
} // close namespace

#endif
