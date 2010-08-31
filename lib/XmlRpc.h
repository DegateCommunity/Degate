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

#ifndef __XMLRPC_H__
#define __XMLRPC_H__

#include <LogicModel.h>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

namespace degate {

  /**
   * Convinience method to call remote methods.
   */

  xmlrpc_c::value remote_method_call(std::string const& server_url,
				     std::string const& method_name,
				     xmlrpc_c::paramList const& params);

  /**
   * Push objects from the logic model to a remote server.
   */

  void push_changes_to_server(std::string const& server_url, LogicModel_shptr lmodel);

  /**
   * Pull objects from a remote server into the logic model.
   * @exception XMLRPCException This exception is thrown, if the XMLRPC fails for some reason.
   */

  transaction_id_t pull_changes_from_server(std::string const& server_url, LogicModel_shptr lmodel,
					    transaction_id_t start_tid) throw();

  /**
   * Parse an execute a command.
   * @exception XMLRPCException This exception is thrown, if commands cannot be parsed.
   */
  void process_changelog_command(LogicModel_shptr lmodel,
				 transaction_id_t transaction_id,
				 std::vector<xmlrpc_c::value> const& command) throw();
}

#endif
