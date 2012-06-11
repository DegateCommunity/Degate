/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

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
#include <Wire.h>
#include <Via.h>

#include <boost/foreach.hpp>

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

    object_id_t local_oid = iter->second->get_object_id();

    if(RemoteObject_shptr ro = std::dynamic_pointer_cast<RemoteObject>((*iter).second)) {

      if(!ro->has_remote_object_id()) {
	std::cout << "Push object to server." << std::endl;
	object_id_t remote_oid = ro->push(server_url);

	lmodel->update_roid_mapping(remote_oid, local_oid);
      }
    }
  }

  BOOST_FOREACH(object_id_t id, lmodel->get_removed_remote_objetcs_list()) {
    xmlrpc_c::paramList params;
    params.add(xmlrpc_c::value_string("remove"));
    params.add(xmlrpc_c::value_int(id));

    remote_method_call(server_url, "degate.push", params);

    debug(TM, "Send remove message for object with remote ID %d", id);
  }

  lmodel->reset_removed_remote_objetcs_list();
}


void degate::process_changelog_command(LogicModel_shptr lmodel,
				       transaction_id_t transaction_id,
				       std::vector<xmlrpc_c::value> const& command) {

  debug(TM, "XMLRPC: process received command for transaction %d", transaction_id);

  if(lmodel->get_local_oid_for_roid(transaction_id) != 0) {
    debug(TM, "XMLRPC: transaction %d aready known.", transaction_id);
    return;
  }


  if(command.size() < 2) throw XMLRPCException("Command has not enough parameters.");

  if(command[0].type() != xmlrpc_c::value::TYPE_STRING)
    throw XMLRPCException("Command parameter is not a string");

  const std::string command_str = xmlrpc_c::value_string(command[0]);
  std::cout << " cmd: " << command_str << std::endl;

  if(!command_str.compare("remove")) {
    if(command.size() < 2)
      throw XMLRPCException("Command wire add has less then 8 parameters.");
    int ro_id = xmlrpc_c::value_int(command[1]);
    lmodel->remove_remote_object(ro_id);
  }

  else if( !command_str.compare("add")) {

    const std::string obj_type_str = xmlrpc_c::value_string(command[1]);

    if(!obj_type_str.compare("wire")) {
      if(command.size() < 8)
	throw XMLRPCException("Command wire add has less then 8 parameters.");

      int layer_id = xmlrpc_c::value_int(command[2]);
      int from_x = xmlrpc_c::value_int(command[3]);
      int from_y = xmlrpc_c::value_int(command[4]);
      int to_x = xmlrpc_c::value_int(command[5]);
      int to_y = xmlrpc_c::value_int(command[6]);
      unsigned int diameter = xmlrpc_c::value_int(command[7]);

      Wire_shptr w(new Wire(from_x, from_y, to_x, to_y, diameter));
      w->set_remote_object_id(transaction_id);
      Layer_shptr layer = lmodel->get_layer_by_id(layer_id);
      lmodel->add_object(layer, w);
    }
    else if(!obj_type_str.compare("via")) {
      if(command.size() < 7)
	throw XMLRPCException("Command via add has less then 8 parameters.");

      int layer_id = xmlrpc_c::value_int(command[2]);
      int x = xmlrpc_c::value_int(command[3]);
      int y = xmlrpc_c::value_int(command[4]);
      unsigned int diameter = xmlrpc_c::value_int(command[5]);
      const std::string direction = xmlrpc_c::value_string(command[6]);

      Via_shptr v(new Via(x, y, diameter, Via::get_via_direction_from_string(direction)));
      v->set_remote_object_id(transaction_id);
      Layer_shptr layer = lmodel->get_layer_by_id(layer_id);
      lmodel->add_object(layer, v);
    }
  }

  /*
  for(std::vector<xmlrpc_c::value>::const_iterator iter = command.begin();
      iter != command.end(); ++iter)
    std::cout << "\ttype " << (*iter).type() << std::endl;
  }
  */

}

transaction_id_t degate::pull_changes_from_server(std::string const& server_url,
						  LogicModel_shptr lmodel,
						  transaction_id_t start_tid) {
  try {
    xmlrpc_c::paramList params;
    params.add(xmlrpc_c::value_int(start_tid));
    xmlrpc_c::value_array ret(remote_method_call(server_url, "degate.pull", params));

    std::vector<xmlrpc_c::value> v(ret.vectorValueValue());

    if(start_tid == 0) ++start_tid;

    debug(TM, "Received %d changes since transaction %ld", v.size(), start_tid);

    unsigned int pos = 0;
    for(std::vector<xmlrpc_c::value>::const_iterator iter = v.begin();
	iter != v.end(); ++iter, pos++) {
      xmlrpc_c::value_array const& command(*iter);
      process_changelog_command(lmodel, start_tid + pos, command.vectorValueValue());

      /* If the process_changelog_command() fails with an exception, changes
	 were already made to the logic model. That is no problem, because
	 process_changelog_command() checks this case.
      */
    }

    return start_tid + v.size();
  }
  catch(XMLRPCException const& e) {
    throw;
  }
  catch(exception const& e) {
    cerr << "Client threw error: " << e.what() << endl;
    throw XMLRPCException(e.what());
  }
  catch(...) {
    cerr << "Client threw unexpected error." << endl;
    throw XMLRPCException("Client threw unexpected error.");
  }

  return start_tid;
}

