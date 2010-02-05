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
#include <algorithm>
#include <string>

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
  std::string lc = port_name;
  std::transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
  port_direction[port_name] = is_inport;
}

std::string CodeTemplateGenerator::get_clock_port_name() const {
  port_direction_type::const_iterator found;
  if((port_direction.end() != (found = port_direction.find("clock"))) ||
     (port_direction.end() != (found = port_direction.find("clk"))))
    return found->first;
  else
    return "";
}

std::string CodeTemplateGenerator::get_reset_port_name() const {
  port_direction_type::const_iterator found;
  if((port_direction.end() != (found = port_direction.find("reset"))) ||
     (port_direction.end() != (found = port_direction.find("rst"))))
    return found->first;
  else
    return "";
}

std::vector<std::string> CodeTemplateGenerator::get_inports() const {
  std::vector<std::string> ports;
  
  BOOST_FOREACH(port_direction_type::value_type const& p, port_direction)
    if(p.second == true) ports.push_back(p.first);

  return ports;
}

std::vector<std::string> CodeTemplateGenerator::get_outports() const {
  std::vector<std::string> ports;
  
  BOOST_FOREACH(port_direction_type::value_type const& p, port_direction)
    if(p.second == false) ports.push_back(p.first);

  return ports;
}
