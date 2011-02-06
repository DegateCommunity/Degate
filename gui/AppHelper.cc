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
    std::string rcvbl_file = "rc_blacklist.xml";

    ProjectExporter exporter;
    exporter.export_all(project->get_project_directory(),
			project, false,
			prefix + project_file,
			prefix + lmodel_file,
			prefix + gatelib_file,
			prefix + rcvbl_file);

    path project_dir(project->get_project_directory());

    if(exists(project_dir / path("." + project_file))) remove(project_dir / path("." + project_file));
    if(exists(project_dir / path("." + gatelib_file))) remove(project_dir / path("." + gatelib_file));
    if(exists(project_dir / path("." + lmodel_file))) remove(project_dir / path("." + lmodel_file));
    if(exists(project_dir / path("." + rcvbl_file))) remove(project_dir / path("." + rcvbl_file));

    create_symlink(path(prefix + project_file), project_dir / path("." + project_file));
    create_symlink(path(prefix + lmodel_file), project_dir / path("." + lmodel_file));
    create_symlink(path(prefix + gatelib_file), project_dir / path("." + gatelib_file));
    create_symlink(path(prefix + rcvbl_file), project_dir / path("." + rcvbl_file));

    project->reset_last_saved_counter();

    return true;
  }
  return false;
}

bool check_for_autosaved_project(boost::filesystem::path const& project_dir) {

  if(!is_symlink(project_dir / path(".project.xml")) ||
     !is_symlink(project_dir / path(".lmodel.xml")) ||
     !is_symlink(project_dir / path(".gate_library.xml")) ||
     !is_symlink(project_dir / path(".gate_library.xml")) ||

     !is_symlink(project_dir / path("project.xml")) ||
     !is_symlink(project_dir / path("lmodel.xml")) ||
     !is_symlink(project_dir / path("gate_library.xml")) ||
     !is_symlink(project_dir / path("gate_library.xml")) 
     ) return false;

  if(last_write_time(project_dir / path(".project.xml")) > last_write_time(project_dir / path("project.xml")) ||
     last_write_time(project_dir / path(".lmodel.xml")) > last_write_time(project_dir / path("lmodel.xml")) ||
     last_write_time(project_dir / path(".gate_library.xml")) > last_write_time(project_dir / path("gate_library.xml")) ||
     last_write_time(project_dir / path(".rc_blacklist.xml")) > last_write_time(project_dir / path("rc_blacklist.xml")) )
    return true;

  return false;
}

void restore_autosaved_project(boost::filesystem::path const& project_dir) {
  if(exists(project_dir / path("project.xml"))) remove(project_dir / path("project.xml"));
  if(exists(project_dir / path("gate_library.xml"))) remove(project_dir / path("gate_library.xml"));
  if(exists(project_dir / path("lmodel.xml"))) remove(project_dir / path("lmodel.xml"));
  if(exists(project_dir / path("rc_blacklist.xml"))) remove(project_dir / path("rc_blacklist.xml"));

  copy_file(project_dir / path(".project.xml"), project_dir / path("project.xml"));
  copy_file(project_dir / path(".lmodel.xml"), project_dir / path("lmodel.xml"));
  copy_file(project_dir / path(".gate_library.xml"), project_dir / path("gate_library.xml"));
  copy_file(project_dir / path(".rc_blacklist.xml"), project_dir / path("rc_blacklist.xml"));
}


void add_image_file_filter_to_file_chooser_for_reading(Gtk::FileChooserDialog & dialog) {

  Gtk::FileFilter filter;
  filter.set_name("Any image");
  filter.add_pattern("*.tif");
  filter.add_pattern("*.tiff");
  filter.add_pattern("*.jpg");
  filter.add_pattern("*.jpeg");
  dialog.add_filter(filter);

  Gtk::FileFilter filter_tiff;
  filter_tiff.set_name("Tiff image");
  filter_tiff.add_pattern("*.tif");
  filter_tiff.add_pattern("*.tiff");
  dialog.add_filter(filter_tiff);

  Gtk::FileFilter filter_jpeg;
  filter_jpeg.set_name("Jpeg image");
  filter_jpeg.add_pattern("*.jpg");
  filter_jpeg.add_pattern("*.jpeg");
  dialog.add_filter(filter_jpeg);
}



