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

#include <AppHelper.h>
#include <boost/filesystem.hpp>

using namespace degate;
using namespace boost::filesystem;

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

bool autosave_project(Project_shptr project, time_t interval) {

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

    path project_dir(project->get_project_directory());

    if(exists(project_dir / path(".project.xml"))) remove(project_dir / path(".project.xml"));
    if(exists(project_dir / path(".gate_library.xml"))) remove(project_dir / path(".gate_library.xml"));
    if(exists(project_dir / path(".lmodel.xml"))) remove(project_dir / path(".lmodel.xml"));

    create_symlink(path(prefix + project_file), project_dir / path(".project.xml"));
    create_symlink(path(prefix + lmodel_file), project_dir / path(".lmodel.xml"));
    create_symlink(path(prefix + gatelib_file), project_dir / path(".gate_library.xml"));

    project->reset_last_saved_counter();
    
    return true;
  }
  return false;
}

bool check_for_autosaved_project(boost::filesystem::path const& project_dir) {
  if(!is_symlink(project_dir / path(".project.xml")) ||
     !is_symlink(project_dir / path(".lmodel.xml")) ||
     !is_symlink(project_dir / path(".gate_library.xml")))
    return false;

  if(last_write_time(project_dir / path(".project.xml")) > last_write_time(project_dir / path("project.xml")) ||
     last_write_time(project_dir / path(".lmodel.xml")) > last_write_time(project_dir / path("lmodel.xml")) ||
     last_write_time(project_dir / path(".gate_library.xml")) > last_write_time(project_dir / path("gate_library.xml")))
    return true;

  return false;
}

void restore_autosaved_project(boost::filesystem::path const& project_dir) {
  if(exists(project_dir / path("project.xml"))) remove(project_dir / path("project.xml"));
  if(exists(project_dir / path("gate_library.xml"))) remove(project_dir / path("gate_library.xml"));
  if(exists(project_dir / path("lmodel.xml"))) remove(project_dir / path("lmodel.xml"));

  copy_file(project_dir / path(".project.xml"), project_dir / path("project.xml"));
  copy_file(project_dir / path(".lmodel.xml"), project_dir / path("lmodel.xml"));
  copy_file(project_dir / path(".gate_library.xml"), project_dir / path("gate_library.xml"));
}


void add_image_file_filter_to_file_chooser(Gtk::FileChooserDialog & dialog) {
  Gtk::FileFilter filter;
  filter.set_name("Tiff image");
  filter.add_pattern("*.tif");
  filter.add_pattern("*.tiff");
  dialog.add_filter(filter);
}


