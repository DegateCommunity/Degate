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

#include "Core/Configuration.h"
#include "Core/Utils/FileSystem.h"

#include <boost/filesystem.hpp>
#include <climits>
#include <cstring>
#include <string>

using namespace degate;
using namespace boost::filesystem;

bool degate::is_directory(std::string const& path)
{
    try
    {
        return boost::filesystem::is_directory(path);
    }
    catch (filesystem_error const& e)
    {
        debug(TM, e.what());
        return false;
    }
}

bool degate::is_file(std::string const& path)
{
    try
    {
        return boost::filesystem::is_regular_file(path);
    }
    catch (filesystem_error const& e)
    {
        debug(TM, e.what());
        return false;
    }
}

bool degate::is_symlink(std::string const& path)
{
    try
    {
        return boost::filesystem::is_symlink(path);
    }
    catch (filesystem_error const& e)
    {
        debug(TM, e.what());
        return false;
    }
}

bool degate::file_exists(std::string const& path)
{
    try
    {
        return boost::filesystem::exists(path);
    }
    catch (filesystem_error const& e)
    {
        debug(TM, e.what());
        return false;
    }
}

std::string degate::get_basedir(std::string const& path)
{
    std::string resolved_path;

    if (file_exists(path))
    {
        resolved_path = get_realpath(path);

        if (is_directory(resolved_path))
            return resolved_path;
        else
        {
            boost::filesystem::path p(path);
            return p.parent_path().make_preferred().string();
        }
    }
    else
    {
        // treat it as a file name
        boost::filesystem::path p(path);
        return p.parent_path().make_preferred().string();
    }
}

std::string degate::get_realpath(std::string const& path)
{
    return boost::filesystem::canonical(path).make_preferred().string();
}

std::string degate::get_file_suffix(std::string const& path)
{
    size_t last_occurance = path.rfind(".", path.size());
    if (last_occurance < path.size())
    {
        return path.substr(last_occurance + 1, path.size() - last_occurance);
    }
    else
        return std::string();
}

bool degate::remove_file(std::string const& path)
{
    try
    {
        return boost::filesystem::remove(path);
    }
    catch (filesystem_error const& e)
    {
        debug(TM, e.what());
        return false;
    }
}

void degate::remove_directory(std::string const& path)
{
    try
    {
        boost::filesystem::remove_all(path);
    }
    catch (filesystem_error const& e)
    {
        debug(TM, e.what());
    }
}

void degate::clear_directory(std::string const& path,
                             std::vector<std::string> const& exclusion_list,
                             std::string const& base_directory_path)
{
    if (path.empty())
        return;

    // Set base directory
    std::string base_directory = base_directory_path;
    if (base_directory_path.empty())
        base_directory = path;

    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator it(path); it != end_itr; it++)
    {
        // If no exclusion list
        if (exclusion_list.empty())
        {
            if (boost::filesystem::is_directory(it->path()))
                remove_directory(it->path().string());
            else
                remove_file(it->path().string());
        }

        // Prepare element (stripped for relative)
        const auto& stripped_element = strip_path(it->path().string(), base_directory).string();
        const auto& element = it->path().string();

        // Check exclusion list
        bool skip = false;
        for (const auto& excluded_element : exclusion_list)
        {
            try
            {
                if (boost::filesystem::equivalent(boost::filesystem::path(element),
                                                  boost::filesystem::path(excluded_element)) ||
                    stripped_element == excluded_element)
                    skip = true;
            }
            catch (const std::exception&)
            {
            }
        }

        // If in exclusion list, skip the element clear
        if (skip)
            continue;

        // Recursive call
        if (boost::filesystem::is_directory(it->path()))
            clear_directory(it->path().string(), exclusion_list, base_directory);
        else
            remove_file(it->path().string());

        // Finally, remove the current directory
        remove_directory(it->path().string());
    }
}

void degate::move_file(std::string const& old_path, std::string const& new_path)
{
    boost::filesystem::rename(old_path, new_path);
}

void degate::create_directory(std::string const& directory)
{
    boost::filesystem::create_directory(directory);
}

std::string degate::create_temp_directory()
{
    boost::filesystem::path t(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path());
    create_directory(t.string());
    return t.string();
}

std::string degate::get_temp_file_path()
{
    return boost::filesystem::unique_path(generate_temp_file_pattern()).make_preferred().string();
}

