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

#ifndef RCLIENT_REXP_H_INCLUDED
#define RCLIENT_REXP_H_INCLUDED

#include "config.h"

#include <inttypes.h>

namespace rclient{

  /** abstract class representing any R object
   */
  class RCLIENT_API REXP{

  public:
    enum eType {
      XT_NULL    = 0,   // P  data: [0]
      XT_INT     = 1,   // -  data: [4] int (removed in protocol 0103)
      XT_DOUBLE  = 2,   // -  data: [8] double (removed in protocol 0103)
      XT_STR     = 3,   // P  data: [n]char null term. (removed in protocol 0103)
      XT_LANG    = 4,   // -  data: same as XT_LIST (removed in protocol 0103)
      XT_SYM     = 5,   // -  data: [n]char sybol name (removed in protocol 0103)
      XT_BOOL    = 6,   // -  data: [1]byte boolean (removed in protocol 0103)
                        // (1 = TRUE, 0 = FALSE, 2 = NA)
      XT_S4      = 7,   // P  data: [0] (completely vanilla REXP)

      XT_VECTOR      = 16,  // P  data: [?]REXP,REXP,... (vector of REXP)
      XT_LIST        = 17,  // -  data: X head, X vals, X tag (removed in protocol 0103)
      XT_CLOS        = 18,  // P  data: X formals, X body (closure)
      XT_SYMNAME     = 19,  // s  data: same as XT_STR (symbol name)
      XT_LIST_NOTAG  = 20,  // s  data: same as XT_VECTOR (dotted pair list w/o tags)
      XT_LIST_TAG    = 21,  // P  data: X tag, X val, Y tag, Y val (dotted pair list w/ tags)
      XT_LANG_NOTAG  = 22,  // s  data: same as XT_LIST_NOTAG (language list w/o tags)
      XT_LANG_TAG    = 23,  // s  data: same as XT_LIST_TAG (language list w/ tags)
      XT_VECTOR_EXP  = 26,  // s  data: same as XT_VECTOR (expression vector)
      XT_VECTOR_STR  = 27,  // -  data: same as XT_VECTOR (string vector unused. use XT_ARRAY_STR instead)
 
      XT_ARRAY_INT     = 32,   // P  data: [n*4]int,int,... (int array)
      XT_ARRAY_DOUBLE  = 33,   // P  data: [n*8]double,double,... (double array)
      XT_ARRAY_STR     = 34,   // P  data: string,string,... (string array)
      XT_ARRAY_BOOL_UA = 35,   // -  data: [n]byte,byte,... (unaligned, NOT supported anymore)
      XT_ARRAY_BOOL    = 36,   // P  data: int(n),byte,byte,...
      XT_RAW           = 37,   // P  data: int(n),byte,byte,...
      XT_ARRAY_CPLX    = 38,   // P  data: [n*16]double,double,... (Re,Im,Re,Im...)
      XT_UNKNOWN       = 48,   // P  data: [4]int - SEXP type (unknown type, no assumptions can be made)

      // flags for implementation:
      //    P = primary type
      //    s = secondary type, decoding identical to a primary type
      //    - = deprecated/removed
      XT_TYPE_MASK     = 63,   // Mask to get REXP type without any flags
      XT_LARGE         = 64,   // flag is set, REXP header in rpacket will be 8 bytes
      XT_HAS_ATTR      = 128}; // flag if set, the following REXP is the attribute

  protected:
    /* m_eType is stored as an int instead of enum because of the XT_HAS_ATTR and XT_LARGE flags.
       The flags are or'd with the eType for Rserve protocol and we cannot store (XT_TYPE | XT_HAS_ATTR) as an enum.
    */
    uint32_t m_eType; //REXP data type
  
  public:
  
    uint32_t getType() const;
    uint32_t getBaseType() const;

    // for network packet entries
    // dont want consumer to have access to these, but needed by rpacket_entry
    // possible use of private + friend here
    virtual bool toNetworkData(unsigned char *buf, size_t &length) const =0;
    virtual size_t bytelength() const = 0;

    virtual ~REXP();
  
  protected:
    explicit REXP(const eType type=XT_S4, const size_t size=0);
  };
} // close namespace
#endif

