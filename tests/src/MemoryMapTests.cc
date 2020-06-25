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

#include <Core/Utils/MemoryMap.h>

#include "catch.hpp"

using namespace degate;

TEST_CASE("Memory", "[MemoryMap]")
{
    MemoryMap<int> mm(100, 100);

    REQUIRE(mm.get_width() == 100);
    REQUIRE(mm.get_height() == 100);

    mm.set(10, 10, 99);
    REQUIRE(mm.get(10, 10) == 99);
}

TEST_CASE("Temporary MemoryMap", "[MemoryMap]")
{
    std::string temp_filename;

    {
        MemoryMap<int> mm(100, 100, MAP_STORAGE_TYPE_TEMP_FILE, "");
        temp_filename = mm.get_filename();
        REQUIRE(file_exists(temp_filename) == true);

        REQUIRE(mm.get_width() == 100);
        REQUIRE(mm.get_height() == 100);

        mm.set(10, 10, 99);
        REQUIRE(mm.get(10, 10) == 99);
    }

    SECTION("Temporary file has been deleted")
    {
        REQUIRE(file_exists(temp_filename) == false);
    }
}

TEST_CASE("Persistent MemoryMap", "[MemoryMap]")
{
    std::string filename(get_temp_file_path());

    REQUIRE(file_exists(filename) == false);

    {
        MemoryMap<int> mm(100, 100, MAP_STORAGE_TYPE_PERSISTENT_FILE, filename);
        REQUIRE(file_exists(filename) == true);

        REQUIRE(mm.get_filename() == filename);

        REQUIRE(mm.get_width() == 100);
        REQUIRE(mm.get_height() == 100);

        mm.set(10, 10, 99);
        REQUIRE(mm.get(10, 10) == 99);
    }

    REQUIRE(file_exists(filename) == true);
    REQUIRE(remove_file(filename) == true);
}