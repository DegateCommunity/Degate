/*
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#include "Importer.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include "FileSystem.h"

using namespace std;
using namespace degate;

ret_t Importer::check_file(std::string const& filename) const {
	
  if(is_file(filename)) {
    return RET_OK;
  }
  else {
    debug(TM, "Can't open file %s", filename.c_str());
    return RET_ERR;
  }
}


bool Importer::parse_bool(std::string const & str) const throw(XMLAttributeParseException) {
  if(str == "true" || str == "enabled") return true;
  else if(str == "false" || str == "disabled") return false;
  else {
    debug(TM, "invalid attribute value '%s'", str.c_str());
    throw std::invalid_argument("Can't parse string as a boolean value.");
  }
}

