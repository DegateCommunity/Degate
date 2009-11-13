/* -*-c++-*-
 
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

#ifndef __XMLEXPORTER_H__
#define __XMLEXPORTER_H__

#include "globals.h"
#include "Exporter.h"
#include <libxml++/libxml++.h>

namespace degate {

  /**
   * A base class for XML exporter.
   */
  class XMLExporter : public Exporter {

  public:
    /**
     * The ctor.
     */
    XMLExporter() {};

    /**
     * The dtor.
     */
    virtual ~XMLExporter() {};
  };

}

#endif
