/*  REXP List: R Object Containing vector of REXP
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

#ifndef RCLIENT_REXP_LIST_H_INCLUDED
#define RCLIENT_REXP_LIST_H_INCLUDED

#include "config.h"
#include "rexp_vector.h"

namespace rclient{

  /** R vector of shared_ptr<REXP>
   */
  class RCLIENT_API REXPList : public REXPVector{

  public:
    typedef RVECTORTYPE<RSHARED_PTR<const REXP> > RVector;

    REXPList();
    ~REXPList();
    REXPList(const REXPList &exp);
    REXPList& operator=(REXPList exp);
    explicit REXPList(const RVector &content);
    REXPList(const RVector &content, const RSHARED_PTR<const REXPPairList> &attr);
    void swap(REXPList &exp);

    virtual size_t length() const;
    virtual const RVector& getData() const;

    // for network packet entries
    virtual bool toNetworkData(unsigned char *buf, const size_t &length) const;
    virtual size_t bytelength() const;

  private:
    RVector m_vecData;
  };

} // close namespace
#endif
