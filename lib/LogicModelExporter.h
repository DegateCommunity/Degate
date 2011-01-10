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

#ifndef __LOGICMODELEXPORTER_H__
#define __LOGICMODELEXPORTER_H__

#include "globals.h"
#include "LogicModel.h"
#include "XMLExporter.h"
#include "ObjectIDRewriter.h"
#include "Layer.h"

#include <stdexcept>

namespace degate {

/**
 * The LogicModelExporter exports a logic model. That is the file lmodel.xml from your degate project.
 *
 */

class LogicModelExporter : public XMLExporter {

private:
  typedef std::map<object_id_t /* net id */, xmlpp::Element *> net_element_map_type;

  void add_gate(xmlpp::Element* gates_elem, Gate_shptr gate, layer_position_t layer_pos) throw(std::runtime_error );
  void add_wire(xmlpp::Element* wires_elem, Wire_shptr wire, layer_position_t layer_pos) throw(std::runtime_error );
  void add_via(xmlpp::Element* vias_elem, Via_shptr via, layer_position_t layer_pos) throw(std::runtime_error );

  void add_emarker(xmlpp::Element* emarkers_elem, EMarker_shptr emarker, layer_position_t layer_pos);

  void add_nets(xmlpp::Element* nets_elem, LogicModel_shptr lmodel) throw(std::runtime_error);

  void add_annotation(xmlpp::Element* annotations_elem, Annotation_shptr annotation, layer_position_t layer_pos)
    throw(std::runtime_error );

  ObjectIDRewriter_shptr oid_rewriter;

public:
  LogicModelExporter(ObjectIDRewriter_shptr _oid_rewriter) : oid_rewriter(_oid_rewriter) {}
  ~LogicModelExporter() {}

  void export_data(std::string const& filename, LogicModel_shptr lmodel)
    throw( InvalidPathException, InvalidPointerException, std::runtime_error );

};

}

#endif
