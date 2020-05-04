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

#include <Degate.h>
#include <Core/GateLibraryImporter.h>
#include <Core/ImageHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h> : Linux only
#include <cerrno>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <memory>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace degate;

GateLibrary_shptr GateLibraryImporter::import(std::string const& filename)
{
	if (RET_IS_NOT_OK(check_file(filename)))
	{
		debug(TM, "Problem: file %s not found.", filename.c_str());
		throw InvalidPathException("Can't load gate library from file.");
	}

	std::string directory = get_basedir(filename);

	try
	{
		QDomDocument parser;

		QFile file(QString::fromStdString(filename));
		if (!file.open(QIODevice::ReadOnly))
		{
			debug(TM, "Problem: can't open the file %s.", filename.c_str());
			throw InvalidFileFormatException(
				"The GateLibraryImporter cannot load the project file. Can't open the file.");
		}

		if (!parser.setContent(&file))
		{
			debug(TM, "Problem: can't parse the file %s.", filename.c_str());
			throw InvalidXMLException("The GateLibraryImporter cannot load the project file. Can't parse the file.");
		}
		file.close();

		const QDomElement root_elem = parser.documentElement();
		assert(!root_elem.isNull());

		return parse_gate_library_element(root_elem, directory);
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
		throw;
	}
}

GateLibrary_shptr GateLibraryImporter::parse_gate_library_element(QDomElement const gt_elem,
                                                                  std::string const& directory)
{
	// parse width and height
	GateLibrary_shptr gate_lib(new GateLibrary());
	assert(gate_lib != NULL);

	const QDomElement e = get_dom_twig(gt_elem, "gate-templates");
	if (!e.isNull()) parse_gate_templates_element(e, gate_lib, directory);

	return gate_lib;
}

void GateLibraryImporter::parse_gate_templates_element(QDomElement const gate_template_element,
                                                       GateLibrary_shptr gate_lib,
                                                       std::string const& directory)
{
	if (gate_template_element.isNull() || gate_lib == NULL) throw(InvalidPointerException("Invalid pointer"));

	const QDomNodeList gate_list = gate_template_element.elementsByTagName("gate");

	QDomElement gate_elem;
	for (int i = 0; i < gate_list.count(); i++)
	{
		gate_elem = gate_list.at(i).toElement();
		if (!gate_elem.isNull())
		{
			object_id_t object_id = parse_number<object_id_t>(gate_elem, "type-id");
            float min_x = parse_number<float>(gate_elem, "min-x", 0);
            float min_y = parse_number<float>(gate_elem, "min-y", 0);
            float max_x = parse_number<float>(gate_elem, "max-x", 0);
            float max_y = parse_number<float>(gate_elem, "max-y", 0);

            float width = parse_number<float>(gate_elem, "width", 0);
            float height = parse_number<float>(gate_elem, "height", 0);

			const std::string name(gate_elem.attribute("name").toStdString());
			const std::string description(gate_elem.attribute("description").toStdString());
			const std::string logic_class(gate_elem.attribute("logic-class").toStdString());
			const std::string frame_color_str(gate_elem.attribute("frame-color").toStdString());
			const std::string fill_color_str(gate_elem.attribute("fill-color").toStdString());

			GateTemplate_shptr gate_template;

			if (min_x == 0 && min_y == 0 && max_x == 0 && max_y == 0)
			{
				gate_template = GateTemplate_shptr(new GateTemplate(width, height));
			}
			else
			{
				gate_template = GateTemplate_shptr(new GateTemplate(min_x, max_x, min_y, max_y));
			}

			gate_template->set_name(string(name.c_str()));
			gate_template->set_description(string(description.c_str()));
			gate_template->set_logic_class(string(logic_class.c_str()));
			gate_template->set_object_id(object_id);
			gate_template->set_fill_color(parse_color_string(fill_color_str));
			gate_template->set_frame_color(parse_color_string(frame_color_str));


			const QDomElement images = get_dom_twig(gate_elem, "images");
			if (!images.isNull()) parse_template_images_element(images, gate_template, directory);


			const QDomElement ports = get_dom_twig(gate_elem, "ports");
			if (!ports.isNull()) parse_template_ports_element(ports, gate_template, gate_lib);

			const QDomElement implementations = get_dom_twig(gate_elem, "implementations");
			if (!implementations.isNull())
				parse_template_implementations_element(implementations, gate_template, directory);

			gate_lib->add_template(gate_template);
		}
	}
}

