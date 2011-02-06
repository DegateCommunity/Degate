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

#ifndef __PROJECTEXPORTER_H__
#define __PROJECTEXPORTER_H__

#include <degate.h>
#include <XMLExporter.h>
#include <Project.h>

#include <stdexcept>

namespace degate {

  /**
   * The ProjectExporter exports a degate project.
   *
   */

  class ProjectExporter : public XMLExporter {

  private:

    void set_project_node_attributes(xmlpp::Element* prj_elem,
				     Project_shptr prj) throw(std::runtime_error );

    void add_grids(xmlpp::Element* prj_elem,
		   Project_shptr prj) throw(std::runtime_error );

    void add_regular_grid(xmlpp::Element* grids_elem,
			  const RegularGrid_shptr grid,
			  std::string const & grid_orientation) throw(std::runtime_error );;

    void add_irregular_grid(xmlpp::Element* grids_elem,
			    const IrregularGrid_shptr grid,
			    std::string const & grid_orientation) throw(std::runtime_error );;

    void add_layers(xmlpp::Element* prj_elem,
		    LogicModel_shptr lmodel,
		    std::string const& project_dir) throw(InvalidPointerException, std::runtime_error );

    void add_port_colors(xmlpp::Element* prj_elem,
			 PortColorManager_shptr port_color_manager) throw(InvalidPointerException, std::runtime_error );

    void add_colors(xmlpp::Element* prj_elem, Project_shptr prj);

  public:
    ProjectExporter() {}
    ~ProjectExporter() {}

    void export_data(std::string const& filename, Project_shptr prj)
      throw( InvalidPathException, InvalidPointerException, std::runtime_error );

    void export_all(std::string const& project_directory, Project_shptr prj,
		    bool enable_oid_rewrite = true,
		    std::string const& project_file = "project.xml",
		    std::string const& lmodel_file = "lmodel.xml",
		    std::string const& gatelib_file = "gate_library.xml",
		    std::string const& rcbl_file = "rc_blacklist.xml")
      throw( InvalidPathException, InvalidPointerException, std::runtime_error );

  };

}

#endif
