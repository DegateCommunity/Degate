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

#include "LogicModelExporter.h"

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h> : Linux only
#include <cerrno>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <memory>

using namespace std;
using namespace degate;

void LogicModelExporter::export_data(std::string const& filename, LogicModel_shptr lmodel)
{
	if (lmodel == NULL) throw InvalidPointerException("Logic model pointer is NULL.");

	try
	{
		QDomDocument doc;

		QDomProcessingInstruction head = doc.createProcessingInstruction("xml", XML_ENCODING);
		doc.appendChild(head);

		QDomElement root_elem = doc.createElement("logic-model");
		assert(!root_elem.isNull());

		QDomElement gates_elem = doc.createElement("gates");
		if (gates_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		QDomElement vias_elem = doc.createElement("vias");
		if (vias_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		QDomElement emarkers_elem = doc.createElement("emarkers");
		if (emarkers_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		QDomElement wires_elem = doc.createElement("wires");
		if (wires_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		QDomElement nets_elem = doc.createElement("nets");
		if (nets_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		QDomElement annotations_elem = doc.createElement("annotations");
		if (annotations_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		for (auto layer_iter = lmodel->layers_begin();
		     layer_iter != lmodel->layers_end(); ++layer_iter)
		{
			if ((*layer_iter) == NULL || (*layer_iter)->is_empty())
				continue;

			Layer_shptr layer = *layer_iter;

			for (Layer::object_iterator iter = layer->objects_begin();
			     iter != layer->objects_end(); ++iter)
			{
				layer_position_t layer_pos = layer->get_layer_pos();

				PlacedLogicModelObject_shptr o = (*iter);

				if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(o))
					add_gate(doc, gates_elem, gate, layer_pos);

				else if (Via_shptr via = std::dynamic_pointer_cast<Via>(o))
					add_via(doc, vias_elem, via, layer_pos);

				else if (EMarker_shptr emarker = std::dynamic_pointer_cast<EMarker>(o))
					add_emarker(doc, emarkers_elem, emarker, layer_pos);

				else if (Wire_shptr wire = std::dynamic_pointer_cast<Wire>(o))
					add_wire(doc, wires_elem, wire, layer_pos);

				else if (Annotation_shptr annotation = std::dynamic_pointer_cast<Annotation>(o))
					add_annotation(doc, annotations_elem, annotation, layer_pos);
			}
		}

		add_nets(doc, nets_elem, lmodel);

		// actually we have only one main module

		// First update the module ports.
		determine_module_ports_for_root(lmodel); // Update main module itself.
		lmodel->get_main_module()->determine_module_ports_recursive(); // Update all of main module's children.

		QDomElement modules_elem = doc.createElement("modules");
		if (modules_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		add_module(doc, modules_elem, lmodel, lmodel->get_main_module());

		root_elem.appendChild(gates_elem);
		root_elem.appendChild(vias_elem);
		root_elem.appendChild(emarkers_elem);
		root_elem.appendChild(wires_elem);
		root_elem.appendChild(annotations_elem);
		root_elem.appendChild(nets_elem);
		root_elem.appendChild(modules_elem);

		doc.appendChild(root_elem);

		QFile file(QString::fromStdString(filename));
		if (!file.open(QIODevice::WriteOnly))
		{
			throw InvalidPathException("Can't create export file.");
		}

		QTextStream stream(&file);
        stream.setCodec("UTF-8");
		stream << doc.toString();

		file.close();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
		throw;
	}
}

void LogicModelExporter::add_nets(QDomDocument& doc, QDomElement& nets_elem, LogicModel_shptr lmodel)
{
	for (LogicModel::net_collection::iterator net_iter = lmodel->nets_begin();
	     net_iter != lmodel->nets_end(); ++net_iter)
	{
		QDomElement net_elem = doc.createElement("net");

		Net_shptr net = net_iter->second;
		assert(net != NULL);

		object_id_t old_net_id = net->get_object_id();
		assert(old_net_id != 0);
		object_id_t new_net_id = oid_rewriter->get_new_object_id(old_net_id);

		net_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_net_id)));

		for (Net::connection_iterator conn_iter = net->begin();
		     conn_iter != net->end(); ++conn_iter)
		{
			object_id_t oid = *conn_iter;

			const ConnectedLogicModelObject_shptr conn_obj =
				std::dynamic_pointer_cast<ConnectedLogicModelObject>(lmodel->get_object(oid));

			QDomElement conn_elem = doc.createElement("connection");
			conn_elem.setAttribute("object-id",
			                       QString::fromStdString(
				                       number_to_string<object_id_t>(oid_rewriter->get_new_object_id(oid))));
			net_elem.appendChild(conn_elem);
		}

		nets_elem.appendChild(net_elem);
	}
}

void LogicModelExporter::add_gate(QDomDocument& doc, QDomElement& gates_elem, Gate_shptr gate,
                                  layer_position_t layer_pos)
{
	QDomElement gate_elem = doc.createElement("gate");
	if (gate_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	object_id_t new_oid = oid_rewriter->get_new_object_id(gate->get_object_id());
	gate_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
	gate_elem.setAttribute("name", QString::fromStdString(gate->get_name()));
	gate_elem.setAttribute("description", QString::fromStdString(gate->get_description()));
	gate_elem.setAttribute("layer", QString::fromStdString(number_to_string<layer_position_t>(layer_pos)));
	gate_elem.setAttribute("orientation", QString::fromStdString(gate->get_orienation_type_as_string()));

	gate_elem.setAttribute("min-x", QString::fromStdString(number_to_string<float>(gate->get_min_x())));
	gate_elem.setAttribute("min-y", QString::fromStdString(number_to_string<float>(gate->get_min_y())));
	gate_elem.setAttribute("max-x", QString::fromStdString(number_to_string<float>(gate->get_max_x())));
	gate_elem.setAttribute("max-y", QString::fromStdString(number_to_string<float>(gate->get_max_y())));

	gate_elem.setAttribute("type-id",
	                       QString::fromStdString(number_to_string<object_id_t>(
		                       oid_rewriter->get_new_object_id(gate->get_template_type_id()))));


	for (Gate::port_iterator iter = gate->ports_begin();
	     iter != gate->ports_end(); ++iter)
	{
		GatePort_shptr port = *iter;

		QDomElement port_elem = doc.createElement("port");
		if (port_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		object_id_t new_port_id = oid_rewriter->get_new_object_id(port->get_object_id());
		port_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_port_id)));

		if (port->get_name().size() > 0) port_elem.setAttribute("name", QString::fromStdString(port->get_name()));
		if (port->get_description().size() > 0) port_elem.setAttribute("description",
		                                                               QString::fromStdString(port->get_description()));

		object_id_t new_type_id = oid_rewriter->get_new_object_id(port->get_template_port_type_id());
		port_elem.setAttribute("type-id", QString::fromStdString(number_to_string<object_id_t>(new_type_id)));

		port_elem.setAttribute("diameter", QString::fromStdString(number_to_string<diameter_t>(port->get_diameter())));

		gate_elem.appendChild(port_elem);
	}

	gates_elem.appendChild(gate_elem);
}

void LogicModelExporter::add_wire(QDomDocument& doc, QDomElement& wires_elem, Wire_shptr wire,
                                  layer_position_t layer_pos)
{
	QDomElement wire_elem = doc.createElement("wire");
	if (wire_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	object_id_t new_oid = oid_rewriter->get_new_object_id(wire->get_object_id());
	wire_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
	wire_elem.setAttribute("name", QString::fromStdString(wire->get_name()));
	wire_elem.setAttribute("description", QString::fromStdString(wire->get_description()));
	wire_elem.setAttribute("layer", QString::fromStdString(number_to_string<layer_position_t>(layer_pos)));
	wire_elem.setAttribute("diameter", QString::fromStdString(number_to_string<unsigned int>(wire->get_diameter())));

	wire_elem.setAttribute("from-x", QString::fromStdString(number_to_string<float>(wire->get_from_x())));
	wire_elem.setAttribute("from-y", QString::fromStdString(number_to_string<float>(wire->get_from_y())));
	wire_elem.setAttribute("to-x", QString::fromStdString(number_to_string<float>(wire->get_to_x())));
	wire_elem.setAttribute("to-y", QString::fromStdString(number_to_string<float>(wire->get_to_y())));

	wire_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(wire->get_fill_color())));
	wire_elem.setAttribute("frame-color", QString::fromStdString(to_color_string(wire->get_frame_color())));

	wire_elem.setAttribute("remote-id",
	                       QString::fromStdString(number_to_string<object_id_t>(wire->get_remote_object_id())));

	wires_elem.appendChild(wire_elem);
}

void LogicModelExporter::add_via(QDomDocument& doc, QDomElement& vias_elem, Via_shptr via, layer_position_t layer_pos)
{
	QDomElement via_elem = doc.createElement("via");
	if (via_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	object_id_t new_oid = oid_rewriter->get_new_object_id(via->get_object_id());
	via_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
	via_elem.setAttribute("name", QString::fromStdString(via->get_name()));
	via_elem.setAttribute("description", QString::fromStdString(via->get_description()));
	via_elem.setAttribute("layer", QString::fromStdString(number_to_string<layer_position_t>(layer_pos)));
	via_elem.setAttribute("diameter", QString::fromStdString(number_to_string<unsigned int>(via->get_diameter())));

	via_elem.setAttribute("x", QString::fromStdString(number_to_string<float>(via->get_x())));
	via_elem.setAttribute("y", QString::fromStdString(number_to_string<float>(via->get_y())));

	via_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(via->get_fill_color())));
	via_elem.setAttribute("frame-color", QString::fromStdString(to_color_string(via->get_frame_color())));

	via_elem.setAttribute("direction", QString::fromStdString(via->get_direction_as_string()));
	via_elem.setAttribute("remote-id",
	                      QString::fromStdString(number_to_string<object_id_t>(via->get_remote_object_id())));

	vias_elem.appendChild(via_elem);
}

