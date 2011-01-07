/*

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

#include <FileSystem.h>
#include <Configuration.h>
#include <degate_exceptions.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <string>

using namespace degate;
using namespace boost::filesystem;

/** @todo Instead of writing own wrapper functions, it would be better to
    use the boost filesystem abstraction.
*/

bool degate::is_directory(std::string const & path) {
  struct stat stat_buf;
  if(stat(path.c_str(), &stat_buf) == 0) {
    return S_ISDIR(stat_buf.st_mode) ? true : false;
  }
  return false;
}

bool degate::is_file(std::string const & path) {
  struct stat stat_buf;
  if(stat(path.c_str(), &stat_buf) == 0) {
    return S_ISREG(stat_buf.st_mode) ? true : false;
  }
  return false;
}

bool degate::is_symlink(std::string const & path) {
  struct stat stat_buf;
  if(stat(path.c_str(), &stat_buf) == 0) {
    return S_ISLNK(stat_buf.st_mode) ? true : false;
  }
  return false;
}

bool degate::file_exists(std::string const & path) {
  struct stat stat_buf;
  return stat(path.c_str(), &stat_buf) == 0 ? true : false;
}

std::string degate::get_basedir(std::string const & path) throw(InvalidPathException) {

  std::string resolved_path;

  if(file_exists(path)) {
    resolved_path = get_realpath(path);

    if(is_directory(resolved_path)) return resolved_path;
    else {
      return resolved_path.substr(0, resolved_path.find_last_of('/'));
    }
  }
  else {
    // treat it as a file name
    size_t last_pos = path.find_last_of('/');
    if(last_pos != 0)
      return path.substr(0, last_pos);
    else return "/";
  }

}


std::string degate::get_realpath(std::string const& path) throw(degate::InvalidPathException) {
  char resolved_path[PATH_MAX];
  if(realpath(path.c_str(), resolved_path) == NULL) {
    boost::format fmter("Error in get_realpath(). Can't get real path for %1%.");
    fmter % path;
    throw degate::InvalidPathException(fmter.str());
  }
  else return std::string(resolved_path);
}


std::string degate::get_file_suffix(std::string const& path) {
  size_t last_occurance = path.rfind(".", path.size());
  if(last_occurance < path.size()) {
    return path.substr(last_occurance + 1, path.size() - last_occurance);
  }
  else return std::string();
}


void degate::remove_file(std::string const& path) throw(degate::FileSystemException) {
  if(unlink(path.c_str()) != 0) {
    throw degate::FileSystemException(strerror(errno));
  }
}

void degate::remove_directory(std::string const& path) throw(degate::FileSystemException) {
  boost::filesystem::path p(path);
  boost::filesystem::remove_all(path);
}

void degate::create_directory(std::string const& directory, mode_t mode)
  throw(degate::FileSystemException) {

  if(mkdir(directory.c_str(), mode) != 0) {
    throw degate::FileSystemException(strerror(errno));
  }

}

std::string degate::create_temp_directory() {
  return create_temp_directory(generate_temp_file_pattern(get_temp_directory()));
}

std::string degate::create_temp_directory(std::string const & directory_pattern) {
  char template_str[PATH_MAX];
  strncpy(template_str, directory_pattern.c_str(), sizeof(template_str));
  char * dirname = mkdtemp(template_str);
  return std::string(dirname);
}


std::string degate::generate_temp_file_pattern(std::string const & basedir) {
  return basedir + std::string("/temp.XXXXXXXXXXX");
}


 std::list<std::string> degate::read_directory(std::string const& path, bool prefix_path)
  throw(degate::FileSystemException) {

  DIR * dir = NULL;
  struct dirent * dir_ent = NULL;

  std::string rpth = get_realpath(path);

  if((dir = opendir(rpth.c_str())) == NULL)
    throw degate::FileSystemException(strerror(errno));

  std::list<std::string> retlist;

  while((dir_ent = readdir(dir)) != NULL)
    if(!(!strcmp(dir_ent->d_name, ".") ||
	 !strcmp(dir_ent->d_name, "..")))

	retlist.push_back(prefix_path ? join_pathes(path, dir_ent->d_name) : dir_ent->d_name);

  closedir(dir);

  return retlist;
}



std::string degate::join_pathes(std::string const& base_path, std::string const& extension_path) {
  return base_path + std::string("/") + extension_path;
}


std::string degate::get_filename_from_path(std::string const& path) {
  size_t last_occurance = path.rfind("/", path.size());
  if(last_occurance < path.size()) {
    return path.substr(last_occurance + 1, path.size() - last_occurance);
  }
  else return path;
}

