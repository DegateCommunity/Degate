/* -*-c++-*-
 
  This file is part of the IC reverse engineering tool degate.
 
  Copyright 2008, 2009 by Martin Schobert
 
  Degate is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.
 
  Degate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with degate. If not, see <http://www.gnu.org/licenses/>.
 
*/

#include <XmlRpc.h>

using namespace degate;
using namespace std;



xmlrpc_c::value degate::remote_method_call(std::string const& server_url,
					   std::string const& method_name,
					   xmlrpc_c::paramList const& params) {

  xmlrpc_c::clientXmlTransport_curl myTransport;
  xmlrpc_c::client_xml myClient(&myTransport);
    
  xmlrpc_c::rpcPtr myRpcP(method_name, params);
  xmlrpc_c::carriageParm_curl0 myCarriageParam(server_url);
    
  myRpcP->call(&myClient, &myCarriageParam);
  
  assert(myRpcP->isFinished());
  
  return myRpcP->getResult();
}


void degate::push_changes_to_server(std::string const& server_url, LogicModel_shptr lmodel) {

  // iterate over gates
  for(LogicModel::object_collection::iterator iter = lmodel->objects_begin();
      iter != lmodel->objects_end(); ++iter) {

    if(RemoteObject_shptr ro = std::tr1::dynamic_pointer_cast<RemoteObject>((*iter).second)) {
      
      if(!ro->has_remote_object_id()) { 
	std::cout << "Push object to server" << std::endl;
	ro->push(server_url);
      }
    }
  }

}

void degate::process_changelog_command(LogicModel_shptr lmodel,
				       transaction_id_t transaction_id,
				       std::vector<xmlrpc_c::value> const& command) {

  for(std::vector<xmlrpc_c::value>::const_iterator iter = command.begin();
      iter != command.end(); ++iter) {
    std::cout << "\ttype " << (*iter).type() << std::endl;
  }

}

transaction_id_t degate::pull_changes_from_server(std::string const& server_url, 
						  LogicModel_shptr lmodel,
						  transaction_id_t start_tid) {
  try {
    xmlrpc_c::paramList params;
    params.add(xmlrpc_c::value_int(start_tid));
    
    xmlrpc_c::value_array ret(remote_method_call(server_url, "degate.pull", params));
    
    std::vector<xmlrpc_c::value> v(ret.vectorValueValue());

    debug(TM, "Received %d changes since transaction %ld", v.size(), start_tid);

    unsigned int pos = 0;
    for(std::vector<xmlrpc_c::value>::const_iterator iter = v.begin();
	iter != v.end(); ++iter, pos++) {
      std::cout << "type " << (*iter).type() << std::endl;

      xmlrpc_c::value_array const& command(*iter);
      process_changelog_command(lmodel, start_tid + pos, command.vectorValueValue());
    }

    return start_tid + v.size();
  }
  catch(exception const& e) {
    cerr << "Client threw error: " << e.what() << endl;
  }
  catch(...) {
    cerr << "Client threw unexpected error." << endl;
  }   

  return start_tid;
}

