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
#include "Globals.h"
#include "Core/LogicModel/Gate/GateLibraryImporter.h"
#include "Core/LogicModel/LogicModelImporter.h"

#include "catch.hpp"

using namespace degate;

TEST_CASE("Test import", "[LogicModelImporter]")
{
    GateLibraryImporter gate_library_importer;
    std::string gl_filename("tests_files/test_project/gate_library.xml");
    GateLibrary_shptr glib(gate_library_importer.import(gl_filename));

    // If this fail, need to add the 'tests_files' folder beside the tests executable.
    REQUIRE(glib != nullptr);


    LogicModelImporter lm_importer(500, 500, glib);
    std::string filename("tests_files/test_project/lmodel.xml");
    LogicModel_shptr lmodel(lm_importer.import(filename, ProjectType::Normal));

    // If this fail, need to add the 'tests_files' folder beside the tests executable.
    REQUIRE(lmodel != nullptr);

    for(auto iter = lmodel->objects_begin(); iter != lmodel->objects_end(); ++iter)
    {
        PlacedLogicModelObject_shptr o = (*iter).second;

        REQUIRE(o != nullptr);
        REQUIRE(o->has_valid_object_id());
        REQUIRE((*iter).first == o->get_object_id());
        REQUIRE(o->get_layer() != nullptr);
    }

    // try to reuse the importer
    LogicModel_shptr lmodel2(lm_importer.import(filename, ProjectType::Normal));
    REQUIRE(lmodel2 != nullptr);
}
