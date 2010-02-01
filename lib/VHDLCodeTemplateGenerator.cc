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


#include <VHDLCodeTemplateGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace boost;
using namespace degate;

VHDLCodeTemplateGenerator::VHDLCodeTemplateGenerator(std::string const& entity_name,
						     std::string const& description) :
  CodeTemplateGenerator(entity_name, description) {
}

VHDLCodeTemplateGenerator::~VHDLCodeTemplateGenerator() {
}

std::string VHDLCodeTemplateGenerator::generate() const {
  return generate_header() + generate_enitity() + generate_architecture();
}

std::string VHDLCodeTemplateGenerator::generate_header() const {
  boost::format f("--\n"
		  "-- This is VHDL code for a gate of type %1%\n"
		  "--\n"
		  "-- Please customize this code template according to your needs.\n\n"
		  "library ieee;\n"
		  "use ieee.std_logic_1164.all;\n\n");
  f % entity_name;
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_enitity() const {
  std::string out_ports;
  std::string in_ports;

  BOOST_FOREACH(port_map_type::value_type const& p, port_map) {
    if(p.second == true) {
      if(!in_ports.empty()) in_ports += ", ";
      in_ports += generate_identifier(p.first);
    }
    else if(p.second == false) {
      if(!out_ports.empty()) out_ports += ", ";
      out_ports += generate_identifier(p.first);
    }
  }
  
  boost::format f("entity %1% is\n"
		  "\tport (%2% : in std_logic;\n"
		  "\t\t%3% : out std_logic);\n"
		  "end %4%;\n\n");
  f % generate_identifier(entity_name) % in_ports  % out_ports % generate_identifier(entity_name);
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_architecture() const {

  boost::format f("architecture Behavioral of %1% is\n"
		  "begin\n"
		  "\t-- \n"
		  "\t-- Please implement behaviour.\n"
		  "\t-- \n"
		  "end Behavioral;\n\n");
  f % generate_identifier(entity_name);
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_identifier(std::string const& name) const {
  std::string identifier;
  
  bool first_char = true;
  BOOST_FOREACH(char c, name) {
    if(first_char && !isalpha(c)) {
      identifier.append("entity_");
      identifier.push_back(c);
    }
    else if(isalnum(c)) identifier.push_back(c);
    else identifier.push_back('_');

    first_char = false;
  }
  return identifier;
}
