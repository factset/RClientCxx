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

#include "rexp_list.h"
#include "rexp_null.h"

#include <stdexcept>

namespace{
  
  /** Calculates bytelength of a list
   * outside of REXPList to be used within its constructor
   * @param[in] list RPairVector to determine size of
   */
  size_t getBytelength(const rclient::REXPList::RVector &list){
    size_t length = 0;
    // for each REXP in the list
    for(size_t i = 0; i<list.size(); ++i){
      // add length of REXP
      length += list[i]->bytelength() + (list[i]->getType() & rclient::REXP::XT_LARGE ? 8:4);
    }
    return length;
  }
}


namespace rclient{


  /** empty constructor creates an empty list and sets R type
   */
  REXPList::REXPList():REXPVector(XT_LIST_NOTAG){}

  /** Copy constructor
   * @param[in] exp REXPList to copy m_pData and m_pAttr from
   */
  REXPList::REXPList(const REXPList &exp):REXPVector(XT_LIST_NOTAG, exp.bytelength()), m_vecData(exp.getData()){
    if(exp.hasAttributes())
      REXP::setAttributes(exp.getAttributes());
  }

  /** Constructor fills data field with provided vector and sets R type
   * @param[in] content Vector of shared_ptr<REXP>
   */
  REXPList::REXPList(const REXPList::RVector &content):REXPVector(XT_LIST_NOTAG, getBytelength(content)), m_vecData(content){}


  /** Constructor fills data field with provided vector and sets R type. Constructor for REXP with Attributes
   * @param[in] content Vector of shared_ptr<REXP>
   * @param[in] attr pointer to REXPPairList containing this REXP's attributes
   */
  REXPList::REXPList(const REXPList::RVector &content, const RSHARED_PTR<const REXPPairList> &attr):REXPVector(attr, XT_LIST_NOTAG,getBytelength(content)), m_vecData(content){}

  /** destructor
   */
  REXPList::~REXPList(){}


  /** Returns the number of members of the REXPList
   * @return number of elements in the REXPList
   */
  const size_t REXPList::length() const{
    return m_vecData.size();
  }

  /** Retrieves Vector of pairs from the REXPList datatype
   * @return vector of shared_ptr <REXP>
   */
  const REXPList::RVector& REXPList::getData() const {
    return m_vecData;
  }


  /** Throws logic error as RPacketEntry needs to handle the lists contents individually
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of array
   */
  bool REXPList::toNetworkData(unsigned char *buf, const size_t &length) const{
    throw std::logic_error("REXPList cannot convert itself to network data, get its members individually.");
  }

  /** returns number of bytes of the content that is sent over the network
   * used by RPacketEntry to create packet entry header
   * @return unsigned 32bit int denoting number of bytes in m_vecData
   */
  size_t REXPList::bytelength() const{
    return getBytelength(m_vecData);
  }
} // close namespace
