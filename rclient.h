/*  RClient: Core object included by consumer. Handles RServe commands.
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

#ifndef RCLIENT_H_INCLUDED
#define RCLIENT_H_INCLUDED

#include "config.h"
#include "network_manager.h"
#include "rexp_class_hierarchy.h"

#include <inttypes.h>

namespace rclient{

  /** Primary class to be used by the consumer.
   * RClient is initialized with Rserve host IP and port that it will be interacting with.
   * RClient contains all of the Rserve commands that can be executed by the consumer.
   * The consumer will want to #include "rclient.h" in order to have access to all functionality within the client.
   * The client attempts to connect to Rserve on the first Rserve command requested by the consumer.
   * The client disconnects when a network error occurs or when its NetworkmManager is destroyed.
   */
  class RCLIENT_API RClient{

  public:

    explicit RClient(const RSTRINGTYPE &host, const int port = 6311, const bool allowAnyVersion = false);
    RClient(const RClient& no_copy); // non construction-copyable
    RClient& operator=(const RClient&); // non-copyable
    
    /* RServe Commands */
    bool login(const RSTRINGTYPE &user, const RSTRINGTYPE &pwd); // CMD_login
    bool shutdown(const RSTRINGTYPE &key = ""); // CMD_shutdown

    RSHARED_PTR<const REXP> eval(const RSTRINGTYPE &expr);

    bool assign(const RSTRINGTYPE &sym, const REXP &expr);
    template<typename T_VAL, typename T_REXP>
      bool assign(const RSTRINGTYPE &sym, const T_VAL &expr);
    template<typename T_VAL, typename T_REXP, typename T_NA>
      bool assign(const RSTRINGTYPE &sym, const T_VAL &expr, const T_NA &consumerNAValue);


    /* Following Rserve Commands not yet implemented on this client

       bool voidEval(const RSTRINGTYPE &expr);

       RSHARED_PTR<RSession> detachedEval(const RSTRINGTYPE expr, int *status=NULL);
       RSHARED_PTR<RSession> detach(int *status = NULL);

       bool openFile(const RSTRINGTYPE &filename);
       bool createFile(const RSTRINGTYPE &filename);
       int  readFile(RSTRINGTYPE &buffer);
       int  writeFile(const RSTRINGTYPE &content);
       bool closeFile();
       bool removeFile(const RSTRINGTYPE &filename);

       bool setServerBufferSize(const size_t buffersize);

       // map encoding options to corresponding strings
       enum encoding {UTF8 = 0, LATIN1 = 1, NATIVE = 2};
 
       bool setStringEncoding(const encoding code);

       void serverEval(const RSTRINGTYPE &expr);
       void serverSource(const RSTRINGTYPE &filename);
       void serverShutdown();
  
       // Since 1.7
       void switchTLS(const RSTRINGTYPE id);
       void secLogin(const ByteStream loginfo);
       RSHARED_PTR<REXP> OCcall(const RSHARED_PTR<REXP> expr);
       ByteStream keyReq(const RSTRINGTYPE loginfo);
    */

    // check status of last response
    bool response_isSuccessful() const;
    RSTRINGTYPE response_errorStatus() const;

    // check contents of last response
    size_t response_entryCount() const;
    int response_getType(const size_t pos) const;
    RSTRINGTYPE response_stringAt(const size_t pos) const;
    RSHARED_PTR<const REXP> response_REXPAt(const size_t &pos) const;

    const RSTRINGTYPE getRserveVersion();

  private:
    // network manager to handle all network activity
    NetworkManager m_NetMan;
    // most recent response from Rserve
    RSHARED_PTR<const RPacket> m_pLast_response;
  };
}
#endif
