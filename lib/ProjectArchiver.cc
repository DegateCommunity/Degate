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

#include <zip.h>

#include <globals.h>
#include <ProjectArchiver.h>
#include <list>
#include <tr1/memory>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>


using namespace std;
using namespace degate;
using namespace boost::filesystem;

void ProjectArchiver::add_single_file(struct zip * zip_archive,
				      path const& archive_file,
				      path const& base_dir_path,
				      path const& file,
				      path const& prepend_dir) const {

  assert(zip_archive != NULL);
  struct zip_source *source;

  path stripped = prepend_dir / strip_path(file, base_dir_path);

  debug(TM, "Add file %s as %s to zip archive.",
	file.string().c_str(), stripped.string().c_str());

  if((source = zip_source_file(zip_archive,
                               file.string().c_str(), 0, 0)) == NULL) {
    boost::format f("Cannot add file %1% to zip archive %2%: %3%");
    f % file % archive_file % zip_strerror(zip_archive);
    throw ZipException(f.str());
  }

  if(zip_add(zip_archive,
             stripped.string().c_str(),
             source) < 0) {

    boost::format f("Cannot add file %1% to zip archive %2%: %3%");
    f % file % archive_file % zip_strerror(zip_archive);
    throw ZipException(f.str());
  }
}


void ProjectArchiver::add_directory(struct zip * zip_archive,
				    path const& archive_file,
				    path const& base_dir_path,
				    path const& dir,
				    path const& prepend_dir) const {

  directory_iterator end_iter;

  for(directory_iterator iter(dir); iter != end_iter; ++iter) {

    path stripped = prepend_dir / strip_path(iter->path(), base_dir_path);

    if(is_directory(iter->path())) {

      std::string rel_dir = get_filename_from_path(stripped.native_file_string());
      std::string pattern = "scaling_";
      bool skip = rel_dir.length() >= pattern.length() && (rel_dir.compare(0, pattern.length(), pattern) == 0);

      if(!skip) {

	if(zip_add_dir(zip_archive, stripped.string().c_str()) < 0) {
	  boost::format f("Cannot add directory %1% to zip archive %2%: %3%");
	  f % iter->path() % archive_file % zip_strerror(zip_archive);
	  throw ZipException(f.str());
	}

	add_directory(zip_archive, archive_file, base_dir_path, 
		      iter->path(), // already prefixed with dir
		      prepend_dir);
      }

    }
    else {
      add_single_file(zip_archive, archive_file, base_dir_path, iter->path(), prepend_dir);
    }
  }

}


void ProjectArchiver::export_data(path const& project_dir,
				  path const& archive_file,
				  path const& prepend_dir) const {

  struct zip *zip_archive;
  int err;

  if((zip_archive = zip_open(archive_file.string().c_str(), ZIP_CREATE, &err)) == NULL) {
    char errstr[1024];
    zip_error_to_str(errstr, sizeof(errstr), err, errno);
    throw ZipException(errstr);
  }

  if(zip_add_dir(zip_archive, prepend_dir.string().c_str()) < 0) {
    boost::format f("Cannot add directory %1% to zip archive %2%: %3%");
    f % prepend_dir % archive_file % zip_strerror(zip_archive);
    throw ZipException(f.str());
  }

  try {
    add_directory(zip_archive, archive_file, project_dir, project_dir, prepend_dir);
  }
  catch(ZipException const& ex) {
    // rethrow exception, but free zip_archive resource first
    if(zip_archive != NULL) zip_close(zip_archive);
    throw;
  }

  if(zip_archive != NULL && zip_close(zip_archive) < 0) {
    boost::format f("Cannot write zip archive %1%: %2%");
    f % archive_file % zip_strerror(zip_archive);
    throw ZipException(f.str());
  }

}
