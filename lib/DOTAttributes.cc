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

#include "DOTAttributes.h"

using namespace degate;

void DOTAttributes::add(std::string const& attribute_name,
			std::string const& value) {
  std::ostringstream stm;
  stm << attribute_name << "=\"" << value << "\"";
  attributes[attribute_name] = stm.str();
}



void DOTAttributes::add_position(long center_x, long center_y,
				 bool preserve_position) {
  std::ostringstream stm;
  stm << "pos" << "=\"" << center_x << "," << center_y;
  if(preserve_position) stm << "!";
  stm << "\"";
  attributes["pos"] = stm.str();
}


std::string DOTAttributes::get_string() const {
  std::string result;

  for(std::map<std::string, std::string>::const_iterator iter = attributes.begin();
      iter != attributes.end(); ++iter) {

    if(result.size() > 0) result += ",";
    result += (*iter).second;
  }

  return std::string("[") + result + std::string("];");
}

