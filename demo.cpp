/*  Main: Demo for RClient
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

#include <iostream>
#include <string>
#include <sstream>

#include "rclient.h"

using namespace std;

/** prints a line
 */
void printDivider(){
  cout << "______________________________________" << endl;
}

/** prints contents of a vector of data
 * Templated for type of contents in vector
 * @param[in] data vector of data to be printed.
 * @param[in] separator string to put in between data entries that are printed
 */
template <typename DataType>
inline void printData(const RVECTORTYPE<DataType> &data, const std::string &separator){
  for(size_t i=0; i < data.size() ; ++i){
    cout << separator << data[i];
  }
  cout << endl;
}


void printData(const rclient::REXPPairList::RPairVector &data, const RSTRINGTYPE &str_na, const double d_na, const int32_t int_na, const std::string &separator);

/** prints contents of a REXPList
 * @param[in] data vector of data to be printed.
 * @param[in] str_na String na representation to be converted from RServe's NA
 * @param[in] d_na Double na representation to be converted from RServe's NA
 * @param[in] int_na Integer na representation to be converted from RServe's NA
 * @param[in] separator string to put in between data entries that are printed
 */
void printData(const rclient::REXPList::RVector &data, const RSTRINGTYPE &str_na, const double d_na, const int32_t int_na, const std::string &separator){
  for(size_t i=0; i < data.size(); ++i){
    if(const rclient::REXPDouble *entry = dynamic_cast<const rclient::REXPDouble *>(data[i].get())){
      printData(entry->getData(d_na), separator);
    }
    if(const rclient::REXPInteger *entry = dynamic_cast<const rclient::REXPInteger *>(data[i].get())){
      printData(entry->getData(int_na), separator);
    }
    if(const rclient::REXPString *entry = dynamic_cast<const rclient::REXPString *>(data[i].get())){
      printData(entry->getData(str_na), separator);
    }
    if(const rclient::REXPList *entry = dynamic_cast<const rclient::REXPList *>(data[i].get())){
      printData(entry->getData(), str_na, d_na, int_na, separator);
    }
    if(const rclient::REXPPairList *entry = dynamic_cast<const rclient::REXPPairList *>(data[i].get())){
      printData(entry->getData(), str_na, d_na, int_na, separator);
    }
  }
  cout << endl;
}


/** prints contents of a pairlist
 * @param[in] data vector of data to be printed.
 * @param[in] str_na String na representation to be converted from RServe's NA
 * @param[in] d_na Double na representation to be converted from RServe's NA
 * @param[in] int_na Integer na representation to be converted from RServe's NA
 * @param[in] separator string to put in between data entries that are printed
 */
void printData(const rclient::REXPPairList::RPairVector &data, const RSTRINGTYPE &str_na, const double d_na, const int32_t int_na, const std::string &separator){
  for(size_t i=0; i < data.size(); ++i){
    cout << "   " << data[i].second << ": ";
    if(const rclient::REXPDouble *entry = dynamic_cast<const rclient::REXPDouble *>(data[i].first.get())){
      printData(entry->getData(d_na), separator);
    }
    if(const rclient::REXPInteger *entry = dynamic_cast<const rclient::REXPInteger *>(data[i].first.get())){
      printData(entry->getData(int_na), separator);
    }
    if(const rclient::REXPString *entry = dynamic_cast<const rclient::REXPString *>(data[i].first.get())){
      printData(entry->getData(str_na), separator);
    }
    if(const rclient::REXPList *entry = dynamic_cast<const rclient::REXPList *>(data[i].first.get())){
      printData(entry->getData(), str_na, d_na, int_na, separator);
    }
    if(const rclient::REXPPairList *entry = dynamic_cast<const rclient::REXPPairList *>(data[i].first.get())){
      printData(entry->getData(), str_na, d_na, int_na, separator);
    }
  }
  cout << endl;
}

/** Prints packet response information from an assign server command
 * @param[in] client RClient which performed assign request
 */
void printAssignStatus(const rclient::RClient &client){
  cout << "   Request successful: " << boolalpha << client.response_isSuccessful() << endl;
  cout << "   Error status: " << std::hex << client.response_errorStatus() << endl;
  cout << "   Number of data entries in response packet: " << std::dec << client.response_entryCount() << endl;
  cout << "   DataType of first entry in packet: " << client.response_getType(0) << endl << endl;
}



