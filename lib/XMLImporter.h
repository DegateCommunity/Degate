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

#ifndef __XMLIMPORTER_H__
#define __XMLIMPORTER_H__

#include "globals.h"
#include "Importer.h"

#include <libxml++/libxml++.h>

namespace degate {

class XMLImporter : public Importer {

 protected:

  using Importer::parse_number;

  /**
   * Parse a string and convert it to a number (e.g. double, long, unsigned int, ...).
   * @exception InvalidPointerException is thrown, if you node is a NULL pointer.
   * @exception XMLAttributeMissingException The XML attribute is not present.
   * @return Returns the number in type T.
   */
  template <typename T> 
    T parse_number(const xmlpp::Element * const node, std::string const& attribute_str) const {

    assert(node != NULL);
    if(node == NULL) throw InvalidPointerException("Parameter must be != NULL.");

    if(node->get_attribute(attribute_str) == NULL) {
      throw XMLAttributeMissingException(std::string("attribute is not present: ") + attribute_str);
    }
    else return parse_number<T>(node->get_attribute_value(attribute_str));
  }

  /**
   * Parse a string and convert it to a number (e.g. double, long, unsigned int, ...).
   * @exception InvalidPointerException is thrown, if you node is a NULL pointer.
   * @return Returns the number in type T. If the XML attribute is not present, the default value is returned.
   */

  template <typename T> 
    T parse_number(const xmlpp::Element * const node, std::string const& attribute_str, T default_value) {

    assert(node != NULL);
    if(node == NULL) throw InvalidPointerException();

    if(node->get_attribute(attribute_str) == NULL) return default_value;
    else return parse_number<T>(node->get_attribute_value(attribute_str));
  }

  const xmlpp::Element * get_dom_twig(const xmlpp::Element * const start_node, std::string const & element_name) const;

  /**
   * Parse a HTML RGBA color description, e.g. '#23FF42A0'.
   * @return Returns the internally used color code. If the string can't be parsed a value of 0 (black) is returned.
   */
  color_t parse_color_string(std::string const& color_string) const;


public:
	XMLImporter() {};
	virtual ~XMLImporter() {};


};

}

#endif
