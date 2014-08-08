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
    cout << "   REXP Type of entry: " << rexp->getType() << endl;
    cout << "   Contents of REXP: ";
    printData(response->getData(na), separator);
  }
  else
    cout << "  Received unexpected REXP type." << endl;
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
  cout << "   demo -s" << endl;
  cout << "   demo -a" << endl;
  cout << "      All flags can be combined. However they are not used if 'help' or an invalid parameter is included." << endl << endl;
  cout << "      'help' prints this usage message." << endl << endl;
  cout << "      The '-h' flag specifies the hostname/IP for RClient to connect to RServe." << endl;
  cout << "      By default, localhost is used (IP 127.0.0.1)" << endl << endl;
  cout << "      The '-p' flag specifies the port for RClient to connect to RServe" << endl;
  cout << "      By default, port 6311 us used" << endl << endl;
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
 * @param[out] shutdown flag to shutdown server instead of sending assign and eval calls
 * @param[out] anyVersion flag to specify if RClient can connect to any version of RServe (instead of only version 0103)
 * return TRUE if RClient should connect to RServe and execute a request. FALSE if demo closes immediately
 */
bool checkFlags(const int argc, char * const argv[], std::string &host, int &port, bool &shutdown, bool &anyVersion){

  if (argc > 7){ // too many arguments
    usage();
    return false;
  }
  for(int argp=1; argp<argc; ++argp){
    if(std::string(argv[argp]).compare("-h") == 0){
      // hostname flag
      ++argp;
      if(argp >= argc || std::string(argv[argp]).compare(0,1,"-") == 0){
	// invalid or missing hostname
	usage();
	return false;
      }
      host = argv[argp];
    }
    else if(std::string(argv[argp]).compare("-p") == 0){
      // port flag
      ++argp;
      if(argp >= argc || std::string(argv[argp]).compare(0,1,"-") == 0){
	// invalid or missing port
	usage();
	return false;
      }
      stringstream(argv[argp]) >> port;
    }
    else if(std::string(argv[argp]).compare("-s") == 0){
      // shutdown flag
      shutdown = true;
    }
    else if(std::string(argv[argp]).compare("-a") == 0){
      anyVersion = true;
    }
    else{
      // invalid flag
      usage();
      return false;
    }
  }
  return true;
}

/** Test REXPDouble and REXPPairList Class
 */
int main (int argc, char * const argv[]) {

  std::string host("localhost");
  int port = 6311;
  bool shutdown = false;
  bool anyVersion = false;
  
  // check flags. if false, then return immediately
  if(!checkFlags(argc, argv, host, port, shutdown, anyVersion))
    return 0;

  // create the client
  rclient::RClient client(host, port, anyVersion);
  
  // retrieve RServe Information
  cout << endl << "Rserve Server Information: " << client.getRserveVersion() << endl;

  if(shutdown){
    // shutdown server if flag is set
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

  // eval 11: REXPDouble / 2
  cout << endl << "eval to get 'rexp_d / 2'" << endl;
  rexp = client.eval("rexp_d / 2");
  printEvalStatus<rclient::REXPDouble>(client, rexp,  -1e20, "  ");
  
  printDivider();

  // eval 12: REXPInteger / 2 (becomes REXPDouble)
  cout << endl << "eval to get 'rexp_i / 2'" << endl;
  cout << " RServe converts int to double for calculations." << endl;
  rexp = client.eval("rexp_i / 2");
  printEvalStatus<rclient::REXPDouble>(client, rexp, -1e20, "  ");

  return 0;
}