/** Prints packet response information from an eval server command
 * Templated for REXP type received
 * @param[in] client RClient which performed the eval request
 * @param[in] rexp response from eval request
 * @param[in] na na representation to be converted from RServe's NA
 * @param[in] separator string to put in between data entries that are printed
 */
template <typename REXPType, typename DataType>
inline void printEvalStatus(const rclient::RClient &client, RSHARED_PTR<const rclient::REXP> rexp, const DataType &na, const std::string &separator){
  REXPType response;
  if (const REXPType *response = dynamic_cast<const REXPType *>(rexp.get())){
    cout << "   Request successful: " << boolalpha << client.response_isSuccessful() << endl;
    cout << "   Error status: " << std::hex << client.response_errorStatus() << endl;
    cout << "   Number of data entries in response packet: " << std::dec << client.response_entryCount() << endl;
    cout << "   DataType of first entry in packet: " << client.response_getType(0) << endl;
    cout << "   REXP Type of entry: " << rexp->getTypeName() << endl;
    cout << "   Contents of REXP: ";
    printData(response->getData(na), separator);
    if(response->hasAttributes()){
      cout << endl << "  Contains attributes: " << endl;
      if(const rclient::REXPPairList *response = dynamic_cast<const rclient::REXPPairList *>(rexp->getAttributes().get()))
        printData(response->getData(), "[NA]", 0, 0, "   ");
      else
        cout << "  Cannot read attributes." << endl;
    }
  }
  else
    cout << "  Received unexpected REXP type: " << rexp->getTypeName() << endl;
  cout << endl;
}


/** Prints packet response information from an eval server command
 * Specialization for REXPList
 * @param[in] client RClient which performed the eval request
 * @param[in] rexp response from eval request
 * @param[in] str_na String na representation to be converted from RServe's NA
 * @param[in] d_na Double na representation to be converted from RServe's NA
 * @param[in] int_na Integer na representation to be converted from RServe's NA
 * @param[in] separator string to put in between data entries that are printed
 */
void printListEvalStatus(const rclient::RClient &client, RSHARED_PTR<const rclient::REXP> rexp, const RSTRINGTYPE &str_na,
                                        const double &d_na, const int32_t &int_na, const std::string &separator){
  rclient::REXPList response;
  if (const rclient::REXPList *response = dynamic_cast<const rclient::REXPList *>(rexp.get())){
    cout << "  Request successful: " << boolalpha << client.response_isSuccessful() << endl;
    cout << "  Error status: " << std::hex << client.response_errorStatus() << endl;
    cout << "  Number of data entries in response packet: " << std::dec << client.response_entryCount() << endl;
    cout << "  DataType of first entry in packet: " << client.response_getType(0) << endl;
    cout << "  REXP Type of entry: " << rexp->getTypeName() << endl;
    cout << "  Size of list: " << response->length() << endl;
    cout << "  Contents of REXP: " << endl;
    printData(response->getData(), str_na, d_na, int_na, "   ");
    if(response->hasAttributes()){
      cout << "  REXP contains attributes: " << endl;
      if(const rclient::REXPPairList *response = dynamic_cast<const rclient::REXPPairList *>(rexp->getAttributes().get()))
        printData(response->getData(), str_na, d_na, int_na, "   ");
      else
        cout << "  Cannot read attributes." << endl;
    }
  }
  else
    cout << "  Received unexpected REXP type: " << rexp->getTypeName() << endl;

  cout << endl;
}


/** Prints packet response information from an eval server command
 * Specialization for REXPPairList
 * @param[in] client RClient which performed the eval request
 * @param[in] rexp response from eval request
 * @param[in] str_na String na representation to be converted from RServe's NA
 * @param[in] d_na Double na representation to be converted from RServe's NA
 * @param[in] int_na Integer na representation to be converted from RServe's NA
 * @param[in] separator string to put in between data entries that are printed
 */
