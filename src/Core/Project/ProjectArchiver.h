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

#ifndef __PROJECTARCHIVER_H__
#define __PROJECTARCHIVER_H__

#include "Core/Project/Project.h"

#include <stdexcept>

namespace degate
{
	/**
	 * Export a project directory as a ZIP archive.
	 *
	 */

	/* Dropped archiver/zip support for the moment.

	class ProjectArchiver
	{
	private:

		void add_single_file(struct zip* zip_archive,
		                     boost::filesystem::path const& archive_file,
		                     boost::filesystem::path const& base_dir_path,
		                     boost::filesystem::path const& file,
		                     boost::filesystem::path const& prepend_dir) const;


		void add_directory(struct zip* zip_archive,
		                   boost::filesystem::path const& archive_file,
		                   boost::filesystem::path const& base_dir_path,
		                   boost::filesystem::path const& dir,
		                   boost::filesystem::path const& prepend_dir) const;

	public:
		ProjectArchiver()
		{
		}

		~ProjectArchiver()
		{
		}

		void export_data(boost::filesystem::path const& project_dir,
		                 boost::filesystem::path const& archive_file,
		                 boost::filesystem::path const& prepend_dir) const;
	};
	*/
}

#endif