void LogicModelExporter::add_emarker(QDomDocument& doc, QDomElement& emarkers_elem, EMarker_shptr emarker,
                                     layer_position_t layer_pos)
{
	QDomElement emarker_elem = doc.createElement("emarker");
	if (emarker_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	object_id_t new_oid = oid_rewriter->get_new_object_id(emarker->get_object_id());
	emarker_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
	emarker_elem.setAttribute("name", QString::fromStdString(emarker->get_name()));
	emarker_elem.setAttribute("description", QString::fromStdString(emarker->get_description()));
	emarker_elem.setAttribute("layer", QString::fromStdString(number_to_string<layer_position_t>(layer_pos)));
	emarker_elem.setAttribute(
		"diameter", QString::fromStdString(number_to_string<unsigned int>(emarker->get_diameter())));

	emarker_elem.setAttribute("x", QString::fromStdString(number_to_string<float>(emarker->get_x())));
	emarker_elem.setAttribute("y", QString::fromStdString(number_to_string<float>(emarker->get_y())));

	emarker_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(emarker->get_fill_color())));
	emarker_elem.setAttribute("frame-color", QString::fromStdString(to_color_string(emarker->get_frame_color())));

	emarker_elem.setAttribute("remote-id",
	                          QString::fromStdString(number_to_string<object_id_t>(emarker->get_remote_object_id())));

	emarkers_elem.appendChild(emarker_elem);
}


