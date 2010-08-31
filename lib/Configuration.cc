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

#include <Configuration.h>
#include <FileSystem.h>

#include <boost/lexical_cast.hpp>

using namespace degate;

std::string degate::get_temp_directory() { 
  Configuration const & conf = Configuration::get_instance();
  return conf.get_temp_directory();
}

Configuration::Configuration() {
}

std::string Configuration::get_temp_directory() const {
  char * td = getenv("DEGATE_TEMP_DIR");
  if(td == NULL) return get_realpath(std::string("/tmp"));
  return get_realpath(std::string(td)); 
}

size_t Configuration::get_max_tile_cache_size() const {
  char * cs = getenv("DEGATE_CACHE_SIZE");
  if(cs == NULL) return 256;
  return boost::lexical_cast<size_t>(cs);
}

std::string Configuration::get_servers_uri_pattern() const {
  char * uri_pattern = getenv("DEGATE_SERVER_URI_PATTERN");
  if(uri_pattern == NULL) return "http://localhost/cgi-bin/test.pl?channel=%1%";
  return uri_pattern;
}
