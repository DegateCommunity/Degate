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

#include "Core/LogicModel/Gate/GateLibraryImporter.h"
#include "Core/LogicModel/LogicModelImporter.h"
#include "Core/Utils/ObjectIDRewriter.h"
#include "Core/DOT/LogicModelDOTExporter.h"

#include <memory>

#include "catch.hpp"

using namespace degate;

TEST_CASE("Test export", "[LogicModelDOTExporter]")
{
    /*
     * parse a library
     */
    GateLibraryImporter gate_library_importer;
    std::string gl_filename("tests_files/test_project/gate_library.xml");
    GateLibrary_shptr glib(gate_library_importer.import(gl_filename));

    // If this fail, need to add the 'tests_files' folder beside the tests executable.
    REQUIRE(glib != nullptr);

    /*
     * parse logic model
     */
    LogicModelImporter lm_importer(500, 500, glib);
    std::string filename("tests_files/test_project/lmodel.xml");
    LogicModel_shptr lmodel(lm_importer.import(filename, ProjectType::Normal));

    // If this fail, need to add the 'tests_files' folder beside the tests executable.
    REQUIRE(lmodel != nullptr);


    /*
     * export logic model as dot files
     */
    LogicModelDOTExporter exporter(std::make_shared<ObjectIDRewriter>(true));

    std::string out_filename("tests_files/layout_gate_placement.dot");

    if (file_exists(out_filename)) remove_file(out_filename);

    exporter.set_property(LogicModelDOTExporter::PRESERVE_GATE_POSITIONS, true);
    exporter.set_property(LogicModelDOTExporter::ENABLE_EDGES, false);
    exporter.set_property(LogicModelDOTExporter::ENABLE_VIAS, false);
    exporter.set_property(LogicModelDOTExporter::ENABLE_WIRES, false);
    exporter.export_data(out_filename, lmodel);

    REQUIRE(file_exists(out_filename) == true);

    LogicModelDOTExporter exporter2(std::make_shared<ObjectIDRewriter>(true));

    std::string out_filename2("tests_files/logic_model_graph.dot");

    if (file_exists(out_filename2)) remove_file(out_filename2);

    exporter2.set_property(LogicModelDOTExporter::PRESERVE_GATE_POSITIONS, false);
    exporter2.set_property(LogicModelDOTExporter::ENABLE_EDGES, true);
    exporter2.set_property(LogicModelDOTExporter::ENABLE_VIAS, true);
    exporter2.set_property(LogicModelDOTExporter::ENABLE_WIRES, true);
    exporter2.set_property(LogicModelDOTExporter::ENABLE_TEMPLATE_NAMES, false);
    exporter2.export_data(out_filename2, lmodel);

    REQUIRE(file_exists(out_filename2) == true);
}