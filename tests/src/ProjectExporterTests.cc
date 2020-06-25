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

#include <Core/Project/ProjectImporter.h>
#include <Core/Project/ProjectExporter.h>
#include <Core/Project/Project.h>

#include "catch.hpp"

using namespace degate;

TEST_CASE("Test project export", "[ProjectExporter]")
{
    /*
     * Import a project
     */
    ProjectImporter importer;

    std::string filename("tests_files/test_project/project.xml");
    Project_shptr prj(importer.import_all(filename));

    REQUIRE(prj != nullptr);

    /*
     * export project data
     */
    ProjectExporter exporter;
    REQUIRE_NOTHROW(exporter.export_all("tests_files/test_project", prj));
}