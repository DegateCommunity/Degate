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

#include "Core/LogicModel/Gate/GateLibraryExporter.h"
#include "Core/LogicModel/Gate/GateLibraryImporter.h"
#include "Core/LogicModel/Gate/GateLibrary.h"
#include "Core/LogicModel/Gate/GateTemplate.h"
#include "Core/LogicModel/Gate/GateTemplatePort.h"

#include <memory>

#include "catch.hpp"

using namespace degate;

TEST_CASE("Gate library exporter", "[GateLibraryExporter]")
{
    /*
     * parse a library
     */
    GateLibraryImporter importer;

    std::string filename("tests_files/test_project/gate_library.xml");

    // If this fail, need to add the 'tests_files' folder beside the tests executable.
    REQUIRE(file_exists(filename) == true);

    GateLibrary_shptr glib(importer.import(filename));
    REQUIRE(glib != nullptr);

    /*
     * export library
     */
    GateLibraryExporter exporter(std::make_shared<ObjectIDRewriter>());


    exporter.export_data(filename, glib);

    REQUIRE(file_exists(filename) == true);

    /*
     * re-parse library
     */
    GateLibraryImporter reimporter;
    GateLibrary_shptr glib2(reimporter.import(filename));
}