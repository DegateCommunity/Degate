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


#include <VHDLTBCodeTemplateGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VHDLTBCodeTemplateGenerator::VHDLTBCodeTemplateGenerator(std::string const& entity_name,
							 std::string const& description,
							 std::string const& logic_class) :
  VHDLCodeTemplateGenerator(entity_name, description, logic_class) {
}

VHDLTBCodeTemplateGenerator::~VHDLTBCodeTemplateGenerator() {
}

std::string VHDLTBCodeTemplateGenerator::generate() const {

  std::string tb_entity_name("testbench_");
  tb_entity_name += entity_name;

  port_map_type port_map;
  BOOST_FOREACH(std::string const& port_name, get_inports()) port_map[port_name] = port_name;
  BOOST_FOREACH(std::string const& port_name, get_outports()) port_map[port_name] = port_name;


  std::string clock_process_impl;
  std::string clock_signal_name = get_clock_port_name();
  std::cout << "clock signal is : " << clock_signal_name << std::endl;
  if(!clock_signal_name.empty()) clock_process_impl += generate_clock_process(clock_signal_name);

  return
    generate_header() +
    generate_entity(tb_entity_name) +
    generate_architecture(tb_entity_name,

			  generate_component(entity_name,
					     generate_port_description()) +
			  generate_signals(),

			  generate_instance("uut", entity_name, port_map) +
			  clock_process_impl +
			  generate_impl(logic_class));
}

std::string VHDLTBCodeTemplateGenerator::generate_header() const {
  boost::format f("--\n"
		  "-- This is VHDL testbench for a gate of type %1%\n"
		  "--\n"
		  "-- Please customize this code template according to your needs.\n\n"
		  "library ieee;\n"
		  "use ieee.std_logic_1164.all;\n\n");
  f % entity_name;
  return f.str();
}

std::string VHDLTBCodeTemplateGenerator::generate_impl(std::string const& logic_class) const {

  boost::format f("  tb_proc : PROCESS is \n"
		  "  BEGIN\n\n"

		  "    -- \n"
		  "    -- Please implement test.\n"
		  "    -- \n\n"

		  "    -- signal_name_a << '0';\n"
		  "    -- signal_name_b << '0';\n"
		  "    -- wait for clock_duration; \n"
		  "    -- assert (signal_name_y == '0') report \"Got unexpected output.\" severity note;\n\n"
		  "  END PROCESS tb_proc;\n\n\n");

  return f.str();
}

std::string VHDLTBCodeTemplateGenerator::generate_signals() const {

  boost::format f("  signal %1% : in std_logic;\n"
		  "  signal %2% : out std_logic;\n");
  f % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_inports()), ", ")
    % boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_outports()), ", ");
  return f.str();
}

std::string VHDLTBCodeTemplateGenerator::generate_clock_process(std::string const& clock_signal_name) const {
  boost::format f("  constant clock_duration : time := 10 ns;\n\n"
		  "  clk_proc : PROCESS (%1%)\n"
		  "  BEGIN\n"
		  "    IF rising_edge(%2%) then\n"
		  "      %3% <= '0' AFTER clock_duration;\n"
		  "  ELSE\n"
		  "    %4% <= '1' AFTER clock_duration;\n"
		  "  END IF;\n"
		  "  END PROCESS clk_proc;\n\n\n");
  f % clock_signal_name
    % clock_signal_name
    % clock_signal_name
    % clock_signal_name;

  return f.str();
}
