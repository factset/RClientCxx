/*  REXP Double: R Object Containing Vector of Doubles
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

#ifndef RCLIENT_REXP_DOUBLE_H_INCLUDED
#define RCLIENT_REXP_DOUBLE_H_INCLUDED

#include "config.h"
#include "rexp_vector.h"

namespace rclient{

  /** R vector with values of type: Double
   */
  class RCLIENT_API REXPDouble : public REXPVector {

  private:
    static const double NA;

  public:
    REXPDouble();
    ~REXPDouble();
    explicit REXPDouble(const double &val, const double &consumerNAValue = NA);
    explicit REXPDouble(const RVECTORTYPE<double> &vals, const double &consumerNAValue = NA);
    explicit REXPDouble(const RVECTORTYPE<float> &vals, const double &consumerNAValue = NA);

    virtual const size_t length() const;
    virtual bool isNA(const double &val);
    virtual double getNARepresentation() const;
    virtual const RVECTORTYPE<double> getData(const double &consumerNAValue = NA) const;
    virtual void fillData(RVECTORTYPE<double> &buf, const double &consumerNAValue = NA) const;

    // for network packet entries
    // dont want consumer to have access to these, but needed by rpacket_entry
    // possible use of private + friend here
    virtual bool toNetworkData(unsigned char *buf, size_t &length) const;
    virtual size_t bytelength() const;
  
  private:
    RVECTORTYPE<double> m_vecData;
    template<typename DoubleType>
      void initData(const RVECTORTYPE<DoubleType> &vals, const double &consumerNAValue);
  };
} // close namespace
#endif
