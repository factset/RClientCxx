/*  REXP: Base class for R Expressions. Container for data.
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

#include "rexp.h"
#include "rpacket_entry_0103.h"
#include "rexp_pairlist.h"

#include <string.h>
#include <sstream>

namespace rclient{

  /** basic public constructor, sets type to XT_S4. contains no data
   */
  REXP::REXP():m_eType(XT_S4){}

  /** Copy constructor.
   * @param[in] exp REXP to copy data from
   */
  REXP::REXP(const REXP &exp):m_eType(XT_S4){
    if(exp.hasAttributes())
      REXP::setAttributes(exp.getAttributes());
  }

  /** public constructor with attributes, sets type to XT_S4. contains no data
   * @param[in] attr REXPPairList containing this REXP's attributes
   */
  REXP::REXP(const RSHARED_PTR<const REXPPairList> &attr):m_eType(XT_S4){
    // make sure the pointer isn't null
    if(attr.get()){
      m_pAttributes = attr;
      m_eType = m_eType | XT_HAS_ATTR;
    }
  }

  /** basic protected constructor, sets type to XT_S4 if type is not provided
   * @param[in] type enum corresponding to the REXP type
   * @param[in] size number of bytes in the REXP's data
   */
  REXP::REXP(const eType type, const size_t size):m_eType(type){
    if(size > 0x7fffff) m_eType = m_eType | XT_LARGE;
  }

  /** protected constructor with attributes, sets type to XT_S4 if type is not provided
   * @param[in] attr REXPPairList containing this REXP's attributes
   * @param[in] type enum corresponding to the REXP type
   * @param[in] size number of bytes in the REXP's data
   */
  REXP::REXP(const RSHARED_PTR<const REXPPairList> &attr, const eType type, const size_t size):m_eType(type){
    // make sure the pointer isn't null
    if(attr.get()){
      m_pAttributes = attr;
      m_eType = m_eType | XT_HAS_ATTR;
    }
    if(size > 0x7fffff) m_eType = m_eType | XT_LARGE;
  }


  /** destructor
   */
  REXP::~REXP(){}

  /** Returns enum value corresponding to the REXP type
   * @return enum REXP data type
   */
  uint32_t REXP::getType() const{
    return m_eType;
  }

  /** Returns value corresponding to the REXP type with no flags included
   * @return enum REXP data type
   */
  uint32_t REXP::getBaseType() const{
    return m_eType & XT_TYPE_MASK;
  }

  /** Returns user-readable string describing the REXP type
   * @return string describing REXP type
   */
  RSTRINGTYPE REXP::getTypeName() const{
    std::stringstream msg;
    
    if(getType() & XT_LARGE)
      msg << "Large ";
    
    switch(getBaseType()){
    case XT_NULL:
      {
        msg << "XT_NULL (REXPNull)";
        break;
      }
    case XT_INT:
      {
        msg << "XT_INT";
        break;
      }
    case XT_DOUBLE:
      {
        msg << "XT_DOUBLE";
        break;
      }
    case XT_STR:
      {
        msg << "XT_STR";
        break;
      }
    case XT_LANG:
      {
        msg << "XT_LANG";
        break;
      }
    case XT_SYM:
      {
        msg << "XT_SYM";
        break;
      }
    case XT_BOOL:
      {
        msg << "XT_BOOL";
        break;
      }
    case XT_S4:
      {
        msg << "XT_S4 (raw REXP)";
        break;
      }
    case XT_VECTOR:
      {
        msg << "XT_VECTOR";
        break;
      }
    case XT_LIST:
      {
        msg << "XT_LIST";
        break;
      }
    case XT_CLOS:
      {
        msg << "XT_CLOS (closure)";
        break;
      }
    case XT_SYMNAME:
      {
        msg << "XT_SYMNAME (symbol name)";
        break;
      }
    case XT_LIST_NOTAG:
      {
        msg << "XT_LIST_NOTAG (REXPPairList)";
        break;
      }
    case XT_LIST_TAG:
      {
        msg << "XT_LIST_TAG (REXPPairList)";
        break;
      }
    case XT_LANG_NOTAG:
      {
        msg << "XT_LANG_NOTAG";
        break;
      }
    case XT_LANG_TAG:
      {
        msg << "XT_LANG_TAG";
        break;
      }
    case XT_VECTOR_EXP:
      {
        msg << "XT_VECTOR_EXP";
        break;
      }
    case XT_VECTOR_STR:
      {
        msg << "XT_VECTOR_STR";
        break;
      }
    case XT_ARRAY_INT:
      {
        msg << "XT_ARRAY_INT (REXPInteger)";
        break;
      }
    case XT_ARRAY_DOUBLE:
      {
        msg << "XT_ARRAY_DOUBLE (REXPDouble)";
        break;
      }
    case XT_ARRAY_STR:
      {
        msg << "XT_ARRAY_STR (REXPString)";
        break;
      }
    case XT_ARRAY_BOOL_UA:
      {
        msg << "XT_ARRAY_BOOL_UA";
        break;
      }
    case XT_ARRAY_BOOL:
      {
        msg << "XT_ARRAY_BOOL";
        break;
      }
    case XT_RAW:
      {
        msg << "XT_RAW";
        break;
      }
    case XT_ARRAY_CPLX:
      {
        msg << "XT_ARRAY_CPLX";
        break;
      }
    case XT_UNKNOWN:
      {
        msg << "XT_UNKNOWN";
        break;
      }
    default:
      {
        msg << "unknown type";
      }
    }

    if(getType() & XT_HAS_ATTR)
      msg << " with attributes.";

    msg << " Type #" << getType();

    return RSTRINGTYPE(msg.str());
  }


  /** return whether or not this REXP has attributes
   * @return True if this REXP has attributes, False otherwise
   */
  bool REXP::hasAttributes() const{
    return m_pAttributes.get();
  }

  /** sets the REXP's attributes (REXPPairList)
   * @param[in] attr attributes to assign to this REXP
   */
  void REXP::setAttributes(RSHARED_PTR<const REXPPairList> attr){
    m_pAttributes = attr;
  }
  
  /** gets the REXP's attributes (REXPPairList)
   * @return this REXP's attributes
   */
  RSHARED_PTR<const REXPPairList> REXP::getAttributes() const{
    return m_pAttributes;
  }

  /** Fills buffer with REXP contents in format to be sent to RServe
   * @param[out] buf unsigned character array to hold network data
   * @param[in] length size of buf
   * @return bool indicating if the network data fits in buf. Always true since raw REXP has no data
   */
  bool REXP::toNetworkData(unsigned char *buf, const size_t &length) const{
    return true;
  }

  /** returns number of bytes of data to be sent over the network for this REXP
   * used by RPacketEntry in the entry header
   * @return Zero as XT_S4 has no data
   */
  size_t REXP::bytelength() const{
    return 0;
  }

} // close namespace
