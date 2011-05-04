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

#include <ProjectImporter.h>
#include <Project.h>
#include <FileSystem.h>
#include <ImageHelper.h>
#include <DegateHelper.h>

#include <string>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::program_options;
using namespace degate;


/**
 * Main program.
 */

int main(int argc, char ** argv) {

  // Parse program options.

  options_description desc("Options");
  desc.add_options()
    ("help", "Show help message.")
    ("project-dir", value<std::string>(), "Directory of the project to open.")
    ("module-path", value<std::string>(), "Module for which module ports should be determined.")
    ;

  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);    


  if(vm.count("help") || vm.empty() || 
     !vm.count("project-dir") || 
     !vm.count("module-path")) {
    std::cout << desc << std::endl;
    return 1;
  }


  // Import project.

  ProjectImporter importer;
  Project_shptr prj(importer.import_all( vm["project-dir"].as<std::string>() ));

  
  // Run
  LogicModel_shptr lmodel = prj->get_logic_model();
  Module_shptr main_module = lmodel->get_main_module();

  std::string module_path = vm["module-path"].as<std::string>();
  Module_shptr module = main_module->lookup_module(module_path);

  if(!module) {
    std::cout << "Failed to lookup module " << module_path << ".\n\n";
    return 0;
  }
  else std::cout << "Found module " << module_path << ".\n\n";

  if(module->is_main_module())
    determine_module_ports_for_root(lmodel);
  else 
    module->determine_module_ports();
  

  // print gates and their ports and mark module port

  for(Module::gate_collection::const_iterator iter = module->gates_begin();
      iter != module->gates_end(); ++iter) {

    Gate_shptr gate = (*iter);
    std::cout << "+ " << gate->get_descriptive_identifier() << " -- " << gate->get_description() << "\n";

    for(Gate::port_iterator piter = gate->ports_begin(); 
	piter != gate->ports_end(); ++piter) {

      GatePort_shptr gport = *piter;

      std::cout << "  - " << gport->get_descriptive_identifier();
      if(!gport->is_connected()) {
	std::cout << "\t\t[nc]";
      }

      for(Module::port_collection::const_iterator mpiter = module->ports_begin();
	  mpiter != module->ports_end(); ++mpiter) {

	if(gport == mpiter->second)
	  std::cout << "\t\tmodule-port=" << mpiter->first;
      }

      std::cout << "\n";
    }

  }

  // show all module ports
  std::cout << "Module ports:\n";
  for(Module::port_collection::const_iterator mpiter = module->ports_begin();
      mpiter != module->ports_end(); ++mpiter) {
    std::cout << "\t\tmodule-port=" << mpiter->first << " -- " << mpiter->second->get_descriptive_identifier() << "\n";
  }

  return 1;
}