void printPairListEvalStatus(const rclient::RClient &client, RSHARED_PTR<const rclient::REXP> rexp, const RSTRINGTYPE &str_na,
                             const double &d_na, const int32_t &int_na, const std::string &separator, const std::string &get_by_name){
  rclient::REXPPairList response;
  if (const rclient::REXPPairList *response = dynamic_cast<const rclient::REXPPairList *>(rexp.get())){
    cout << "  Request successful: " << boolalpha << client.response_isSuccessful() << endl;
    cout << "  Error status: " << std::hex << client.response_errorStatus() << endl;
    cout << "  Number of data entries in response packet: " << std::dec << client.response_entryCount() << endl;
    cout << "  DataType of first entry in packet: " << client.response_getType(0) << endl;
    cout << "  REXP Type of entry: " << rexp->getTypeName() << endl;
    cout << "  Size of list: " << response->length() << endl;
    cout << "  \"" << get_by_name <<"\" is of type: " << response->getMember(get_by_name)->getTypeName() << endl;
    if (const rclient::REXPString *rexp_string = dynamic_cast<const rclient::REXPString *>(response->getMember(get_by_name).get()))
      printData(rexp_string->getData(str_na), "   ");
    else if (const rclient::REXPDouble *rexp_double = dynamic_cast<const rclient::REXPDouble *>(response->getMember(get_by_name).get()))
      printData(rexp_double->getData(d_na), "   ");
    else if (const rclient::REXPInteger *rexp_int = dynamic_cast<const rclient::REXPInteger *>(response->getMember(get_by_name).get()))
      printData(rexp_int->getData(int_na), "   ");
    else if (const rclient::REXPList *rexp_list = dynamic_cast<const rclient::REXPList *>(response->getMember(get_by_name).get()))
      printData(rexp_list->getData(), str_na, d_na, int_na, "   ");
    else if (const rclient::REXPPairList *rexp_pairlist = dynamic_cast<const rclient::REXPPairList *>(response->getMember(get_by_name).get()))
      printData(rexp_pairlist->getData(), str_na, d_na, int_na, "   ");
    cout << "  Contents of REXP: " << endl;
    printData(response->getData(), str_na, d_na, int_na, "   ");
    if(response->hasAttributes()){
      cout << "  REXP contains attributes: " << endl;
      if(const rclient::REXPPairList *response = dynamic_cast<const rclient::REXPPairList *>(rexp->getAttributes().get()))
        printData(response->getData(), str_na, d_na, int_na, "   ");
      else
        cout << "  Cannot read attributes." << endl;
    }
  }
  else
    cout << "  Received unexpected REXP type: " << rexp->getTypeName() << endl;

  cout << endl;
}


/** Prints usage for the demo
 */
void usage(){
  cout << "RClient Demo -- Executes assign and eval calls to RServe:" << endl;
  cout << "   Default host:  localhost (127.0.0.1)" << endl;
  cout << "   Default port:  6311" << endl << endl;
  cout << "   demo" << endl;
  cout << "   demo help" << endl;
  cout << "   demo -h hostname" << endl;
  cout << "   demo -p port" << endl;
  cout << "   demo -l username password" << endl;
  cout << "   demo -s" << endl;
  cout << "   demo -a" << endl;
  cout << "      All flags can be combined. However they are not used if 'help' or an invalid parameter is included." << endl << endl;
  cout << "      'help' prints this usage message." << endl << endl;
  cout << "      The '-h' flag specifies the hostname/IP for RClient to connect to RServe." << endl;
  cout << "      By default, localhost is used (IP 127.0.0.1)" << endl << endl;
  cout << "      The '-p' flag specifies the port for RClient to connect to RServe" << endl;
  cout << "      By default, port 6311 us used" << endl << endl;
  cout << "      The '-l' flag tells RClient to login to RServe with the provided username and password." << endl << endl;
  cout << "      The '-s' flag tells RServe to shutdown. RClient will send a shutdown request" << endl;
  cout << "      instead of calling assign and eval." << endl << endl;
  cout << "      The '-a' flag declares that RClient should connect to any version of RServe." << endl;
  cout << "      If the flag is not set, RClient will only connect to RServe version 0103." << endl << endl;
}


