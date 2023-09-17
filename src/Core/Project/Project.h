/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2012 Robert Nitsch
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include "Core/Primitive/DeepCopyable.h"
#include "Globals.h"
#include "Core/LogicModel/LogicModel.h"
#include "Core/Project/PortColorManager.h"
#include "Core/RuleCheck/RCBase.h"

#include <boost/date_time.hpp>

#include <string>
#include <list>
#include <memory>
#include <vector>

#include <ctime>

namespace degate
{
    class Project;
    typedef std::list<std::shared_ptr<Project>> ProjectList;
    typedef std::shared_ptr<Project> Project_shptr;
}

#include "ProjectImporter.h"
#include "Core/Grid/RegularGrid.h"
#include "Core/Grid/IrregularGrid.h"

namespace degate
{
    /**
     * Transform a string to ProjectType enum element.
     */
    inline ProjectType to_project_type(std::string string_type)
    {
        if (string_type == "Normal")
            return ProjectType::Normal;
        else if (string_type == "Attached")
            return ProjectType::Attached;
        else
            return ProjectType::Normal;
    }

    /**
     * Transform a ProjectType enum element to string.
     */
    inline std::string from_project_type(ProjectType type)
    {
        switch (type)
        {
            case ProjectType::Normal: return "Normal";
            case ProjectType::Attached: return "Attached";
            default: return "Normal";
        }
    }

    struct ProjectSnapshot
    {
        boost::posix_time::ptime datetime;
        std::string title;
        Project_shptr clone;
        bool automatic;
    };

    typedef std::shared_ptr<ProjectSnapshot> ProjectSnapshot_shptr;

    /**
     * The project class is a container for project related data.
     *
     */
    class Project : public DeepCopyable
    {
    private:

        BoundingBox bounding_box;


        std::string name;
        std::string description;
        std::string degate_version;
        std::string directory;
        std::string server_url;

        RegularGrid_shptr regular_horizontal_grid;
        RegularGrid_shptr regular_vertical_grid;
        IrregularGrid_shptr irregular_horizontal_grid;
        IrregularGrid_shptr irregular_vertical_grid;

        bool changed;
        time_t last_persistent_version;

        diameter_t default_via_diameter;
        diameter_t default_wire_diameter;
        diameter_t default_port_diameter;
        length_t lambda;

        transaction_id_t last_transaction_id;

        LogicModel_shptr logic_model;

        PortColorManager_shptr port_color_manager;

        default_colors_t default_colors;

        double pixel_per_um;
        int template_dimension;

        RCBase::container_type rcv_blacklist;

        unsigned int font_size;

        ProjectType project_type;

    private:

        void init_default_values();

    public:

        /**
         * Create a new and empty project.
         * It will create an empty logic model as well.
         */
        Project(length_t width, length_t height, ProjectType project_type = ProjectType::Normal);

        /**
         * Create a new and empty project.
         * It will create an empty logic model as well.
         */
        Project(length_t width, length_t height, std::string const& directory, ProjectType project_type = ProjectType::Normal, unsigned int layers = 0);


        /**
         * The destructor.
         */
        virtual ~Project();

        //@{
        DeepCopyable_shptr clone_shallow() const;
        void clone_deep_into(DeepCopyable_shptr destination, oldnew_t* oldnew) const;
        //@}

        /**
         * Set the project directory.
         */
        void set_project_directory(std::string const& directory);

        /**
         * Get the project directory.
         */
        std::string const& get_project_directory();

        /**
         * Get the bounding box that represents the covered area for this project.
         */
        BoundingBox const& get_bounding_box() const;

        /**
         * Get the width of the project.
         */
        unsigned int get_width() const;

        /**
         * Get the height of a project.
         */
        unsigned int get_height() const;

        /**
         * Update the project size.
         * It needs to be equal or bigger as the previous one.
         * Also, it needs a project reopen.
         *
         * @param width : the new width of the project.
         * @param height : the new height of the project.
         *
         * @return Returns true if the size was updated, false otherwise.
         */
        bool update_size(unsigned int width, unsigned int height);

        /**
         * Get the logic model. The logic model should be present all time. This means
         * that you can call this method and should not receive a nullptr pointer.
         */
        LogicModel_shptr get_logic_model();

        /**
         * Set the logic model for a project.
         * If you reset the logic model, the old logic model will be destroyed.
         */
        void set_logic_model(LogicModel_shptr logic_model);


        void set_name(const std::string& name);
        std::string const& get_name() const;

        void set_description(std::string description);
        std::string const& get_description() const;

        void set_degate_version(std::string version_str);
        std::string const& get_degate_version();

        void set_lambda(length_t l);
        length_t get_lambda() const;

        void set_default_via_diameter(diameter_t via_diameter);
        diameter_t get_default_via_diameter() const;

        void set_default_wire_diameter(diameter_t wire_diameter);
        diameter_t get_default_wire_diameter() const;

        void set_default_port_diameter(diameter_t port_diameter);
        diameter_t get_default_port_diameter() const;


        /**
         * Set the default color for an entity.
         */
        void set_default_color(ENTITY_COLOR e, color_t c);

        /**
         * Get the default color for an entity.
         * @return Returns the default color. If there is no default setting
         *   0 is returned.
         */
        color_t get_default_color(ENTITY_COLOR e) const;

        /**
         * Get all color defaults.
         */
        const default_colors_t get_default_colors() const;

        /**
         * Set changed flag to indicate that the project data was changed.
         *
         * This method is normally called from the GUI.
         * If the project was saved to disc, the code should call set_changed(false). Project
         * saving is implemented in ProjectExporter, but because the ProjectExporter might be used
         * for different purposes, this method must be called from the GUI code.
         */
        void set_changed(bool state = true);

        /**
         * Check if the project was changed.
         */
        bool is_changed() const;

        /**
         * Get time since last "save".
         * @return Returns the time in seconds since the project change state was set to false.
         * @see set_changed()
         */
        time_t get_time_since_last_save() const;

        /**
         * Reset last save counter.
         */
        void reset_last_saved_counter();


        RegularGrid_shptr get_regular_horizontal_grid();
        RegularGrid_shptr get_regular_vertical_grid();
        IrregularGrid_shptr get_irregular_horizontal_grid();
        IrregularGrid_shptr get_irregular_vertical_grid();

        PortColorManager_shptr get_port_color_manager();

        /**
         * Dump basic meta data for the project as human readable text into an ostream.
         */
        void print(std::ostream&);

        /**
         * Dump most meta data for the project as human readable text into an ostream.
         */
        void print_all(std::ostream&);

        /**
         * Get the ratio of pixels per micrometer.
         */
        double get_pixel_per_um() const;

        /**
         * Get the ratio of pixels per micrometer.
         */
        void set_pixel_per_um(double pix_per_um);


        /**
         * Get the template dimension. This is either the height or the width of a gate (template).
         */
        int get_template_dimension() const;

        /**
         * Set the template dimension.
         */
        void set_template_dimension(int template_dimension);

        /**
         * Set the default font size.
         */
        void set_font_size(unsigned int font_size);

        /**
         * Get the default font size.
         */
        unsigned int get_font_size() const;

        /**
         * Get a list of blacklisted Rule Check violations.
         */
        RCBase::container_type& get_rcv_blacklist();

        /**
         * Get the type of the project (attached or normal).
         */
        ProjectType get_project_type() const;
    };
}

#endif
