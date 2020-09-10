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

#include "Core/Utils/FileSystem.h"
#include <fstream>

#include "catch.hpp"

using namespace degate;

TEST_CASE("Is directory", "[FileSystem]")
{
    REQUIRE(is_directory("/reallynonexistentdirectory/") == false);
    REQUIRE(is_directory(get_temp_directory_path()) == true);

    std::string filename = get_temp_file_path();
    std::fstream temp_file;
    temp_file.open(filename, std::fstream::out | std::fstream::app);
    temp_file << "dump" << std::endl;

    REQUIRE(file_exists(filename) == true);

    REQUIRE(is_directory(filename) == false);

    temp_file.close();

    std::remove(filename.c_str());
}

TEST_CASE("Is file", "[FileSystem]")
{
    REQUIRE(is_file(get_temp_directory_path()) == false);

    std::string filename = get_temp_file_path();
    std::fstream temp_file;
    temp_file.open(filename, std::fstream::out | std::fstream::app);
    temp_file << "dump" << std::endl;

    REQUIRE(file_exists(filename) == true);

    REQUIRE(is_file(filename) == true);

    temp_file.close();

    std::remove(filename.c_str());
}

TEST_CASE("Get temporary file path", "[FileSystem]")
{
    std::string filename = get_temp_file_path();

    REQUIRE(file_exists(filename) == false);

    std::fstream temp_file;
    temp_file.open(filename, std::fstream::out | std::fstream::app);
    temp_file << "dump" << std::endl;

    REQUIRE(file_exists(filename) == true);

    REQUIRE(is_directory(filename) == false);
    REQUIRE(is_file(filename) == true);

    temp_file.close();

    std::remove(filename.c_str());

    REQUIRE(file_exists(filename) == false);
}

TEST_CASE("File exists", "[FileSystem]")
{
    std::string filename = get_temp_file_path();

    REQUIRE(file_exists(filename) == false);

    std::fstream temp_file;
    temp_file.open(filename, std::fstream::out | std::fstream::app);
    temp_file << "dump" << std::endl;

    REQUIRE(file_exists(filename) == true);

    temp_file.close();

    std::remove(filename.c_str());

    REQUIRE(file_exists(filename) == false);
}

TEST_CASE("Get suffix", "[FileSystem]")
{
    std::string suffix_dat(get_file_suffix("test.dat"));

    REQUIRE(suffix_dat.compare("dat") == 0);

    std::string suffix_none(get_file_suffix("testdat"));
    REQUIRE(suffix_none.size() == 0);

    std::string suffix_not_really(get_file_suffix("testdat."));
    REQUIRE(suffix_not_really.size() == 0);

    std::string suffix_not_really2(get_file_suffix("testdat......."));
    REQUIRE(suffix_not_really2.size() == 0);
}

TEST_CASE("Read directory", "[FileSystem]")
{
    std::string filename = get_temp_file_path();

    REQUIRE(file_exists(filename) == false);

    std::fstream temp_file;
    temp_file.open(filename, std::fstream::out | std::fstream::app);
    temp_file << "dump" << std::endl;

    std::string filename2 = get_temp_directory_path();

    std::list<std::string> files = read_directory(get_temp_directory_path());
    REQUIRE(files.empty() == false);

    REQUIRE_THROWS(read_directory("/reallynonexistentdirectory"));

    temp_file.close();

    std::remove(filename.c_str());

    REQUIRE(file_exists(filename) == false);
}

TEST_CASE("Create and remove temporary directory", "[FileSystem]")
{
    std::string dir = create_temp_directory();

    REQUIRE(file_exists(dir) == true);
    remove_directory(dir);
    REQUIRE(file_exists(dir) == false);
}

TEST_CASE("Create and remove directory", "[FileSystem]")
{
    std::string my_dir = join_pathes(get_temp_directory_path(), "test-dir-for-degate-unit-tests");

    REQUIRE(file_exists(my_dir) == false);

    create_directory(my_dir);

    REQUIRE(file_exists(my_dir) == true);

    remove_directory(my_dir);

    REQUIRE(file_exists(my_dir) == false);
}

TEST_CASE("Get filename", "[FileSystem]")
{
    REQUIRE(get_filename_from_path("") == "");
    REQUIRE(get_filename_from_path("/etc/hosts") == "hosts");
    REQUIRE(get_filename_from_path("/etc/resolv.conf") == "resolv.conf");
}

TEST_CASE("Get basename", "[FileSystem]")
{
    REQUIRE(get_basename("") == "");
    REQUIRE(get_basename("/etc/hosts") == "hosts");
    REQUIRE(get_basename("/etc/resolv.conf") == "resolv");
    REQUIRE(get_basename("/etc/resolv.") == "resolv");
    REQUIRE(get_basename("/etc/.conf") == "");
    REQUIRE(get_basename("/etc/resolv...conf") == "resolv..");

    REQUIRE(get_basename("resolv.conf") == "resolv");
    REQUIRE(get_basename(".conf") == "");
}