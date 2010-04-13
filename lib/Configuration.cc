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

#include "Configuration.h"
#include "FileSystem.h"

using namespace degate;

std::string degate::get_temp_directory() { 
  Configuration * conf = Configuration::get_instance();
  assert(conf != NULL);
  return conf->get_temp_directory();
}

Configuration * Configuration::instance = NULL;

Configuration::Configuration() {
}


Configuration * Configuration::get_instance() {
  if(instance == NULL)
    instance = new Configuration();
  return instance;
}

std::string Configuration::get_temp_directory() const {
  return get_realpath(std::string("/tmp")); 
}
