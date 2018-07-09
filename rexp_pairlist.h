/*  REXP PairList: R Object Containing vector of Pairs <REXP, string>
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

#ifndef RCLIENT_REXP_PAIRLIST_H_INCLUDED
#define RCLIENT_REXP_PAIRLIST_H_INCLUDED

#include "config.h"
#include "rexp_vector.h"

namespace rclient{

  /** R vector of pairs of type <shared_ptr<REXP>, RSTRINGTYPE>
   * Primarily used by REXP as the attribute list, where the string is the attribute name and the REXP is the attribute value.
   */
  class RCLIENT_API REXPPairList : public REXPVector{
 
  public:
    typedef std::pair<RSHARED_PTR<const REXP>, RSTRINGTYPE> RPair;
    typedef RVECTORTYPE<RPair> RPairVector; // not to be confused with REXPVector

    REXPPairList();
    ~REXPPairList();
    REXPPairList(const REXPPairList &exp);
    explicit REXPPairList(const RPairVector &content);
    REXPPairList(const RPairVector &content, const RSHARED_PTR<const REXPPairList> &attr);

    virtual size_t length() const;
    virtual const RPairVector& getData() const;

    virtual RSHARED_PTR<const REXP> getMember(const RSTRINGTYPE &name) const;
    virtual bool hasMember(const RSTRINGTYPE &name) const;

    // for network packet entries
    virtual bool toNetworkData(unsigned char *buf, const size_t &length) const;
    virtual size_t bytelength() const;

  private:
    RPairVector m_vecData;
  };

} // close namespace
#endif
