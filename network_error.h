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

#ifndef RCLIENT_NETWORK_ERROR_H_INCLUDED
#define RCLIENT_NETWORK_ERROR_H_INCLUDED

#include "config.h"
#include <string>
#include <stdexcept> // for runtime_error

namespace rclient{

  /** Derived from std::runtime_error, exception for failed network communication
   * Contains errno and strerror(errno)
   */
  class RCLIENT_API NetworkError : public std::runtime_error {

  public:
    explicit NetworkError(const std::string &what_arg, const int &error = 0);
    NetworkError(const std::string &what_arg, const int &error, const RSTRINGTYPE &error_string);
    
    virtual ~NetworkError() throw();
    
    int getErrno() const;
    const RSTRINGTYPE & getErrnoString() const;
    
    
  private:
    int m_iErrno; // holds errno
    RSTRINGTYPE m_strError; // holds strerror(errno)
  };

} // close namespace

#endif
