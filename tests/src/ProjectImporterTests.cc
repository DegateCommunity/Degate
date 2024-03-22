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

#include "Core/Project/ProjectImporter.h"
#include "Core/Project/ProjectExporter.h"
#include "Core/Project/Project.h"
#include "Core/LogicModel/LogicModelHelper.h"

#include <catch.hpp>

using namespace degate;

TEST_CASE("Test project import", "[ProjectImporter]")
{
    ProjectImporter importer;

    std::string filename("tests_files/test_project/project.xml");
    Project_shptr prj(importer.import(filename));

    REQUIRE(prj != nullptr);

    LogicModel_shptr lmodel = prj->get_logic_model();
    REQUIRE(lmodel != nullptr);

    Layer_shptr layer = get_first_logic_layer(lmodel);
    REQUIRE(layer != nullptr);
}

TEST_CASE("Test project import all", "[ProjectImporter]")
{
    ProjectImporter importer;

    std::string filename("tests_files/test_project/project.xml");
    Project_shptr prj(importer.import_all(filename));

    REQUIRE(prj != nullptr);

    std::cout << "back in test." << std::endl;

    LogicModel_shptr lmodel = prj->get_logic_model();
    REQUIRE(lmodel != nullptr);

    prj->print_all(std::cout);

    REQUIRE(lmodel->get_num_layers() == 2);

    Layer_shptr layer = get_first_logic_layer(lmodel);
    REQUIRE(layer != nullptr);
}

TEST_CASE("Test project import all new format", "[ProjectImporter]")
{
    ProjectImporter importer;

    std::string filename("tests_files/test_project/project.xml");
    Project_shptr prj(importer.import_all(filename));

    LogicModel_shptr lmodel = prj->get_logic_model();
    REQUIRE(lmodel != nullptr);

    Layer_shptr layer = get_first_logic_layer(lmodel);
    REQUIRE(layer != nullptr);

    GateLibrary_shptr gate_lib = lmodel->get_gate_library();
    REQUIRE(gate_lib != nullptr);

    for(auto iter = gate_lib->begin(); iter != gate_lib->end(); ++iter)
    {
        GateTemplate_shptr gate_tmpl((*iter).second);
        REQUIRE(gate_tmpl != nullptr);

        int i = 0;
        for (auto img_iter = gate_tmpl->images_begin(); img_iter != gate_tmpl->images_end(); ++img_iter, i++)
        {
            GateTemplateImage_shptr img = (*img_iter).second;
            REQUIRE(img != nullptr);
        }
        REQUIRE(i > 0);
    }
}

TEST_CASE("Test get object at", "[ProjectImporter]")
{
    ProjectImporter importer;

    std::string filename("tests_files/test_project/project.xml");
    Project_shptr prj(importer.import_all(filename));

    LogicModel_shptr lmodel = prj->get_logic_model();
    Layer_shptr layer = get_first_logic_layer(lmodel);
    PlacedLogicModelObject_shptr plo = layer->get_object_at_position(38, 38);

    REQUIRE(plo != nullptr);
    REQUIRE(plo->get_name() == "test_gate_1");
}
