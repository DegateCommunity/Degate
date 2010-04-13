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

#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "globals.h"

#include <stdexcept>
#include <sstream>

namespace degate {

  /**
   * The base class for importers that can parse text files.
   */

  class Importer {

  protected:
    
    /**
     * Check if the file exists and can be read.
     * @return Returns RET_OK if the file can be read.
     * @todo Change the return type tool bool.
     */
    virtual ret_t check_file(std::string const& filename) const;
	
    /**
     * Parse a string that represents a boolean value, that is "true" or "false".
     * @return Returns a C++ bool, depending on the parsed string.
     * @throw std::invalid_argument The exception is thrown, if the string can not be parsed.
     */
    virtual bool parse_bool(std::string const& str) const throw(XMLAttributeParseException);

    /**
     * Parse a string that represents a number.
     * @throw XMLAttributeParseException This exception is thrown, if the string can't be parsed.
     */
    template <typename T> T parse_number(std::string const& str) const throw(XMLAttributeParseException) {
      std::stringstream strm(str);
      T v;
      strm >> v;
      if(!strm) throw XMLAttributeParseException();
      else return v;
    }
    
  public:

    /**
     * Create a new text importer object.
     */
    Importer() {};

    /**
     * Destroy a text importer object.
     */
    virtual ~Importer() {};
  };

}

#endif
