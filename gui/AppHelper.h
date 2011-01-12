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

#ifndef __APPHELPER_H__
#define __APPHELPER_H__

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <Project.h>
#include <ProjectExporter.h>
#include <Editor.h>
#include <DegateRenderer.h>
#include <gtkmm.h>

/**
 * Get date and time as a string that can be used to generate file names.
 */

std::string get_date_and_time_as_file_prefix();


/**
 * Autosave a project.
 * @param project Shared pointer to the project.
 * @parem interval Minimum time in seconds. If you pass a zero, autosave is enforced.
 * @return Returns true if the project was saved. Returns false, if there
 *   is nothing to save.
 */

bool autosave_project(degate::Project_shptr project, time_t interval = 5 * 60);

/**
 * Add file filter for background images to a Gtk::FileChooserDialog.
 * @todo Lookup available image importer.
 */

void add_image_file_filter_to_file_chooser_for_reading(Gtk::FileChooserDialog & dialog);

/**
 * Add file filter for background images to a Gtk::FileChooserDialog.
 * @todo Lookup available image importer.
 */

void add_image_file_filter_to_file_chooser_for_writing(Gtk::FileChooserDialog & dialog);

/**
 * Check if there are autosaved files in \p project_dir that are newer than the
 * normal project files.
 */
bool check_for_autosaved_project(boost::filesystem::path const& project_dir);

/**
 * Restore project files from the last autosaved project files.
 */
void restore_autosaved_project(boost::filesystem::path const& project_dir);


/**
 * Get the selected area as bounding box. If nothing is selected a bounding box
 * is retunred that represents the whole project area.
 */
degate::BoundingBox get_selection_bounding_box(GfxEditor<DegateRenderer> const& editor,
					       const degate::Project_shptr project);

/**
 * Snap bounding box to a grid, that is referenced in a project.
 * @param corridor_size Snap bottom or right edge of the bounding box, so that
 *   width or height gets equal to the corridor size. If this value is 0, the bottom
 *   or right side is left untouched.
 * @return Returns the type of grid orientation, which matches.
 */
degate::Grid::ORIENTATION snap_upper_or_left_edge_to_grid(const degate::Project_shptr project, 
				     degate::BoundingBox & bbox,
				     int corridor_size);

/**
 * Check if either the horizontal or the vertical grid is enabled.
 * If both grid types are enabled, false will be returned.
 */
bool check_grid_either_horizontal_or_vertical(const degate::Project_shptr project);

#endif
