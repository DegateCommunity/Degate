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

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include "globals.h"
#include "degate_exceptions.h"

#include <stdexcept>
#include <sstream>
#include <list>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/filesystem/path.hpp>

#define BOOST_FILESYSTEM_VERSION 2

namespace degate{

  /**
   * Check if a path is a directory.
   * @return Returns true, if the path is a dirctory.
   * It returns false, if not or if the path doesn't exists.
   */

  bool is_directory(std::string const & path);

  /**
   * Check if a path is a regular file.
   * @return Returns true, if the path is a regular file.
   *   It returns false, if not or if the path doesn't exist.
   */

  bool is_file(std::string const & path);

  /**
   * Check if a path is a symlink.
   * @return Returns true, if the path is a symlink.
   *   It returns false, if not or if the path doesn't exist.
   */

  bool is_symlink(std::string const & path);

  /**
   * Check if a file or directory exists.
   * @returns Returns true, if the file or directory exist.
   */

  bool file_exists(std::string const & path);


  /**
   * Get the base directory for file or directory.
   * It is no problem if you request the basedir for a symlink
   * on a directory, because the path is internally expanded
   * via get_realpath(), but only if the file exist.
   * If you use get_basedir() on a non existing path, the path
   * is treated as a path to a file. Then the file name part is
   * stripped.
   * @return Returns a string with the base directory.
   * @exception Throws an InvalidPathException if the path does not exists.
   * @see get_realpath()
   */

  std::string get_basedir(std::string const & path) throw(degate::InvalidPathException);


  /**
   * Get the canonicalized absolute pathname.
   * @return Returns the resolved path as an absolut path name.
   * @exception Throws an InvalidPathException if the path does not exists.
   */

  std::string get_realpath(std::string const& path) throw(degate::InvalidPathException);


  /**
   * Get a file suffix without the dot, e.g. "xml" from a file named "foobar.xml".
   * @return Returns the file suffix as a string. If the file has no suffix, an
   * empty string is returned. The suffix is returned as it is. There is no
   * lowercase conversion or somthing like that.
   */

  std::string get_file_suffix(std::string const& path);


  /**
   * Get filename part of a path.
   */
  std::string get_filename_from_path(std::string const& path);


  /**
   * Get the basename of a file. That is the substring without the directory
   * part, the suffix and the dot.
   * @test FileSystemTest::test_get_basename()
   */
  std::string get_basename(std::string const& path);

  /**
   * Unlink a file.
   */
  void remove_file(std::string const& filename) throw(degate::FileSystemException);

  /**
   * Unlink a directory with all files in it.
   * Because this function is only for degate. We make some sanity checks.
   */
  void remove_directory(std::string const& path) throw(degate::FileSystemException);


  /**
   * Create a directory.
   */

  void create_directory(std::string const& directory, mode_t mode = 0700)
    throw(degate::FileSystemException);

  /**
   * Create a temp directory.
   */
  std::string create_temp_directory();

  /**
   * Create a temporary directory based on a path pattern specification.
   * The directory is created with mode 0700.
   * @param directory_pattern A pattern of the form e.g. "/tmp/temp.XXXXXXXXX".
   * @return Returns the path of the created directory.
   */
  std::string create_temp_directory(std::string const & directory_pattern);


  /**
   * Generate a pattern within a basedir for temp files.
   * The pattern is in the form of e.g. "/tmp/temp.XXXXXXXX" that can be passed
   * to mkstemp(). Only the pattern is generated.
   */

  std::string generate_temp_file_pattern(std::string const & basedir);

  /**
   * Read all entries from a directory, but not from subdirectories.
   * The path is expanded first with get_realpath().
   * @param path The directory you want to read.
   * @param prefix_path Control whether you will get file names relative to \p path
   *    or absolute file names.
   * @return Returns a list of strings containing files and subdirectory names.
   *    The special directory entires ".." and "." are not in the list.
   * @see get_realpath()
   */

  std::list<std::string> read_directory(std::string const& path, bool prefix_path = false)
    throw(degate::FileSystemException);


  /**
   * Join path specifications.
   * This is a helper function to create path names. E.g. if you call this
   * function with parameter "/etc" and "hosts", you will get a path
   * "/etc/hosts". You don't have to deal with path seperator in the first
   * or second parameter.
   */

  std::string join_pathes(std::string const& base_path, std::string const& extension_path);


  /**
   * Make a path specification relative to another.
   *
   */

  std::string get_relative_path(std::string const& path,
				std::string const& relative_to);




  /**
   * Strip the leading directory part from a path.
   * @param strip_from Is the path.
   * @param strip_what Is the base directory that should be removed.
   * @return Returnes a new path with the base directory stripped.
   */
  boost::filesystem::path strip_path(boost::filesystem::path const& strip_from,
				     boost::filesystem::path const& strip_what);

}

#endif