/** Traverses mainline arguments for appropriate flags
 * @param[in] argc number of arguments
 * @param[in] argv array of mainline arguments
 * @param[out] host RServe hostname/IP to connect to
 * @param[out] port port # to connect to
 * @param[out] username String username to log into RServe with -l flag
 * @param[out] password String password to log into RServe with -l flag
 * @param[out] login flag to send login as first command to RServe
 * @param[out] shutdown flag to shutdown server instead of sending assign and eval calls
 * @param[out] anyVersion flag to specify if RClient can connect to any version of RServe (instead of only version 0103)
 * return TRUE if RClient should connect to RServe and execute a request. FALSE if demo closes immediately
 */
bool checkFlags(const int argc, char * const argv[], RSTRINGTYPE &host, int &port, RSTRINGTYPE &username, RSTRINGTYPE &password, bool &login, bool &shutdown, bool &anyVersion){

  if (argc > 10){
    cout << "Too many arguments!" << endl << endl;
    usage();
    return false;
  }
  for(int argp=1; argp<argc; ++argp){
    if(std::string(argv[argp]).compare("-h") == 0){
      // hostname flag
      ++argp;
      if(argp >= argc || std::string(argv[argp]).compare(0,1,"-") == 0){
        cout << "Invalid or missing hostname." << endl << endl;
	usage();
	return false;
      }
      host = argv[argp];
    }
    else if(std::string(argv[argp]).compare("-p") == 0){
      // port flag
      ++argp;
      if(argp >= argc || std::string(argv[argp]).compare(0,1,"-") == 0){
        cout << "Invalid or missing port." << endl << endl;
	usage();
	return false;
      }
      stringstream(argv[argp]) >> port;
    }
    else if(std::string(argv[argp]).compare("-l") == 0){
      // login flag
      ++argp;
      if(argp >= argc || std::string(argv[argp]).compare(0,1,"-") == 0){
        cout << "Invalid or missing username." << endl << endl;
	usage();
	return false;
      }
      username = argv[argp];
      ++argp;
      if(argp >= argc || std::string(argv[argp]).compare(0,1,"-") == 0){
        cout << "Invalid or missing password." << endl << endl;
	usage();
	return false;
      }
      password = argv[argp];
      login = true;
    }
    else if(std::string(argv[argp]).compare("-s") == 0){
      // shutdown flag
      shutdown = true;
    }
    else if(std::string(argv[argp]).compare("-a") == 0){
      anyVersion = true;
    }
    else{
      // invalid flag or 'help'
      usage();
      return false;
    }
  }
  return true;
}

/** Test REXPDouble and REXPPairList Class
 */