void LogicModelExporter::add_annotation(QDomDocument& doc, QDomElement& annotations_elem, Annotation_shptr annotation,
                                        layer_position_t layer_pos)
{
	QDomElement annotation_elem = doc.createElement("annotation");
	if (annotation_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	object_id_t new_oid = oid_rewriter->get_new_object_id(annotation->get_object_id());
	annotation_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
	annotation_elem.setAttribute("name", QString::fromStdString(annotation->get_name()));
	annotation_elem.setAttribute("description", QString::fromStdString(annotation->get_description()));
	annotation_elem.setAttribute("layer", QString::fromStdString(number_to_string<layer_position_t>(layer_pos)));
	annotation_elem.setAttribute(
		"class-id", QString::fromStdString(number_to_string<layer_position_t>(annotation->get_class_id())));

	annotation_elem.setAttribute("min-x", QString::fromStdString(number_to_string<float>(annotation->get_min_x())));
	annotation_elem.setAttribute("min-y", QString::fromStdString(number_to_string<float>(annotation->get_min_y())));
	annotation_elem.setAttribute("max-x", QString::fromStdString(number_to_string<float>(annotation->get_max_x())));
	annotation_elem.setAttribute("max-y", QString::fromStdString(number_to_string<float>(annotation->get_max_y())));

	annotation_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(annotation->get_fill_color())));
	annotation_elem.setAttribute("frame-color", QString::fromStdString(to_color_string(annotation->get_frame_color())));

	for (Annotation::parameter_set_type::const_iterator iter = annotation->parameters_begin();
	     iter != annotation->parameters_end(); ++iter)
	{
		annotation_elem.setAttribute(QString::fromStdString(iter->first), QString::fromStdString(iter->second));
	}

	annotations_elem.appendChild(annotation_elem);
}