void add_image_file_filter_to_file_chooser_for_writing(Gtk::FileChooserDialog & dialog) {

  Gtk::FileFilter filter;
  filter.set_name("Any image");
  filter.add_pattern("*.tif");
  filter.add_pattern("*.tiff");
  dialog.add_filter(filter);

  Gtk::FileFilter filter_tiff;
  filter_tiff.set_name("Tiff image");
  filter_tiff.add_pattern("*.tif");
  filter_tiff.add_pattern("*.tiff");
  dialog.add_filter(filter_tiff);
}



degate::BoundingBox get_selection_bounding_box(GfxEditor<DegateRenderer> const& editor,
					       const degate::Project_shptr project) {

  // get bounding box for working area
  const std::tr1::shared_ptr<GfxEditorToolSelection<DegateRenderer> > selection_tool =
    std::tr1::dynamic_pointer_cast<GfxEditorToolSelection<DegateRenderer> >(editor.get_tool());

  BoundingBox bbox;

  if(selection_tool != NULL && selection_tool->has_selection()) // selected area
    bbox = selection_tool->get_bounding_box();
  else // the whole area
    bbox = project->get_bounding_box();

  return bbox;
}


bool snap_upper_or_left_edge_to_grid(const degate::Grid_shptr grid, 
				     degate::BoundingBox & bbox,
				     int corridor_size) {
  
  if(!grid->is_enabled()) return false;

  // set left or top edge
  if(grid->is_horizontal())
    bbox.set_min_x(grid->snap_to_grid(bbox.get_min_x()));
  else 
    bbox.set_min_y(grid->snap_to_grid(bbox.get_min_y()));

  // set right or bottom edge
  if(corridor_size > 0) {
    if(grid->is_horizontal()) {
      bbox.set_max_x(bbox.get_min_x() + corridor_size);
    }
    else {
      bbox.set_max_y(bbox.get_min_y() + corridor_size);    
    }
  }

  return true;
}

degate::Grid::ORIENTATION snap_upper_or_left_edge_to_grid(const degate::Project_shptr project, 
							  degate::BoundingBox & bbox,
							  int corridor_size) {

  Grid_shptr grid;

  grid = project->get_regular_horizontal_grid();
  if(snap_upper_or_left_edge_to_grid(grid, bbox, corridor_size)) 
    return Grid::HORIZONTAL;

  grid = project->get_regular_vertical_grid();
  if(snap_upper_or_left_edge_to_grid(grid, bbox, corridor_size)) 
    return Grid::VERTICAL;

  grid = project->get_irregular_horizontal_grid();
  if(snap_upper_or_left_edge_to_grid(grid, bbox, corridor_size)) 
    return Grid::HORIZONTAL;

  grid = project->get_irregular_vertical_grid();
  if(snap_upper_or_left_edge_to_grid(grid, bbox, corridor_size)) 
    return Grid::VERTICAL;

  return Grid::UNDEFINED;
}


bool check_grid_either_horizontal_or_vertical(const degate::Project_shptr project) {
  Grid_shptr grid1 = project->get_regular_horizontal_grid();
  Grid_shptr grid2 = project->get_irregular_horizontal_grid();

  Grid_shptr grid3 = project->get_regular_vertical_grid();
  Grid_shptr grid4 = project->get_irregular_vertical_grid();

  bool h = grid1->is_enabled() || grid2->is_enabled();
  bool v = grid3->is_enabled() || grid4->is_enabled();
  
  if(h == false && v == false) return false;
  else return  h != v;
}

