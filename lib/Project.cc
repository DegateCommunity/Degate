/*
 
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

#include "globals.h"
#include "Project.h"

#include <string>

using namespace std;
using namespace degate;

void Project::print(std::ostream & os) {
  os 
    << endl
    << "================================[ " 
    << (name.empty() ? "Unnamed project" : name) 
    << " ]=================================" << endl
    << "+ Project description: " << description << endl
    << "+ Degate version: " << degate_version << endl
    << "+ Directory: " << directory << endl
    << "+" << endl
    << "+ Changed: " << (changed ? "true" : "false") << endl
    << "+ Number of sub projects: " << sub_projects.size() << endl
    << "+" << endl
    << "+ Default wire diameter: " << default_wire_diameter << endl
    << "+ Default pin diameter: " << default_pin_diameter << endl
    << "+ Min distance between electrically isolated objects in pixel (lambda value): " << lambda << endl
    << endl;

}

void Project::print_all(std::ostream & os) {
  print(os);
  if(logic_model == NULL) {
    os << "+ The project has no logic model." << endl
       << endl;
  }
  else {
    logic_model->print(os);
  }
}
