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

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <degate.h>
#include <globals.h>
#include <LogicModel.h>
#include <PortColorManager.h>

#include <string>
#include <list>
#include <tr1/memory>

#include <ctime>

namespace degate {

  class Project;
  typedef std::list<std::tr1::shared_ptr<Project> > ProjectList;
  typedef std::tr1::shared_ptr<Project> Project_shptr;
}

#include "ProjectImporter.h"

namespace degate {

  /**
   * The project class is a container for project related data.
   *
   */
  
  class Project {
    
  private:
    
    BoundingBox bounding_box;
    
    std::string name;
    std::string description;
    std::string degate_version;
    std::string directory;
    
    RegularGrid_shptr regular_horizontal_grid;
    RegularGrid_shptr regular_vertical_grid;
    IrregularGrid_shptr irregular_horizontal_grid;
    IrregularGrid_shptr irregular_vertical_grid;
  
    bool changed;
    clock_t last_persistent_version;
  
    diameter_t default_pin_diameter;
    diameter_t default_wire_diameter;
    length_t lambda;
    
    ProjectList sub_projects;
    
    LogicModel_shptr logic_model;

    PortColorManager_shptr port_color_manager;

  private:

    void init_default_valus() {
      default_pin_diameter = 5;
      default_wire_diameter = 5;
      lambda = 5;

      set_changed(false);

      regular_horizontal_grid = RegularGrid_shptr(new RegularGrid(Grid::HORIZONTAL));
      regular_vertical_grid = RegularGrid_shptr(new RegularGrid(Grid::VERTICAL));
      irregular_horizontal_grid = IrregularGrid_shptr(new IrregularGrid(Grid::HORIZONTAL));
      irregular_vertical_grid = IrregularGrid_shptr(new IrregularGrid(Grid::VERTICAL));
    }

  public:

    /**
     * Create a new and empty project.
     * It will create an empty logic model as well.
     */
    Project(length_t width, length_t height) : 
      bounding_box(width, height),
      logic_model(new LogicModel(width, height)),
      port_color_manager(new PortColorManager()) {
      init_default_valus();
    }
    
    /**
     * Create a new and empty project.
     * It will create an empty logic model as well.
     */
    Project(length_t width, length_t height, std::string const& _directory, unsigned int layers = 0) : 
      bounding_box(width, height),
      directory(_directory),
      logic_model(new LogicModel(width, height, layers)),
      port_color_manager(new PortColorManager()) {
      init_default_valus();
    }

    
    /**
     * Set the project directory.
     */
    void set_project_directory(std::string const& _directory) {
      directory = _directory;
    }
    
    /**
     * Get the project directory.
     */
    std::string const& get_project_directory() {
      return directory;
    }
    
    /**
     * Get the bounding box that represents the covered area for this project.
     */
    BoundingBox const& get_bounding_box() const { return bounding_box; }
    
    /**
     * Get the width of the project.
     */
    int get_width() const { return bounding_box.get_width(); }
    
    /**
     * get the height of a project.
     */
    int get_height() const { return bounding_box.get_height(); }
    
    /**
     * Get the logic model. The logic model should be present all time. This means
     * that you can call this method and should not receive a NULL pointer.
     */
    LogicModel_shptr get_logic_model() { 
      return logic_model; 
    }
    
    /**
     * Set the logic model for a project.
     * If you reset the logic model, the old logic model will be destroyed.
     */
    
    void set_logic_model(LogicModel_shptr _logic_model) {
      logic_model = _logic_model;
    }
    
    
    void set_name(std::string _name) { name = _name; }
    std::string const& get_name() const { return name; }
    
    void set_description(std::string _description) { description = _description; }
    std::string const& get_description() const { return description; }
    
    void set_degate_version(std::string version_str) { degate_version = version_str; }
    std::string const&get_degate_version() { return degate_version; }
    
    ProjectList & get_sub_projects() { return sub_projects; }
    void add_sub_project(Project_shptr prj) { sub_projects.push_back(prj); }
    
    void set_lambda(length_t l) { lambda = l; }
    length_t get_lambda() const { return lambda; }
    
    void set_default_pin_diameter(diameter_t pin_diameter) { default_pin_diameter = pin_diameter; }
    diameter_t get_default_pin_diameter() const { return default_pin_diameter; }
    
    void set_default_wire_diameter(diameter_t wire_diameter) { default_wire_diameter = wire_diameter; }
    diameter_t get_default_wire_diameter() const { return default_wire_diameter; }
    

    /**
     * Set changed flag to indicate that the project data was changed.
     *
     * This method is normally called from the GUI.
     * If the project was saved to disc, the code should call set_changed(false). Project
     * saving is implemented in ProjectExporter, but because the ProjectExporter might be used
     * for different purposes, this method must be called from the GUI code.
     */
    void set_changed(bool state = true) { 
      changed = state; 
      if(state == false)
	reset_last_saved_counter();
    }

    /**
     * Check if the project was changed.
     */
    bool is_changed() const { return changed; }
    
    /**
     * Get time since last "save".
     * @return Returns the time in seconds since the project change state was set to false.
     * @see set_changed()
     */

    unsigned int get_time_since_last_save() const {
      return (clock() - last_persistent_version)/CLOCKS_PER_SEC;
    }

    /**
     * Reset last save counter.
     */
    void reset_last_saved_counter() {
      last_persistent_version = clock();
    }


    RegularGrid_shptr get_regular_horizontal_grid() { return regular_horizontal_grid; }
    RegularGrid_shptr get_regular_vertical_grid() { return regular_vertical_grid; }
    IrregularGrid_shptr get_irregular_horizontal_grid() { return irregular_horizontal_grid; }
    IrregularGrid_shptr get_irregular_vertical_grid() { return irregular_vertical_grid; }
    
    PortColorManager_shptr get_port_color_manager() { return port_color_manager; }

    /**
     * Dump basic meta data for the project as human readable text into an ostream.
     */
    
    void print(std::ostream &);
    
    /**
     * Dump most meta data for the project as human readable text into an ostream.
     */
    
    void print_all(std::ostream &);
  };
  
}

#endif
