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
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VHDLCodeTemplateGenerator::VHDLCodeTemplateGenerator(std::string const& entity_name,
						     std::string const& description,
						     std::string const& logic_class) :
  CodeTemplateGenerator(entity_name, description, logic_class) {
}

VHDLCodeTemplateGenerator::~VHDLCodeTemplateGenerator() {
}

std::string VHDLCodeTemplateGenerator::generate() const {
  return 
    generate_header() + 
    generate_entity() + 
    generate_architecture(generate_impl(logic_class));
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

std::string VHDLCodeTemplateGenerator::generate_entity() const {
  
  boost::format f("entity %1% is\n"
		  "\tport(%2% : in std_logic;\n"
		  "\t\t%3% : out std_logic);\n"
		  "end %4%;\n\n");
  f % generate_identifier(entity_name) 
    % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_inports()), ", ")
    % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_outports()), ", ")
    % generate_identifier(entity_name);
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_impl(std::string const& logic_class) const {

  std::vector<std::string> in = get_inports();
  std::vector<std::string> out = get_outports();

  if(logic_class == "inverter" &&
     in.size() == 1 && out.size() == 1) {
    boost::format f("\t%1% <= not %2%;");
    f % generate_identifier(out[0]) % generate_identifier(in[0]);
    return f.str();
  }
  else if((logic_class == "xor" ||
	   logic_class == "or" ||
	   logic_class == "and" ||
	   logic_class == "xnor" ||
	   logic_class == "nor" ||
	   logic_class == "nand") &&
	  in.size() >= 2 && out.size() == 1) {

    std::string inner_op, outer_op = "not";

    if(logic_class == "nand") inner_op = "and";
    else if(logic_class == "nor") inner_op = "or";
    else if(logic_class == "xnor") inner_op = "xor";
    else {
      outer_op = "";
      inner_op = logic_class;
    }

    boost::format f("\t%1% <= %2%%3%%4%%5%;");
    f % generate_identifier(out[0]) 
      % outer_op
      % (outer_op.empty() ? "" : "(")
      % boost::algorithm::join(generate_identifier<std::vector<std::string> >(in), 
			       std::string(" ") + inner_op + std::string(" "))
      % (outer_op.empty() ? "" : ")");

    return f.str();
  }
  else {
    return 
      "\t-- \n"
      "\t-- Please implement behaviour.\n"
      "\t-- \n";
  }
}

std::string VHDLCodeTemplateGenerator::generate_architecture(std::string const& impl) const {

  boost::format f("architecture Behavioral of %1% is\n"
		  "begin\n"
		  "%2%\n"
		  "end Behavioral;\n\n");
  f % generate_identifier(entity_name) % impl;
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

