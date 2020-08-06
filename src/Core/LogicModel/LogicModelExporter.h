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

#include "Globals.h"
#include "LogicModel.h"
#include "Core/XML/XMLExporter.h"
#include "Core/Utils/ObjectIDRewriter.h"
#include "Layer.h"

#include <stdexcept>

namespace degate
{
	/**
	 * The LogicModelExporter exports a logic model. That is the file lmodel.xml from your degate project.
	 *
	 */
	class LogicModelExporter : public XMLExporter
	{
	private:
		typedef std::map<object_id_t /* net id */, QDomElement&> net_element_map_type;

		void add_gate(QDomDocument& doc, QDomElement& gates_elem, Gate_shptr gate, layer_position_t layer_pos);
		void add_wire(QDomDocument& doc, QDomElement& wires_elem, Wire_shptr wire, layer_position_t layer_pos);
		void add_via(QDomDocument& doc, QDomElement& vias_elem, Via_shptr via, layer_position_t layer_pos);

		void add_emarker(QDomDocument& doc, QDomElement& emarkers_elem, EMarker_shptr emarker,
		                 layer_position_t layer_pos);

		void add_nets(QDomDocument& doc, QDomElement& nets_elem, LogicModel_shptr lmodel);

		void add_annotation(QDomDocument& doc, QDomElement& annotations_elem, Annotation_shptr annotation,
		                    layer_position_t layer_pos);

		void add_module(QDomDocument& doc, QDomElement& modules_elem, LogicModel_shptr lmodel, Module_shptr module);

		ObjectIDRewriter_shptr oid_rewriter;

	public:
		LogicModelExporter(ObjectIDRewriter_shptr oid_rewriter) : oid_rewriter(oid_rewriter)
		{
		}

		~LogicModelExporter()
		{
		}

		void export_data(std::string const& filename, LogicModel_shptr lmodel);
	};
}

#endif
