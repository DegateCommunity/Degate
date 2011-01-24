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


#include <VerilogCodeTemplateGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VerilogCodeTemplateGenerator::VerilogCodeTemplateGenerator(std::string const& entity_name,
							   std::string const& description,
							   std::string const& logic_class) :
  CodeTemplateGenerator(entity_name, description, logic_class) {
}


VerilogCodeTemplateGenerator::~VerilogCodeTemplateGenerator() {
}

std::string VerilogCodeTemplateGenerator::generate() const {
  return
    generate_header() +
    generate_module(entity_name, generate_port_list()) +
    generate_port_definition() +
    generate_impl(logic_class) + 
    "\n\n"
    "endmodule\n\n";
}

std::string VerilogCodeTemplateGenerator::generate_header() const {
  boost::format f("/** \n"
		  " * This is a Verilog implementation for a gate of type %1%.\n"
		  " */\n\n"
		  "// Please customize this code template according to your needs.\n"
		  "\n\n");
  f % entity_name;
  return f.str();
}

std::string VerilogCodeTemplateGenerator::generate_module(std::string const& entity_name,
							  std::string const& port_description) const {

  boost::format f("module %1% (\n"
		  "%2%"
		  ");\n\n");
  f % generate_identifier(entity_name)
    % port_description;
  return f.str();
}

std::string VerilogCodeTemplateGenerator::generate_port_list() const {

  boost::format f("  %1%%2%\n"
		  "  %3%\n");
  f % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_inports()), ", ")
    % (get_inports().size() > 0 ? ", " : "")
    % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_outports()), ", ");
  return f.str();
}


std::string VerilogCodeTemplateGenerator::generate_port_definition() const {
  std::string ret;

  ret += "  // input ports;\n";
  BOOST_FOREACH(std::string const& port_name, 
		generate_identifier<std::vector<std::string> >(get_inports())) {
    boost::format f("  input %1%;\n");
    f % port_name;    
    ret += f.str();
  }


  ret += "\n  // output ports;\n";
  BOOST_FOREACH(std::string const& port_name, 
		generate_identifier<std::vector<std::string> >(get_outports())) {
    boost::format f("  output %1%;\n");
    f % port_name;    
    ret += f.str();
  }

  ret += "\n";

  return ret;
}


std::string VerilogCodeTemplateGenerator::generate_impl(std::string const& logic_class) const {

  std::vector<std::string> in = get_inports();
  std::vector<std::string> out = get_outports();

  std::string clock_name = get_clock_port_name();
  if(clock_name.empty()) clock_name = "clock";
  std::string reset_name = get_reset_port_name();
  if(reset_name.empty()) reset_name = "reset";
  std::string enable_name = get_enable_port_name();
  if(reset_name.empty()) reset_name = "enable";

  if(logic_class == "inverter" &&
     in.size() == 1 && out.size() == 1) {

    boost::format f("  assign %1% = !%2%;");
    f % generate_identifier(out[0]) % generate_identifier(in[0]);
    return f.str();
  }
  else if(logic_class == "tristate-inverter") {
    boost::format f("  tri %1%; // ???\n\n"
		    "  assign %2% = %3% ? !%4% : 1'bz;");
    f % generate_identifier(out[0]) 
      % generate_identifier(out[0])
      % generate_identifier(enable_name)
      % generate_identifier(get_first_port_name_not_in(in, enable_name));
    return f.str();
  }

  else if((logic_class == "xor" ||
	   logic_class == "or" ||
	   logic_class == "and" ||
	   logic_class == "nor" ||
	   logic_class == "nand" ||
	   logic_class == "xnor") &&
	  in.size() >= 2 && out.size() == 1) {

    std::string inner_op, outer_op = "!";

    if(logic_class == "nand") inner_op = "&";
    else if(logic_class == "nor") inner_op = "|";
    else if(logic_class == "xnor") inner_op = "^";
    else {
      outer_op = "";
      if(logic_class == "and") inner_op = "&";
      else if(logic_class == "or") inner_op = "|";
    }

    boost::format f("  assign %1% = %2%%3%%4%%5%;"); 
    f % generate_identifier(out[0])
      % outer_op
      % (outer_op.empty() ? "" : "(")
      % boost::algorithm::join(generate_identifier<std::vector<std::string> >(in),
			       std::string(" ") + inner_op + std::string(" "))
      % (outer_op.empty() ? "" : ")");

    return f.str();
  }
  else if(logic_class == "flipflop-async-rst") {
    boost::format f(
      "  -- \n"
      "  -- Please implement behaviour.\n"
      "  -- \n"
      "  -- process(%1%, %2%)\n"
      "  -- begin\n"
      "  --  if %3% = '1' then   -- or '0' if RESET is active low...\n"
      "  --    Q <= '0';\n"
      "  --  elsif rising_edge(%4%) then\n"
      "  --    if Enable = '1' then  -- or '0' if Enable is active low...\n"
      "  --      Q <= D;\n"
      "  --    end if;\n"
      "  --   end if;\n"
      "  -- end process;\n");
    f % clock_name % reset_name
      % reset_name
      % clock_name;
    return f.str();
  }
  else {
    return
      "/*\n"
      " * Please implement behaviour.\n"
      " */\n";
  }
}


std::string VerilogCodeTemplateGenerator::generate_identifier(std::string const& name) const {
  std::string identifier;

  bool first_char = true;
  BOOST_FOREACH(char c, name) {
    if(c == '/' || c == '!') identifier.append("not");
    else if(first_char && !isalpha(c)) {
      //identifier.append("entity_");
      identifier.push_back(c);
    }
    else if(isalnum(c)) identifier.push_back(c);
    else identifier.push_back('_');

    first_char = false;
  }
  return identifier;
}


