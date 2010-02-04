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

#include <CodeTemplateGenerator.h>
#include <boost/foreach.hpp>

using namespace degate;
using namespace boost;

CodeTemplateGenerator::CodeTemplateGenerator(std::string const& _entity_name,
					     std::string const& _description,
					     std::string const& _logic_class) : 
  entity_name(_entity_name),
  description(_description),
  logic_class(_logic_class) {
}

CodeTemplateGenerator::~CodeTemplateGenerator() {
}


void CodeTemplateGenerator::add_port(std::string port_name, bool is_inport) {
  port_map[port_name] = is_inport;
}

std::vector<std::string> CodeTemplateGenerator::get_inports() const {
  std::vector<std::string> ports;
  
  BOOST_FOREACH(port_map_type::value_type const& p, port_map)
    if(p.second == true) ports.push_back(p.first);

  return ports;
}

std::vector<std::string> CodeTemplateGenerator::get_outports() const {
  std::vector<std::string> ports;
  
  BOOST_FOREACH(port_map_type::value_type const& p, port_map)
    if(p.second == false) ports.push_back(p.first);

  return ports;
}
