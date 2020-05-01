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

#include <degate.h>
#include <GateLibraryExporter.h>
#include <FileSystem.h>
#include <ImageHelper.h>
#include <DegateHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h> : Linux only
#include <cerrno>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <list>
#include <memory>


using namespace std;
using namespace degate;

void GateLibraryExporter::export_data(std::string const& filename, GateLibrary_shptr gate_lib)
{
	if (gate_lib == NULL) throw InvalidPointerException("Gate library pointer is NULL.");

	std::string directory = get_basedir(filename);

	try
	{
		QDomDocument doc;

		QDomProcessingInstruction head = doc.createProcessingInstruction("xml", XML_ENCODING);
		doc.appendChild(head);

		QDomElement root_elem = doc.createElement("gate-library");
		assert(!root_elem.isNull());

		QDomElement templates_elem = doc.createElement("gate-templates");
		if (templates_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		add_gates(doc, templates_elem, gate_lib, directory);

		root_elem.appendChild(templates_elem);

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

void GateLibraryExporter::add_gates(QDomDocument& doc,
                                    QDomElement& templates_elem,
                                    GateLibrary_shptr gate_lib,
                                    std::string const& directory)
{
	for (GateLibrary::template_iterator iter = gate_lib->begin();
	     iter != gate_lib->end(); ++iter)
	{
		GateTemplate_shptr gate_tmpl((*iter).second);

		QDomElement gate_elem = doc.createElement("gate");
		if (gate_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		object_id_t new_oid = oid_rewriter->get_new_object_id(gate_tmpl->get_object_id());
		gate_elem.setAttribute("type-id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
		gate_elem.setAttribute("name", QString::fromStdString(gate_tmpl->get_name()));
		gate_elem.setAttribute("description", QString::fromStdString(gate_tmpl->get_description()));
		gate_elem.setAttribute("logic-class", QString::fromStdString(gate_tmpl->get_logic_class()));

		gate_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(gate_tmpl->get_fill_color())));
		gate_elem.setAttribute("frame-color", QString::fromStdString(to_color_string(gate_tmpl->get_frame_color())));

		gate_elem.setAttribute("width", QString::fromStdString(number_to_string<float>(gate_tmpl->get_width())));
		gate_elem.setAttribute(
			"height", QString::fromStdString(number_to_string<float>(gate_tmpl->get_height())));

		add_images(doc, gate_elem, gate_tmpl, directory);
		add_ports(doc, gate_elem, gate_tmpl);
		add_implementations(doc, gate_elem, gate_tmpl, directory);

		templates_elem.appendChild(gate_elem);
	}
}


void GateLibraryExporter::add_images(QDomDocument& doc,
                                     QDomElement& gate_elem,
                                     GateTemplate_shptr gate_tmpl,
                                     std::string const& directory)
{
	// export images

	QDomElement images_elem = doc.createElement("images");
	if (images_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	for (GateTemplate::image_iterator img_iter = gate_tmpl->images_begin();
	     img_iter != gate_tmpl->images_end(); ++img_iter)
	{
		Layer::LAYER_TYPE layer_type = (*img_iter).first;
		GateTemplateImage_shptr img = (*img_iter).second;
		assert(img != NULL);

		QDomElement img_elem = doc.createElement("image");
		if (img_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		img_elem.setAttribute("layer-type", QString::fromStdString(Layer::get_layer_type_as_string(layer_type)));

		// export the image
		object_id_t new_oid = oid_rewriter->get_new_object_id(gate_tmpl->get_object_id());
		boost::format fmter("%1%_%2%.tif");
		fmter % new_oid % Layer::get_layer_type_as_string(layer_type);
		std::string filename(fmter.str());

		img_elem.setAttribute("image", QString::fromStdString(filename));

		save_image<GateTemplateImage>(join_pathes(directory, filename), img);

		images_elem.appendChild(img_elem);
	}

	gate_elem.appendChild(images_elem);
}

void GateLibraryExporter::add_ports(QDomDocument& doc,
                                    QDomElement& gate_elem,
                                    GateTemplate_shptr gate_tmpl)
{
	QDomElement ports_elem = doc.createElement("ports");
	if (ports_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	for (GateTemplate::port_iterator piter = gate_tmpl->ports_begin();
	     piter != gate_tmpl->ports_end(); ++piter)
	{
		QDomElement port_elem = doc.createElement("port");
		if (port_elem.isNull()) throw(std::runtime_error("Failed to create node."));

		GateTemplatePort_shptr tmpl_port((*piter));

		object_id_t new_port_id = oid_rewriter->get_new_object_id(tmpl_port->get_object_id());
		port_elem.setAttribute("id", QString::fromStdString(number_to_string<object_id_t>(new_port_id)));
		port_elem.setAttribute("name", QString::fromStdString(tmpl_port->get_name()));
		port_elem.setAttribute("description", QString::fromStdString(tmpl_port->get_description()));

		// color
        port_elem.setAttribute("fill-color", QString::fromStdString(to_color_string(tmpl_port->get_fill_color())));

		port_elem.setAttribute("type", QString::fromStdString(tmpl_port->get_port_type_as_string()));

		if (tmpl_port->is_position_defined())
		{
			Point const& point = tmpl_port->get_point();
			port_elem.setAttribute("x", QString::fromStdString(number_to_string<float>(point.get_x())));
			port_elem.setAttribute("y", QString::fromStdString(number_to_string<float>(point.get_y())));
		}

		ports_elem.appendChild(port_elem);
	}

	gate_elem.appendChild(ports_elem);
}

void GateLibraryExporter::add_implementations(QDomDocument& doc,
                                              QDomElement& gate_elem,
                                              GateTemplate_shptr gate_tmpl,
                                              std::string const& directory)
{
	QDomElement implementations_elem = doc.createElement("implementations");
	if (implementations_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	for (GateTemplate::implementation_iter iter = gate_tmpl->implementations_begin();
	     iter != gate_tmpl->implementations_end(); ++iter)
	{
		GateTemplate::IMPLEMENTATION_TYPE t = iter->first;
		std::string const& code = iter->second;

		//std::cout << "Code: " << code;
		if (t != GateTemplate::UNDEFINED && !code.empty())
		{
			QDomElement impl_elem = doc.createElement("implementation");
			if (impl_elem.isNull()) throw(std::runtime_error("Failed to create node."));

			object_id_t new_oid = oid_rewriter->get_new_object_id(gate_tmpl->get_object_id());
			boost::format fmter("%1%%2%.%3%");
			switch (t)
			{
			case GateTemplate::TEXT: fmter % "" % new_oid % "txt";
				break;
			case GateTemplate::VHDL: fmter % "" % new_oid % "vhdl";
				break;
			case GateTemplate::VHDL_TESTBENCH: fmter % "test_" % new_oid % "vhdl";
				break;
			case GateTemplate::VERILOG: fmter % "" % new_oid % "v";
				break;
			case GateTemplate::VERILOG_TESTBENCH: fmter % "test_" % new_oid % "v";
				break;
			default: assert(1==0); // already handled. just to get rid of a compiler warning.
			}
			std::string filename(fmter.str());

			write_string_to_file(join_pathes(directory, filename), code);

			impl_elem.setAttribute("type", QString::fromStdString(GateTemplate::get_impl_type_as_string(t)));
			impl_elem.setAttribute("file", QString::fromStdString(filename));

			implementations_elem.appendChild(impl_elem);
		}
	}

	gate_elem.appendChild(implementations_elem);
}