std::string degate::get_basename(std::string const& path) {
  std::string filename(get_filename_from_path(path));

  size_t last_occurance = filename.rfind(".", filename.size());
  if(last_occurance < filename.size())
    return filename.substr(0, last_occurance);
  else return filename;
}


// a copy from qemu

char *realpath_alloc(const char *path)
{
  int buff_len;
  char *result;

#ifdef PATH_MAX
  buff_len = PATH_MAX;
#else
  buff_len = pathconf(path, _PC_PATH_MAX);
  if (buff_len <= 0)
    buff_len = 4096;
#endif

  ++buff_len;
  result = (char*)malloc(buff_len * sizeof(char));
  if (!result)
    return NULL;

  if(realpath(path, result) == NULL) {
    free(result);
    return NULL;
  }
  else return result;
}


char * _get_relative_path(const char * const path, const char * const relative_to) {
  char *path_real;
  char *path_prefix;
  char *path_real_suffix;
  char *path_real_to;
  char *path_real_to_suffix;
  char *result;
  int prefix_len, i, slash_count;
  char *string_end;
  char path_separator;
#ifdef _WIN32
  path_separator = '\\';
#else
  path_separator = '/';
#endif

  if (NULL == path || NULL == relative_to)
    return NULL;

  path_real = realpath_alloc(path);
  if (!path_real)
    return NULL;
  path_real_to = realpath_alloc(relative_to);
  if (!path_real_to)
    {
      free(path_real);
      return NULL;
    }

  if (0 == strcmp(path_real, path_real_to))
    {
      free(path_real);
      free(path_real_to);

      //the two directories are equal, the relative path is an empty string
      result = (char*)malloc(sizeof(char));
      *result = '\0';
      return result;
    }

  result = NULL;

  //eliminate the common prefix
  for (prefix_len = 0;
       path_real[prefix_len] != '\0' &&
	 path_real_to[prefix_len] != '\0' &&
	 path_real[prefix_len] == path_real_to[prefix_len];
       ++prefix_len);

  path_prefix = path_real;
  path_real_suffix = path_real + prefix_len;
  while ('\0' != *path_real_suffix &&
#ifdef _WIN32
	 ('/' == *path_real_suffix || '\\' == *path_real_suffix)
#else
	 ('/' == *path_real_suffix)
#endif
	 ) { *path_real_suffix++ = '\0'; }

  path_real_to_suffix = path_real_to + prefix_len;
  while ('\0' != *path_real_to_suffix &&
#ifdef _WIN32
	 ('/' == *path_real_to_suffix || '\\' == *path_real_to_suffix)
#else
	 ('/' == *path_real_to_suffix)
#endif
	 ) { *path_real_to_suffix++ = '\0'; }

  slash_count = 0;
  for (i = 0; '\0' != path_real_to_suffix[i]; ++i)
#ifdef _WIN32
    if ('/' == path_real_to_suffix[i] || '\\' == path_real_to_suffix[i])
#else
      if ('/' == path_real_to_suffix[i])
#endif
	++slash_count;
  if ('\0' != *path_real_to_suffix) ++slash_count;
  result = (char*)malloc(sizeof(char) * (4 + 3 * slash_count + strlen(path_real_suffix)));

  string_end = result;
  for (i = 0; i < slash_count; ++i)
    {
      if (i > 0)
	*string_end++ = path_separator;
      *string_end++ = '.';
      *string_end++ = '.';
    }
  if (0 == slash_count)
    *string_end++ = '.';
  if ('\0' != *path_real_suffix)
    {
      *string_end++ = path_separator;
      for (i = 0; '\0' != path_real_suffix[i]; ++i)
	*string_end++ = path_real_suffix[i];
    }
  *string_end++ = '\0';

  free(path_real);
  free(path_real_to);
  return result;
}

std::string degate::get_relative_path(std::string const& path,
				      std::string const& relative_to) {

  //boost::format fmter("\npath=%1%\nrelative_to=%2%");
  //fmter % path % relative_to;
  //std::cout << fmter.str() << std::endl;

  char * rel_path = _get_relative_path(path.c_str(), relative_to.c_str());

  std::string ret(rel_path);

  //std::cout << "rel_path=" << ret << std::endl;

  free(rel_path);
  return ret;
}



boost::filesystem::path degate::strip_path(boost::filesystem::path const& strip_from,
					   boost::filesystem::path const& strip_what) {

  path::iterator src_path_iter = strip_what.begin();
  path::iterator src_path_end = strip_what.end();
  path stripped;

  BOOST_FOREACH(std::string s, strip_from) {
    if(src_path_iter != src_path_end && *src_path_iter == s)
      ++src_path_iter;
    else
      stripped /= s;
  }

  return stripped;
}