std::string degate::get_temp_directory_path()
{
    return boost::filesystem::temp_directory_path().make_preferred().string();
}

std::string degate::generate_temp_file_pattern()
{
    boost::filesystem::path p(boost::filesystem::temp_directory_path() /
                              boost::filesystem::path("degate_temp.%%%%%%%%"));
    return p.make_preferred().string();
}

std::list<std::string> degate::read_directory(std::string const& path, bool prefix_path)
{
    boost::filesystem::path p(get_realpath(path));

    boost::filesystem::directory_iterator end_itr;

    std::list<std::string> retlist;

    for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
    {
        std::string file_name = itr->path().filename().string();
        if (file_name != "." && file_name != "..")
        {
            retlist.push_back(prefix_path ? join_pathes(path, file_name) : file_name);
        }
    }

    return retlist;
}

std::string degate::join_pathes(std::string const& base_path, std::string const& extension_path)
{
    boost::filesystem::path p(base_path / extension_path);
    return p.make_preferred().string();
}

std::string degate::get_filename_from_path(std::string const& path)
{
    boost::filesystem::path p(path);
    return p.filename().string();
}

std::string degate::get_basename(std::string const& path)
{
    std::string filename(get_filename_from_path(path));

    size_t last_occurance = filename.rfind(".", filename.size());
    if (last_occurance < filename.size())
        return filename.substr(0, last_occurance);
    else
        return filename;
}

boost::filesystem::path naive_uncomplete(boost::filesystem::path const p, boost::filesystem::path const base)
{
    using boost::filesystem::path;

    boost::filesystem::canonical(p);
    boost::filesystem::canonical(base);

    if (p == base)
        return "";
    /*!! this breaks stuff if path is a filename rather than a directory,
         which it most likely is... but then base shouldn't be a filename so... */

    boost::filesystem::path from_path, from_base, output;

    boost::filesystem::path::iterator path_it = p.begin(), path_end = p.end();
    boost::filesystem::path::iterator base_it = base.begin(), base_end = base.end();

    // check for emptiness
    if ((path_it == path_end) || (base_it == base_end))
        throw std::runtime_error("path or base was empty; couldn't generate relative path");

#ifdef WIN32
    // drive letters are different; don't generate a relative path
    if (*path_it != *base_it)
        return p;

    // now advance past drive letters; relative paths should only go up
    // to the root of the drive and not past it
    ++path_it, ++base_it;
#endif

    // Cache system-dependent dot, double-dot and slash strings
    const std::string dot = ".";
    const std::string dots = "..";
#ifdef WIN32
    const std::string sep = "/";
#else
    const std::string sep = "\\";
#endif

    // iterate over path and base
    while (true)
    {
        // compare all elements so far of path and base to find greatest common root;
        // when elements of path and base differ, or run out:
        if ((path_it == path_end) || (base_it == base_end) || (*path_it != *base_it))
        {
            // write to output, ../ times the number of remaining elements in base;
            // this is how far we've had to come down the tree from base to get to the common root
            for (; base_it != base_end; ++base_it)
            {
                if (*base_it == dot)
                    continue;
                else if (*base_it == sep)
                    continue;

                output /= "..";
            }

            // write to output, the remaining elements in path;
            // this is the path relative from the common root
            boost::filesystem::path::iterator path_it_start = path_it;
            for (; path_it != path_end; ++path_it)
            {
                if (*path_it == dot)
                    continue;
                if (*path_it == sep)
                    continue;

                output /= *path_it;
            }

            break;
        }

        // add directory level to both paths and continue iteration
        from_path /= path(*path_it);
        from_base /= path(*base_it);

        ++path_it, ++base_it;
    }

    return output;
}


std::string degate::get_relative_path(std::string const& path, std::string const& relative_to)
{
    return naive_uncomplete(path, relative_to).make_preferred().string();
}


boost::filesystem::path degate::strip_path(boost::filesystem::path const& strip_from,
                                           boost::filesystem::path const& strip_what)
{
    path::iterator src_path_iter = strip_what.begin();
    path::iterator src_path_end = strip_what.end();
    path stripped;

    for (auto s : strip_from)
    {
        if (src_path_iter != src_path_end && *src_path_iter == s)
            ++src_path_iter;
        else
            stripped /= s;
    }

    return stripped;
}
