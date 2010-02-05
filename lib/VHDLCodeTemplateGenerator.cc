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
    generate_entity(entity_name, generate_port_description()) + 
    generate_architecture(entity_name, "", generate_impl(logic_class));
}

std::string VHDLCodeTemplateGenerator::generate_header() const {
  boost::format f("--\n"
		  "-- This is a VHDL implementation for a gate of type %1%\n"
		  "--\n"
		  "-- Please customize this code template according to your needs.\n\n"
		  "library ieee;\n"
		  "use ieee.std_logic_1164.all;\n\n");
  f % entity_name;
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_port_description() const {

  boost::format f("  port(%1% : in std_logic;\n"
		  "       %2% : out std_logic);\n");
  f % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_inports()), ", ")
    % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_outports()), ", ");
  return f.str();
}


std::string VHDLCodeTemplateGenerator::generate_entity(std::string const& entity_name,
						       std::string const& port_description) const {
  
  boost::format f("entity %1% is\n"
		  "%2%"
		  "end %3%;\n\n");
  f % generate_identifier(entity_name) 
    % port_description
    % generate_identifier(entity_name);
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_component(std::string const& entity_name,
								  std::string const& port_description) const {

  boost::format f("  component %1% is\n"
		  "%2%"
		  "  end component;\n\n");
  f % generate_identifier(entity_name) 
    % port_description;
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_impl(std::string const& logic_class) const {

  std::vector<std::string> in = get_inports();
  std::vector<std::string> out = get_outports();

  std::string clock_name = get_clock_port_name();
  if(clock_name.empty()) clock_name = "clock";
  std::string reset_name = get_reset_port_name();
  if(reset_name.empty()) reset_name = "reset";

  if(logic_class == "inverter" &&
     in.size() == 1 && out.size() == 1) {
    boost::format f("  %1% <= not %2%;");
    f % generate_identifier(out[0]) % generate_identifier(in[0]);
    return f.str();
  }
  else if((logic_class == "xor" ||
	   logic_class == "or" ||
	   logic_class == "and" ||
	   logic_class == "nor" ||
	   logic_class == "nand" ||
	   logic_class == "xnor") &&
	  in.size() >= 2 && out.size() == 1) {

    std::string inner_op, outer_op = "not";

    if(logic_class == "nand") inner_op = "and";
    else if(logic_class == "nor") inner_op = "or";
    else if(logic_class == "xnor") inner_op = "xor";
    else {
      outer_op = "";
      inner_op = logic_class;
    }

    boost::format f("  %1% <= %2%%3%%4%%5%;");
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
  else if(logic_class == "flipflop-sync-rst") {
    boost::format f(
      "  -- \n"
      "  -- Please implement behaviour.\n"
      "  -- \n"
      "  -- process(%1%)\n"
      "  -- begin\n"
      "  --   if rising_edge(%2%) then\n"
      "  --     if RESET = '1' then\n"
      "  --       Q <= '0';\n"
      "  --     elsif Enable = '1' then  -- or '0' if Enable is active low...\n"
      "  --       Q <= D;\n"
      "  --     end if;\n"
      "  --   end if;\n"
      "  -- end process;\n");
    f % clock_name % clock_name;
    return f.str();
  }
  else {
    return 
      "  -- \n"
      "  -- Please implement behaviour.\n"
      "  -- \n";
  }
}

std::string VHDLCodeTemplateGenerator::generate_architecture(std::string const& entity_name,
							     std::string const& header,
							     std::string const& impl) const {

  boost::format f("architecture Behavioral of %1% is\n"
		  "%2%\n"
		  "begin\n"
		  "%3%\n"
		  "end Behavioral;\n\n");
  f % generate_identifier(entity_name) 
    % header
    % impl;
  return f.str();
}

std::string VHDLCodeTemplateGenerator::generate_identifier(std::string const& name) const {
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


std::string VHDLCodeTemplateGenerator::generate_instance(std::string const& instance_name,
							 std::string const& instance_type,
							 port_map_type const& port_map) const {

  std::list<std::string> port_map_str;

  BOOST_FOREACH(port_map_type::value_type p, port_map) {
    boost::format m("    %1% => %2%");
    m % generate_identifier(p.first) % generate_identifier(p.second);
    port_map_str.push_back(m.str());
  }

  boost::format f("  %1% : %2% port map (\n%3%\n  );\n\n\n");
  f % generate_identifier(instance_name)
    % generate_identifier(instance_type)
    % boost::algorithm::join(port_map_str, ",\n");
  return f.str();
}
