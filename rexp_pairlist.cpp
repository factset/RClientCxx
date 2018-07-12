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

#include "rexp_pairlist.h"
#include "rexp_null.h"

#include <stdexcept>

namespace{
  
  /** Calculates bytelength of a pairlist
   * outside of REXPPairList to be used within its constructor
   * @param[in] pairlist RPairVector to determine size of
   */
  size_t getBytelength(const rclient::REXPPairList::RPairVector &pairlist){
    size_t length = 0;
    // for each pair in the pairlist
    for(size_t i = 0; i<pairlist.size(); ++i){
      // add length of first member (REXP)
      size_t rexp_header_size = (pairlist[i].first->getType() & rclient::REXP::XT_LARGE ? 8:4);
      length += pairlist[i].first->bytelength() + rexp_header_size;

      // add length of  second member (string)
      size_t str_len = pairlist[i].second.size()+1;
      size_t str_len_aligned = str_len + (str_len%4 ? (4-(str_len%4)):0);
      size_t str_header_size = (str_len_aligned > 0x7fffff ? 8:4);
      length += str_header_size + str_len_aligned;
    }
    return length;
  }
}


namespace rclient{

  /** empty constructor creates an empty pairlist and sets R type
   */
  REXPPairList::REXPPairList():REXPVector(XT_LIST_TAG){}

  /** Copy constructor
   * @param[in] exp REXPPairList to copy m_pData and m_pAttr from
   */
  REXPPairList::REXPPairList(const REXPPairList &exp):REXPVector(XT_LIST_TAG, exp.bytelength()),m_vecData(exp.getData()){
    if(exp.hasAttributes())
      REXP::setAttributes(exp.getAttributes());
  }

  /** Assignment operator
   * @param[in] exp REXPPairList to copy data from
   */
  REXPPairList& REXPPairList::operator=(REXPPairList exp){
    exp.swap(*this);
    return *this;
  }

  /** constructor fills data field with provided vector and sets R type
   * @param[in] content Vector of <string (name), boost::share_ptr<REXP> > pairs
   */
  REXPPairList::REXPPairList(const REXPPairList::RPairVector &content):REXPVector(XT_LIST_TAG, getBytelength(content)),m_vecData(content){}


  /** constructor fills data field with provided vector and sets R type. Constructor for REXP with Attributes
   * @param[in] content Vector of <string (name), boost::share_ptr<REXP> > pairs
   * @param[in] attr pointer to REXPPairList containing this REXP's attributes
   */
  REXPPairList::REXPPairList(const RPairVector &content, const RSHARED_PTR<const REXPPairList> &attr):REXPVector(attr, XT_LIST_TAG, getBytelength(content)),m_vecData(content){}

  /** destructor
   */
  REXPPairList::~REXPPairList(){}

  /** swap contents of one instance with another
   *  @param[in] exp REXPPairList instance to swap with this
   */
  void REXPPairList::swap(REXPPairList &exp) {
    REXP::swap(exp);
    m_vecData.swap(exp.m_vecData);
  }

  /** Returns the number of members of the REXPPairList
   * @return number of elements in the REXPPairList
   */
  size_t REXPPairList::length() const{
    return m_vecData.size();
  }

  /** Retrieves Vector of pairs from the REXPPairList datatype
   * @return vector of pairs <shared_ptr <REXP>, string>
   */
  const REXPPairList::RPairVector& REXPPairList::getData() const {
    return m_vecData;
  }

  /** Returns the first REXP paired with the given string
   * @param[in] name key corresponding to an REXP (an Attribute as used within REXP class)
   * @return shared pointer to the REXP member paired to the string or REXPNull if no match
   */
  RSHARED_PTR<const REXP> REXPPairList::getMember(const RSTRINGTYPE &name) const {
    for(size_t i=0; i<m_vecData.size(); ++i){
      if(m_vecData[i].second == name){
	// match found
        return m_vecData[i].first;
      }
    }
    // no match found
    return RMAKE_SHARED<REXPNull>(); // does not compile with -o2 and higher
  }

  /** Checks m_rgData for the REXP matching the provided name
   * @param[in] name key corresponding to an REXP (an Attribute as used within REXP class)
   */
  bool REXPPairList::hasMember(const RSTRINGTYPE &name) const {
    for(size_t i=0; i<m_vecData.size(); ++i){
      if(m_vecData[i].second == name){
	// match found
	return true;
      }
    }
    // no match found
    return false;  
  }

  /** Throws logic error as RPacketEntry needs to handle the pairlists contents individually
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of array
   */
  bool REXPPairList::toNetworkData(unsigned char *buf, const size_t &length) const{
    throw std::logic_error("REXPPairList cannot convert itself to network data, get its members individually.");
  }

  /** returns number of bytes of the content that is sent over the network
   * used by RPacketEntry to create packet entry header
   * @return unsigned 32bit int denoting number of bytes in m_vecData
   */
  size_t REXPPairList::bytelength() const{
    return getBytelength(m_vecData);
  }
} // close namespace
