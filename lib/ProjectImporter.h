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

#ifndef __PROJECTIMPORTER_H__
#define __PROJECTIMPORTER_H__

#include "globals.h"
#include "Project.h"
#include "XMLImporter.h"

#include <stdexcept>

namespace degate {

/**
 * Parser for degate's project files.
 *
 * The ProjectImporter parses the degate's project files. Basically this is the
 * file project.xml, that is present in every degate project directory.
 * The ProjectImporter loads associated files, e.g. the logic model file and
 * the gate library, as well.
 */
class ProjectImporter : public XMLImporter {

private:
	
  void parse_project_element(Project_shptr parent_prj, const xmlpp::Element * const project_node);
  void parse_grids_element(const xmlpp::Element * const project_node, Project_shptr prj);
  void parse_layers_element(const xmlpp::Element * const layers_node, Project_shptr prj);
  void parse_port_colors_element(const xmlpp::Element * const port_colors_elem, Project_shptr prj);

  void parse_colors_element(const xmlpp::Element * const port_colors_elem, Project_shptr prj);
  
  std::string get_project_filename(std::string const& dir) const;

  /**
   * Load a background image and set it to the layer. In case of a conversion
   * from old  single file images to tile based images, the new image is stored
   * in the project directory.
   */
  void load_background_image(Layer_shptr layer, 
			     std::string const& image_filename,
			     Project_shptr prj);

public:
  ProjectImporter() {}
  ~ProjectImporter() {}
	
  /**
   * Import a degate project.
   * @param path The parameter path specifies the project directory
   *             or the path to the project.xml file. It is determined automatically.
   * @exception InvalidPathException This exception is raised if \p path does not exists.
   * @exception InvalidXMLException This exception is raised if there is a parsing error.
   * @return Returns a pointer to a project object.
   */
  Project_shptr import(std::string const& path);

  /**
   * Import a complete degate project, including the default gate library and the logic model.
   * @param path The parameter path specifies the project directory
   *             or the path to the project.xml file. It is determined automatically.
   * @exception std::runtime_error If there are parsing problems.
   * @return Returns a pointer to a project object.
   */
  Project_shptr import_all(std::string const& path);

};

}

#endif
