/*  NetworkError: subclass of runtime_error. Exception for NetworkManager
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

#include "network_error.h"

#include <cerrno> // for errno
#include <string.h> // for strerror()

namespace rclient{

  /** constructor stores errno and creates string from errno
   */
  NetworkError::NetworkError(const std::string &what_arg, const int &error):
    std::runtime_error(what_arg),m_iErrno(error),m_strError(strerror(error)){}

  /** constructor stores error and erro string provided
   */
  NetworkError::NetworkError(const std::string &what_arg, const int &error, const RSTRINGTYPE &error_string):
    std::runtime_error(what_arg),m_iErrno(error),m_strError(error_string){}

  /** destructor
   */
  NetworkError::~NetworkError() throw(){}
  
  /** retrieves stored errno from exception
   */
  int NetworkError::getErrno() const{
    return m_iErrno;
  }
  
  /** retrieves string corresponding to errno
   */
  const RSTRINGTYPE & NetworkError::getErrnoString() const{
    return m_strError;
  }


} // close namespace
