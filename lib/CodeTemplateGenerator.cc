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


void CodeTemplateGenerator::add_port(std::string const& port_name, bool is_inport) {
  std::string lc = port_name;
  std::transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
  port_direction[lc] = is_inport;
}


std::string CodeTemplateGenerator::get_port_name_by_type(CodeTemplateGenerator::PORT_FUNCTION_TYPE t) const {
  port_direction_type::const_iterator found;
  
  if(t == CLOCK) {
    if((port_direction.end() != (found = port_direction.find("clock"))) ||
       (port_direction.end() != (found = port_direction.find("clk"))))
      return found->first;
  }
  else if(t == RESET) {
    if((port_direction.end() != (found = port_direction.find("/reset"))) ||
       (port_direction.end() != (found = port_direction.find("!reset"))) ||
       (port_direction.end() != (found = port_direction.find("reset"))) ||
       (port_direction.end() != (found = port_direction.find("/rst"))) ||
       (port_direction.end() != (found = port_direction.find("!rst"))) ||
       (port_direction.end() != (found = port_direction.find("rst"))))
      return found->first;
  }
  else if(t == ENABLE) {
    if((port_direction.end() != (found = port_direction.find("en"))) ||
       (port_direction.end() != (found = port_direction.find("enable"))) ||
       (port_direction.end() != (found = port_direction.find("/en"))) ||
       (port_direction.end() != (found = port_direction.find("!en"))) ||
       (port_direction.end() != (found = port_direction.find("/enable"))) ||
       (port_direction.end() != (found = port_direction.find("!enable"))))
      return found->first;
  }
  else if(t == SELECT) {
    if((port_direction.end() != (found = port_direction.find("select"))) ||
       (port_direction.end() != (found = port_direction.find("sel"))) ||
       (port_direction.end() != (found = port_direction.find("s"))))
      return found->first;
  }
  else if(t == Q) {
    if((port_direction.end() != (found = port_direction.find("q"))) )
      return found->first;
  }
  else if(t == NOT_Q) {
    if((port_direction.end() != (found = port_direction.find("!q"))) ||
       (port_direction.end() != (found = port_direction.find("/q"))))
      return found->first;
  }
  else if(t == D) {
    if((port_direction.end() != (found = port_direction.find("d"))) )
      return found->first;
  }

  return "";
}



std::string CodeTemplateGenerator::get_first_port_name_not_in(std::vector<std::string> const& ports,
							      std::vector<std::string> const& blacklist) const {
  
  typedef std::vector<std::string>::const_iterator iter;

  BOOST_FOREACH(std::string const& p_name, ports) {
    iter i = std::find(blacklist.begin(), blacklist.end(), p_name);
    if(i == blacklist.end()) return p_name;    
  }

  return "";
}

std::string CodeTemplateGenerator::get_first_port_name_not_in(std::vector<std::string> const& ports,
							      std::string const& blacklist_item) const {
  std::vector<std::string> v(1);
  v[0] = blacklist_item;
  return get_first_port_name_not_in(ports, v);
}

std::vector<std::string> CodeTemplateGenerator::get_inports() const {
  std::vector<std::string> ports;

  BOOST_FOREACH(port_direction_type::value_type const& p, port_direction)
    if(p.second == true) ports.push_back(p.first);

  std::sort(ports.begin(), ports.end());
  return ports;
}

std::vector<std::string> CodeTemplateGenerator::get_outports() const {
  std::vector<std::string> ports;

  BOOST_FOREACH(port_direction_type::value_type const& p, port_direction)
    if(p.second == false) ports.push_back(p.first);

  std::sort(ports.begin(), ports.end());
  return ports;
}

std::vector<std::string> CodeTemplateGenerator::get_ports() const {
  std::vector<std::string> ports;

  BOOST_FOREACH(port_direction_type::value_type const& p, port_direction)
    ports.push_back(p.first);

  std::sort(ports.begin(), ports.end());
  return ports;
}