void GateLibraryImporter::parse_template_images_element(QDomElement const template_images_element,
                                                        GateTemplate_shptr gate_tmpl,
                                                        std::string const& directory)
{
	if (template_images_element.isNull() ||
		gate_tmpl == NULL)
		throw InvalidPointerException("Invalid pointer");

	const QDomNodeList image_list = template_images_element.elementsByTagName("image");

	QDomElement image_elem;
	for (int i = 0; i < image_list.count(); i++)
	{
		image_elem = image_list.at(i).toElement();
		if (!image_elem.isNull())
		{
			const std::string layer_type_str(image_elem.attribute("layer-type").toStdString());
			const std::string image_file(image_elem.attribute("image").toStdString());

			Layer::LAYER_TYPE layer_type = Layer::get_layer_type_from_string(layer_type_str);
			GateTemplateImage_shptr img = load_image<GateTemplateImage>(join_pathes(directory, image_file));

			assert(img != NULL);
			gate_tmpl->set_image(layer_type, img);
		}
	}
}

void GateLibraryImporter::parse_template_implementations_element(QDomElement const implementations_element,
                                                                 GateTemplate_shptr gate_tmpl,
                                                                 std::string const& directory)
{
	if (implementations_element.isNull() ||
		gate_tmpl == NULL)
		throw InvalidPointerException("Invalid pointer");

	const QDomNodeList impl_list = implementations_element.elementsByTagName("implementation");

	QDomElement impl_elem;
	for (int i = 0; i < impl_list.count(); i++)
	{
		impl_elem = impl_list.at(i).toElement();
		if (!impl_elem.isNull())
		{
			const std::string impl_type_str(impl_elem.attribute("type").toStdString());
			const std::string impl_file_attr(impl_elem.attribute("file").toStdString());
			const std::string impl_file(join_pathes(directory, impl_file_attr));

			if (!impl_file_attr.empty())
			{
				GateTemplate::IMPLEMENTATION_TYPE impl_type;
				try
				{
					impl_type = GateTemplate::get_impl_type_from_string(impl_type_str);
				}
				catch (DegateRuntimeException const& ex)
				{
					boost::format f("Can't interprete attribute %1% while reading gate library (%2%)");
					f % impl_type_str % ex.what();
					throw XMLAttributeParseException(f.str());
				}

				debug(TM, "Parsing file '%s'", impl_file.c_str());
				std::ifstream myfile(impl_file.c_str());
				std::string line, code;
				if (myfile.is_open())
				{
					while (!myfile.eof())
					{
						getline(myfile, line);
						code.append(line);
						code.append("\n");
					}
					myfile.close();
					gate_tmpl->set_implementation(impl_type, code);
					gate_tmpl->get_implementation(impl_type);
				}
				else
				{
					boost::format f("Can't open file %1%");
					f % impl_file;
					throw FileSystemException(f.str());
				}
			}
		}
	}
}


void GateLibraryImporter::parse_template_ports_element(QDomElement const template_ports_element,
                                                       GateTemplate_shptr gate_tmpl,
                                                       GateLibrary_shptr gate_lib)
{
	if (template_ports_element.isNull() ||
		gate_tmpl == NULL)
		throw InvalidPointerException("Invalid pointer");

	const QDomNodeList port_list = template_ports_element.elementsByTagName("port");

	QDomElement port_elem;
	for (int i = 0; i < port_list.count(); i++)
	{
		port_elem = port_list.at(i).toElement();
		if (!port_elem.isNull())
		{
			object_id_t object_id = parse_number<object_id_t>(port_elem, "id");

			// we check if the object id is somehow present in the logic model
			if (gate_lib->exists_template_port(object_id) || gate_lib->exists_template(object_id))
			{
				boost::format f("Error: The object ID %1% is used twice in the gate library.");
				f % object_id;
				std::cout << f.str() << "\n";
				throw DegateInconsistencyException(f.str());
			}


			const std::string name(port_elem.attribute("name").toStdString());
			const std::string description(port_elem.attribute("description").toStdString());
			const std::string type_str(boost::algorithm::to_lower_copy(port_elem.attribute("type").toStdString()));

            // color
            QString port_fill_color = port_elem.attribute("fill-color");

			GateTemplatePort::PORT_TYPE port_type = GateTemplatePort::get_port_type_from_string(type_str);

			GateTemplatePort_shptr tmpl_port;

            float pos_x, pos_y;
			try
			{
				pos_x = parse_number<float>(port_elem, "x");
				pos_y = parse_number<float>(port_elem, "y");

				tmpl_port = GateTemplatePort_shptr(new GateTemplatePort(pos_x, pos_y, port_type));
			}
			catch (XMLAttributeMissingException const& ex)
			{
				tmpl_port = GateTemplatePort_shptr(new GateTemplatePort(port_type));
			}
			assert(tmpl_port != NULL);


			tmpl_port->set_name(string(name.c_str()));
			tmpl_port->set_description(string(description.c_str()));
			tmpl_port->set_object_id(object_id);

			// color
            if(!port_fill_color.isNull())
                tmpl_port->set_fill_color(parse_color_string(port_fill_color.toStdString()));

			// add port to the gate template
			gate_tmpl->add_template_port(tmpl_port);
		}
	}
}
