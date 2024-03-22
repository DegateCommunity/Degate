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

#include "Core/LogicModel/Gate/GateLibraryExporter.h"
#include "Core/LogicModel/Gate/GateLibraryImporter.h"
#include "Core/LogicModel/Gate/GateLibrary.h"
#include "Core/LogicModel/Gate/GateTemplate.h"
#include "Core/LogicModel/Gate/GateTemplatePort.h"

#include <memory>

#include <catch.hpp>

using namespace degate;

TEST_CASE("Gate library importer", "[GateLibraryImporter]")
{
    GateLibraryImporter importer;

    std::string filename("tests_files/test_project/gate_library.xml");

    // If this fail, need to add the 'tests_files' folder beside the tests executable.
    REQUIRE(file_exists(filename) == true);

    GateLibrary_shptr glib(importer.import(filename));

    REQUIRE(glib != nullptr);

    // iterator test
    int i = 0;
    for(auto iter = glib->begin(); iter != glib->end(); ++iter, i++)
    {
        GateTemplate_shptr tmpl = (*iter).second;
        REQUIRE(tmpl != nullptr);

        // valid assumptions for the test data
        REQUIRE(tmpl->get_reference_counter() == 0);
        REQUIRE(tmpl->get_height() > 0);
        REQUIRE(tmpl->get_width() > 0);
        REQUIRE(tmpl->has_valid_object_id() == true);

        int j = 0;
        for(auto piter = tmpl->ports_begin(); piter != tmpl->ports_end(); ++piter, j++)
        {
            GateTemplatePort_shptr port = *piter;
            REQUIRE(port != nullptr);
            REQUIRE(port->has_valid_object_id() == true);
        }
        REQUIRE(j > 0);
    }

    REQUIRE(i > 0);
}
