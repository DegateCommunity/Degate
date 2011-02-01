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


#include <VerilogTBCodeTemplateGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VerilogTBCodeTemplateGenerator::VerilogTBCodeTemplateGenerator(std::string const& entity_name,
							       std::string const& description,
							       std::string const& logic_class) :
  VerilogCodeTemplateGenerator(entity_name, description, logic_class) {
}

VerilogTBCodeTemplateGenerator::~VerilogTBCodeTemplateGenerator() {
}

std::string VerilogTBCodeTemplateGenerator::generate() const {

  port_map_type port_map;
  BOOST_FOREACH(std::string const& port_name, get_inports()) 
    port_map[port_name] = port_name;
  BOOST_FOREACH(std::string const& port_name, get_outports())
    port_map[port_name] = port_name;

  return
    generate_header() +
    generate_module(entity_name);
			  
}

std::string VerilogTBCodeTemplateGenerator::generate_header() const {
  boost::format f("/**\n"
		  " * This is Verilog testbench for a gate of type %1%\n"
		  " *\n"
		  " *  Please customize this code template according to your needs.\n"
		  " */\n\n"
		  "`timescale 1ns/100ps\n"
		  "\n\n");
  f % entity_name;
  return f.str();
}


std::string VerilogTBCodeTemplateGenerator::generate_module(std::string const& device_type_name) const {
    
  std::string inports = 
    boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_inports()), ", ");
  
  std::string outports = 
    boost::algorithm::join(generate_identifier<std::vector<std::string> >(get_outports()), ", ");
  
  std::list<std::string> port_wiring;
  BOOST_FOREACH(std::string const & pname, 
		generate_identifier(get_ports())) {
    boost::format f(".%1%(%2%)");
    f % pname % pname;
    port_wiring.push_back(f.str());
  }
  
  std::string inport_init;
  BOOST_FOREACH(std::string const & pname, 
		generate_identifier(get_inports())) {
      boost::format f("    %1% <= 1'b0;\n");
      f % pname;
      inport_init += f.str();
  }
  
  boost::format f("module testbench_%1%;\n"
		  "  reg %2%;\n"
		  "  wire %3%;\n"
		  "\n"
		  "  // helper task to simulate a C-style assert() function\n"
		  "  task assert;\n"
		  "    input asserted_value;\n"
		  "    begin\n"
		  "      if(!asserted_value) begin\n"
		  "        $display(\"Assertion failed.\");\n"
		  "        $finish;\n"
		  "      end\n"
		  "    end\n"
		  "  endtask\n"
		  "\n"
		  "  // create an instance of the device to test\n"
		  "  %4% unit(%5%);\n"
		  "\n"
		  "  // initialize\n"
		  "  initial begin\n"
		  "    // Enable signal dumping\n"
		  "    // Generate a VCD dump file. Please, do not change the filename.\n"
		  "    $dumpfile(\"test.vcd\"); \n"
		  "    $dumpvars(0, testbench_%6%); // for this module\n"
		  "\n"
		  "    // initialize signals on ports\n"
		  "%7%"
		  "  end\n"
		  "\n"
		  "  initial begin\n"
		  "    #10; // wait 10 ns for port initialisation\n"
		  "\n"
		  "%8%"
		  "\n"
		  "    #10; // wait 10 ns\n"
		  "\n"
		  "  end\n"
		  "\n"
		  "endmodule // testbench_%9%");
  f % generate_identifier(device_type_name, "dg_")
    % inports
    % outports
    % generate_identifier(device_type_name, "dg_") % boost::algorithm::join(port_wiring, ", ")
    % generate_identifier(device_type_name, "dg_")
    % inport_init
    
    % generate_all_assignments(get_inports(), get_outports())
    
    % generate_identifier(device_type_name, "dg_");

  return f.str();    
}


/**
 * Recursive implementation of an increment function.
 * Starts with MSB and increments until position right to LSB is reached.
 * @return Returns false, if there is nothing to do.
 */
bool increment(std::vector<int> & assignment, size_t pos = 0) {
  if(assignment.size() == 0) return false;

  // toggle lsb
  if(assignment[pos] == 0) {
    assignment[pos] = 1;
    return true;
  }
  else {
    assignment[pos] = 0;
    if(pos + 1 == assignment.size()) return false;
    else return increment(assignment, pos + 1);
  }
      
}

std::string VerilogTBCodeTemplateGenerator::generate_all_assignments
(std::vector<std::string> const& in_port_idents, 
 std::vector<std::string> const& out_port_idents) const {

  // left side: {a, b, c, ... }
  std::string assignment_dst = 
    "{" +
    boost::algorithm::join(generate_identifier<std::vector<std::string> >(in_port_idents), ", ") + 
    "}";

  // right side prefix
  boost::format f_prefix("%1%'b");
  f_prefix % in_port_idents.size();
  std::string prefix = f_prefix.str();


  std::string testcode;

  std::vector<int> assignment(in_port_idents.size()); // initialized with 0
  while(increment(assignment)) {

    std::string assignment2;
    BOOST_FOREACH(int i, assignment) 
      assignment2.push_back(boost::lexical_cast<char>(i));
    std::reverse(assignment2.begin(), assignment2.end());

    // generate assignemt string
    boost::format f("    %1% = %2%%3%;\n"
		    "    #10;\n");
    f % assignment_dst % prefix % assignment2;

    testcode += f.str();
    BOOST_FOREACH(std::string const& oport,
		  generate_identifier<std::vector<std::string> >(out_port_idents)) {
      boost::format f2("    assert(%1% === 1'bX); // please edit\n\n");
      f2 % oport;
      testcode += f2.str();
    }

  }

  return testcode;
}

