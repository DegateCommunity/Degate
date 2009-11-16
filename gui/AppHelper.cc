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

#include <AppHelper.h>

using namespace degate;

std::string get_date_and_time_as_file_prefix() {
  time_t tim = time(NULL);
  tm *now = localtime(&tim);
    
  boost::format f("%4d-%02d-%02d_%02d%02d_");
  f 
    % (now->tm_year+1900)
    % (now->tm_mon+1)
    % now->tm_mday
    % now->tm_hour
    % now->tm_min;
  return f.str();
}

bool autosave_project(Project_shptr project, unsigned int interval) {

  if(project->is_changed() &&
     project->get_time_since_last_save() >= interval) {

    std::string prefix(get_date_and_time_as_file_prefix());

    std::string project_file = "project.xml";
    std::string lmodel_file = "lmodel.xml";
    std::string gatelib_file = "gate_library.xml";

    ProjectExporter exporter;
    exporter.export_all(project->get_project_directory(), 
			project, false,
			prefix + project_file, 
			prefix + lmodel_file, 
			prefix + gatelib_file);

    project->reset_last_saved_counter();
    
    return true;
  }
  return false;
}
