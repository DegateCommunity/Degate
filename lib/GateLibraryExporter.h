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

#ifndef __GATELIBRARYEXPORTER_H__
#define __GATELIBRARYEXPORTER_H__

#include "globals.h"
#include "GateLibrary.h"
#include "XMLExporter.h"
#include "ObjectIDRewriter.h"

#include <stdexcept>

namespace degate {

/**
 * The GateLibraryExporter exports a gate library. That is the file
 * gate_library.xml from your degate project.
 */

class GateLibraryExporter : public XMLExporter {

private:

  void add_gates(xmlpp::Element* templates_elem, GateLibrary_shptr gate_lib,
		 std::string const& directory) throw(std::runtime_error );

  void add_images(xmlpp::Element* gate_elem, GateTemplate_shptr gate_tmpl,
		  std::string const& directory)
    throw(std::runtime_error );

  void add_implementations(xmlpp::Element* gate_elem, GateTemplate_shptr gate_tmpl,
			   std::string const& directory);

  void add_ports(xmlpp::Element* gate_elem, GateTemplate_shptr gate_tmpl);

  ObjectIDRewriter_shptr oid_rewriter;

public:
  GateLibraryExporter(ObjectIDRewriter_shptr _oid_rewriter) : oid_rewriter(_oid_rewriter) {}
  ~GateLibraryExporter() {}

  void export_data(std::string const& filename, GateLibrary_shptr gate_lib)
    throw( InvalidPathException, InvalidPointerException, std::runtime_error );

};

}

#endif
