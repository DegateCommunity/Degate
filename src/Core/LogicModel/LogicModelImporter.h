/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
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

#ifndef __LOGICMODELIMPORTER_H__
#define __LOGICMODELIMPORTER_H__

#include "Globals.h"
#include "LogicModel.h"
#include "Core/XML/XMLImporter.h"

#include <stdexcept>

namespace degate
{
    /**
     * This class implements a logic model loader.
     */
    class LogicModelImporter : public XMLImporter
    {
    private:

        unsigned int width, height;
        GateLibrary_shptr gate_library;

        std::list<Gate_shptr> gates;

        void parse_logic_model_element(QDomElement const lm_element,
                                       LogicModel_shptr lmodel);

        void parse_gates_element(QDomElement const gates_element, LogicModel_shptr lmodel);

        void parse_vias_element(QDomElement const vias_element,
                                LogicModel_shptr lmodel);

        void parse_emarkers_element(QDomElement const emarkers_element,
                                    LogicModel_shptr lmodel);

        void parse_wires_element(QDomElement const wires_element,
                                 LogicModel_shptr lmodel);

        void parse_nets_element(QDomElement const nets_element,
                                LogicModel_shptr lmodel);

        void parse_annotations_element(QDomElement const annotations_element,
                                       LogicModel_shptr lmodel);

        std::list<Module_shptr> parse_modules_element(QDomElement const modules_element,
                                                      LogicModel_shptr lmodel);

    public:

        /**
         * Create a logic model importer.
         * @param width The geometrical width of the logic model.
         * @param height The geometrical height of the logic model.
         * @param gate_library The gate library to resolve references to gate templates.
         *              The gate library is stored into the logic model. You should not set it by yourself.
         */
        LogicModelImporter(unsigned int width, unsigned int height, GateLibrary_shptr gate_library) :
            width(width),
            height(height),
            gate_library(gate_library)
        {
        }


        /**
         * Create a logic model importer. The gate library is not used to resolve references.
         * @param width The geometrical width of the logic model.
         * @param height The geometrical height of the logic model.
         */
        LogicModelImporter(unsigned int width, unsigned int height) :
            width(width),
            height(height)
        {
        }


        /**
         * The destructor.
         */
        ~LogicModelImporter()
        {
        }

        /**
         * import a logic model.
         */
        LogicModel_shptr import(std::string const& filename, ProjectType project_type);


        /**
         * Import a logic model that is stored in a XML file into an existing logic model.
         */
        void import_into(LogicModel_shptr lmodel, std::string const& filename);
    };
}

#endif