int main (int argc, char * const argv[]) {

  RSTRINGTYPE host("localhost");
  int port = 6311;
  RSTRINGTYPE username;
  RSTRINGTYPE password;
  bool login = false;
  bool shutdown = false;
  bool anyVersion = false;
  
  // check flags. if false, then return immediately
  if(!checkFlags(argc, argv, host, port, username, password, login, shutdown, anyVersion))
    return 0;

  // create the client
  rclient::RClient client(host, port, anyVersion);
  
  // retrieve RServe Information
  cout << endl << "Rserve Server Information: " << client.getRserveVersion() << endl;
  


  // attempt login if login flag is set
  if(login){
    if(client.login(username, password)){
      cout << "Login: Ready to send requests to server." << endl << endl;
    }
    else{
      cout << "Login Failed." << endl << endl;;
      return 0;
    }
  }
  
  // shutdown server if flag is set
  if(shutdown){
    cout << endl << "mainprog shutting down server" << endl;
    client.shutdown();
    cout << "  Request successful: " << boolalpha << client.response_isSuccessful() << endl;
    return 0;
  }

  // REXPInteger with int vector
  RVECTORTYPE<int> vec_i;
  vec_i.push_back(2);
  vec_i.push_back(10);
  vec_i.push_back(77);
  vec_i.push_back(-5555);
  vec_i.push_back(300);
  rclient::REXPInteger rexp_int(vec_i, -5555); // treat -5555 as NA
  cout << "Created REXPInteger to send to server: ";
  printData(rexp_int.getData(), "  ");

  // REXPDouble with double vector
  RVECTORTYPE<double> vec_d;
  vec_d.push_back(0);
  vec_d.push_back(10.5);
  vec_d.push_back(77);
  vec_d.push_back(-5.5);
  vec_d.push_back(-1e20);
  rclient::REXPDouble rexp_d(vec_d, -1e20); // treat -1e20 as NA
  cout << "Created REXPDouble to send to server: ";
  printData(rexp_d.getData(), "  ");

  // REXPString with string vector
  RVECTORTYPE<RSTRINGTYPE> vec_str;
  vec_str.push_back("Hello World");
  vec_str.push_back("Test");
  vec_str.push_back("RClient");
  vec_str.push_back("");
  vec_str.push_back("Goodbye World");
  rclient::REXPString rexp_str(vec_str, ""); // treat "" as NA
  cout << "Created REXPString to send to server: ";
  printData(rexp_str.getData(), "\n       ");

  // REXPList with RVector
  RSHARED_PTR<rclient::REXPDouble> bsp_rexp_d = RMAKE_SHARED<rclient::REXPDouble>(vec_d, -1e20);
  RSHARED_PTR<rclient::REXPInteger> bsp_rexp_i = RMAKE_SHARED<rclient::REXPInteger>(vec_i, -5555);
  RSHARED_PTR<rclient::REXPString> bsp_rexp_s = RMAKE_SHARED<rclient::REXPString>(vec_str, "");
  rclient::REXPList::RVector r_vector;
  r_vector.push_back(bsp_rexp_d);
  r_vector.push_back(bsp_rexp_s);
  r_vector.push_back(bsp_rexp_i);
  rclient::REXPList rexp_list(r_vector);
  cout << "Created REXPList to send to server: " << endl;
  printData(rexp_list.getData(), "[NOT AVAILABLE]",  -1e20, -5555, "   ");
  cout << endl;

  // REXPPairList with RPairVector
  rclient::REXPPairList::RPair pair_d(bsp_rexp_d,"REXP Double");
  rclient::REXPPairList::RPair pair_i(bsp_rexp_i,"REXP Integer");
  rclient::REXPPairList::RPair pair_s(bsp_rexp_s,"REXP String");
  rclient::REXPPairList::RPairVector pair_vector;
  pair_vector.push_back(pair_d);
  pair_vector.push_back(pair_i);
  pair_vector.push_back(pair_s);
  rclient::REXPPairList rexp_pairlist(pair_vector);
  cout << "Created REXPPairList to send to server: " << endl;
  printData(rexp_pairlist.getData(), "[NOT AVAILABLE]",  -1e20, -5555, "   ");
  cout << endl;

  // Attribute
  RVECTORTYPE<RSTRINGTYPE> attr_str;
  attr_str.push_back("Attribute");
  attr_str.push_back("Information");
  RSHARED_PTR<rclient::REXPString> attr_rexp = RMAKE_SHARED<rclient::REXPString>(attr_str, "");
  rclient::REXPPairList::RPair attr_pair(attr_rexp,"ATTR");
  rclient::REXPPairList::RPairVector attr_pair_vector;
  attr_pair_vector.push_back(attr_pair);
  RSHARED_PTR<const rclient::REXPPairList> attr = RMAKE_SHARED<const rclient::REXPPairList>(attr_pair_vector);

  // REXPDouble with attribute
  rclient::REXPDouble rexp_d_attr(vec_d, attr, -1e20);
  cout << "Created REXPDouble with attributes to send to server." << endl;
  // REXPInteger with attribute
  rclient::REXPInteger rexp_int_attr(vec_i, attr, -5555);
  cout << "Created REXPInteger with attributes to send to server." << endl;
  // REXPString with attribute
  rclient::REXPString rexp_str_attr(vec_str, attr, "");
  cout << "Created REXPString with attributes to send to server." << endl;
  // REXPList with attribute
  rclient::REXPList rexp_list_attr(r_vector, attr);
  cout << "Created REXPList with attributes to send to server." << endl;
  // REXPPairList with attribute
  rclient::REXPPairList rexp_pair_attr(pair_vector, attr);
  cout << "Created REXPPairList with attributes to send to server." << endl;
  cout << "Attributes: " << endl;
  printData(attr->getData(), "[NOT AVAILABLE]",  -1e20, -5555, "   ");



  printDivider();

  // assign 1: vector<double>
  cout << endl << "assign vec_d <- vector<double>" << endl;
  client.assign<RVECTORTYPE<double>, rclient::REXPDouble, double>("vec_d", vec_d, -1e20);
  printAssignStatus(client);
  // eval 1: REXPDouble
  cout << "eval to get 'vec_d'" << endl;
  RSHARED_PTR<const rclient::REXP> rexp = client.eval("vec_d");
  printEvalStatus<rclient::REXPDouble>(client, rexp, -1e20, "  ");

  printDivider();

  // assign 2: vector<string>
  cout << endl << "assign vec_s <- vector<string>" << endl;
  client.assign<RVECTORTYPE<RSTRINGTYPE>, rclient::REXPString, RSTRINGTYPE>("vec_s", vec_str, "");
  printAssignStatus(client);
  // eval 2: REXPString
  cout << "eval to get 'vec_s'" << endl;
  rexp = client.eval("vec_s");
  printEvalStatus<rclient::REXPString>(client, rexp, "[NOT AVAILABLE]", "\n    ");

  printDivider();

  // assign 3: vector<int>
  cout << endl << "assign vec_i <- vector<int>" << endl;
  client.assign<RVECTORTYPE<int>, rclient::REXPInteger, int>("vec_i", vec_i, -5555);
  printAssignStatus(client);
  // eval 3: REXPInteger
  cout << "eval to get 'vec_i'" << endl;
  rexp = client.eval("vec_i");
  printEvalStatus<rclient::REXPInteger>(client, rexp, -5555, "  ");

  printDivider();

  // assign 4: string
  cout << endl << "assign single_s <- string" << endl;
  client.assign<RSTRINGTYPE, rclient::REXPString>("single_s", "Hello World");
  printAssignStatus(client);
  // eval 4: REXPString
  cout << "eval to get 'single_s'" << endl;
  rexp = client.eval("single_s");
  printEvalStatus<rclient::REXPString>(client, rexp, "[NOT AVAILABLE]", "\n    ");

  printDivider();

  // assign 5: int
  cout << endl << "assign single_i <- integer" << endl;
  client.assign<int, rclient::REXPInteger>("single_i", 13);
  printAssignStatus(client);
  // eval 5: REXPInteger
  cout << "eval to get 'single_i'" << endl;
  rexp = client.eval("single_i");
  printEvalStatus<rclient::REXPInteger>(client, rexp, -5555, "  ");

  printDivider();

  // assign 6: double
  cout << endl << "assign single_d <- double" << endl;
  client.assign<double, rclient::REXPDouble>("single_d", 7.125);
  printAssignStatus(client);
  // eval 6: REXPDouble
  cout << "eval to get 'single_d'" << endl;
  rexp = client.eval("single_d");
  printEvalStatus<rclient::REXPDouble>(client, rexp, -1e20, "  ");

  printDivider();

  // assign 7: REXPDouble
  cout << endl << "assign rexp_d <- REXPDouble" << endl;
  client.assign("rexp_d", rexp_d);
  printAssignStatus(client);
  // eval 7: REXPDouble
  cout << "eval to get 'rexp_d'" << endl;
  rexp = client.eval("rexp_d");
  printEvalStatus<rclient::REXPDouble>(client, rexp, -1e20, "  ");

  printDivider();

  // eval 7.1: REXPDouble / 2
  cout << endl << "eval to get 'rexp_d / 2'" << endl;
  rexp = client.eval("rexp_d / 2");
  printEvalStatus<rclient::REXPDouble>(client, rexp,  -1e20, "  ");

  printDivider();

  // assign 9: REXPString
  cout << endl << "assign rexp_s <- REXPString" << endl;
  client.assign("rexp_s", rexp_str);
  printAssignStatus(client);
  // eval 9: REXPString
  cout << "eval to get 'rexp_s'" << endl;
  rexp = client.eval("rexp_s");
  printEvalStatus<rclient::REXPString>(client, rexp, "[NOT AVAILABLE]", "\n    ");

  printDivider();

  // assign 10: REXPInteger
  cout << endl << "assign rexp_i <- REXPInteger" << endl;
  client.assign("rexp_i", rexp_int);
  printAssignStatus(client);
  // eval 10: REXPInteger
  cout << "eval to get 'rexp_i'" << endl;
  rexp = client.eval("rexp_i");
  printEvalStatus<rclient::REXPInteger>(client, rexp, -5555, "  ");
  
  printDivider();

  // eval 10.1: REXPInteger / 2 (becomes REXPDouble)
  cout << endl << "eval to get 'rexp_i / 2'" << endl;
  cout << " RServe converts int to double for calculations." << endl;
  rexp = client.eval("rexp_i / 2");
  printEvalStatus<rclient::REXPDouble>(client, rexp, -1e20, "  ");

  printDivider();

  // assign 11: REXPList
  cout << endl << "assign rexp_list <- REXPList" << endl;
  client.assign("rexp_list", rexp_list);
  printAssignStatus(client);
  // eval 11: REXPList
  cout << endl << "eval to get 'rexp_list'" << endl;
  // call eval to get the data of rlist
  rexp = client.eval("rexp_list");
  printListEvalStatus(client, rexp, "[NOT AVAILABLE]",  -1e20, -5555, "  ");

  printDivider();

  // assign 12: REXPPairList
  cout << endl << "assign rexp_pairlist <- REXPPairList" << endl;
  client.assign("rexp_pairlist", rexp_pairlist);
  printAssignStatus(client);
  cout << endl << "eval to get 'rexp_pairlist'" << endl;
  // call eval to get the data of rlist
  rexp = client.eval("rexp_pairlist");
  printPairListEvalStatus(client, rexp, "[NOT AVAILABLE]",  -1e20, -5555, "  ", "REXP Double");

  printDivider();

  // assign 13: REXPDouble with attributes
  cout << endl << "assign rexp_d_attr <- REXPDouble w/ attributes" << endl;
  client.assign("rexp_d_attr", rexp_d_attr);
  printAssignStatus(client);
  // eval 13: REXPDouble with attributes
  cout << "eval to get 'rexp_d_attr'" << endl;
  rexp = client.eval("rexp_d_attr");
  printEvalStatus<rclient::REXPDouble>(client, rexp, -1e20, "  ");

  printDivider();

  // assign 14: REXPInteger with attributes
  cout << endl << "assign rexp_int_attr <- REXPInteger w/ attributes" << endl;
  client.assign("rexp_int_attr", rexp_int_attr);
  printAssignStatus(client);
  // eval 14: REXPInteger with attributes
  cout << "eval to get 'rexp_int_attr'" << endl;
  rexp = client.eval("rexp_int_attr");
  printEvalStatus<rclient::REXPInteger>(client, rexp, -5555, "  ");

  printDivider();

  // assign 15: REXPString with attributes
  cout << endl << "assign rexp_str_attr <- REXPString w/ attributes" << endl;
  client.assign("rexp_str_attr", rexp_str_attr);
  printAssignStatus(client);
  // eval 15: REXPString with attributes
  cout << "eval to get 'rexp_str_attr'" << endl;
  rexp = client.eval("rexp_str_attr");
  printEvalStatus<rclient::REXPString>(client, rexp, "[NOT AVAILABLE}", "  ");

  printDivider();

  // assign 16: REXPList with attributes
  cout << endl << "assign rexp_list_attr <- REXPList w/ attributes" << endl;
  client.assign("rexp_list_attr", rexp_list_attr);
  printAssignStatus(client);
  // eval 16: REXPList with attributes
  cout << "eval to get 'rexp_list_attr'" << endl;
  rexp = client.eval("rexp_list_attr");
  printListEvalStatus(client, rexp, "[NOT AVAILABLE]",  -1e20, -5555, "  ");

  printDivider();

  // assign 17: REXPPairList with attributes
  cout << endl << "assign rexp_pair_attr <- REXPPairList w/ attributes" << endl;
  client.assign("rexp_pair_attr", rexp_pair_attr);
  printAssignStatus(client);
  // eval 17: REXPPairList with attributes
  cout << "eval to get 'rexp_pair_attr'" << endl;
  rexp = client.eval("rexp_pair_attr");
  printPairListEvalStatus(client, rexp, "[NOT AVAILABLE]",  -1e20, -5555, "  ", "REXP String");

  printDivider();

  cout << endl;

  return 0;
}
