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

#include <string>

#include <boost/program_options.hpp>

using namespace boost::program_options;
using namespace degate;

// function prototypes

void dump(Project_shptr prj);

/**
 * Main program.
 */

int main(int argc, char ** argv) {

  // Parse program options.

  options_description desc("Options");
  desc.add_options()
    ("help", "Show help message.")
    ("project-dir", value<std::string>(), "Directory of the project to open.")
    ;

  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);    


  if(vm.count("help") || vm.empty()) {
    std::cout << desc << std::endl;
    return 1;
  }


  // Import project.

  ProjectImporter importer;
  Project_shptr prj(importer.import_all( vm["project-dir"].as<std::string>() ));


  // Dump project data.
  dump(prj);

  return 1;
}


void dump(Project_shptr prj) {
}