void LogicModelExporter::add_module(QDomDocument& doc, QDomElement& modules_elem, LogicModel_shptr lmodel,
                                    Module_shptr module)
{
	QDomElement this_elem = doc.createElement("module");
	if (this_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	QDomElement module_ports_elem = doc.createElement("module-ports");
	QDomElement cells_elem = doc.createElement("cells");
	QDomElement sub_modules_elem = doc.createElement("modules");
	if (module_ports_elem.isNull() ||
		cells_elem.isNull() ||
		sub_modules_elem.isNull())
		throw(std::runtime_error("Failed to create node."));

	/*
	  <module id="42" name="ff23" entity-type="flip-flop">
  
	    <module-ports>
	      <module-port name="d" object-id="666"/> -- connected with object 666
	      <module-port name="q" object-id="667"/>
	    </module-ports>
  
	    <cells>
	      <cell id="9999"/>
	    </cells>
  
	    <modules>
	      ...
	    </modules>
  
	  </module>
  
	*/

	// module itself

	object_id_t new_mod_id = oid_rewriter->get_new_object_id(module->get_object_id());
	this_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_mod_id)));
	this_elem.setAttribute("name", QString::fromStdString(module->get_name()));
	this_elem.setAttribute("entity", QString::fromStdString(module->get_entity_name()));

	// write module ports
	for (Module::port_collection::const_iterator p_iter = module->ports_begin();
	     p_iter != module->ports_end(); ++p_iter)
	{
		QDomElement mport_elem = doc.createElement("module-port");
		if (mport_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		GatePort_shptr gport = p_iter->second;

		mport_elem.setAttribute("name", QString::fromStdString(p_iter->first));
		new_mod_id = oid_rewriter->get_new_object_id(gport->get_object_id());
		mport_elem.setAttribute("object-id",
		                        QString::fromStdString(number_to_string<object_id_t>(new_mod_id)));

		module_ports_elem.appendChild(mport_elem);
	}

	// write standard cells
	for (Module::gate_collection::const_iterator g_iter = module->gates_begin();
	     g_iter != module->gates_end(); ++g_iter)
	{
		QDomElement cell_elem = doc.createElement("cell");
		if (cell_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		new_mod_id = oid_rewriter->get_new_object_id((*g_iter)->get_object_id());
		cell_elem.setAttribute("object-id",
		                       QString::fromStdString(number_to_string<object_id_t>(new_mod_id)));

		cells_elem.appendChild(cell_elem);
	}

	// write sub-modules
	for (Module::module_collection::const_iterator m_iter = module->modules_begin();
	     m_iter != module->modules_end(); ++m_iter)
	{
		add_module(doc, sub_modules_elem, lmodel, *m_iter);
	}

	this_elem.appendChild(sub_modules_elem);
	this_elem.appendChild(cells_elem);
	this_elem.appendChild(module_ports_elem);

	modules_elem.appendChild(this_elem);
}
