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

#include <Core/ProjectImporter.h>
#include <Core/Layer.h>
#include <Core/FileSystem.h>
#include <Core/DegateExceptions.h>
#include <Core/GateLibraryImporter.h>
#include <Core/LogicModelImporter.h>
#include <Core/RCVBlacklistImporter.h>
#include <Core/PortColorManager.h>
#include <Core/Image.h>
#include <Core/LogicModelHelper.h>
#include <Core/ImageHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h> : Linux only
#include <cerrno>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <boost/format.hpp>

using namespace std;
using namespace degate;

std::string ProjectImporter::get_project_filename(std::string const& dir) const
{
	if (is_directory(dir))
		return join_pathes(dir, "project.xml");
	else
		return dir;
}

Project_shptr ProjectImporter::import_all(std::string const& directory)
{
	Project_shptr prj = import(directory);

	if (prj != NULL)
	{
		GateLibraryImporter gl_importer;

		GateLibrary_shptr gate_lib;

		std::string gate_lib_file(get_basedir(directory) + "/gate_library.xml");
		std::string rcbl_file(get_basedir(directory) + "/rc_blacklist.xml");

		if (file_exists(gate_lib_file))
			gate_lib = gl_importer.import(gate_lib_file);
		else gate_lib = GateLibrary_shptr(new GateLibrary());

		LogicModelImporter lm_importer(prj->get_width(), prj->get_height(), gate_lib);

		lm_importer.import_into(prj->get_logic_model(),
		                        get_basedir(directory) + "/lmodel.xml");

		LogicModel_shptr lmodel = prj->get_logic_model();
		lmodel->set_default_gate_port_diameter(prj->get_default_port_diameter());

		if (file_exists(rcbl_file))
		{
			RCVBlacklistImporter rcvbl_importer(lmodel);
			rcvbl_importer.import_into(rcbl_file, prj->get_rcv_blacklist());
		}

		/*
	  For degate projects that were exported with degate 0.0.6 the gate templates
	  were expressed in terms of an image region. This is bad. Here is a part of the fix:
	  We have loaded the project with the background images and we have the gate
	  library. We iterate over the gate library, extract the template image from the
	  background image and put it into the gate library. We do it for the first
	  transistor, the first logic and the first metal layer.
		*/

		debug(TM, "Check if we have template images.");
		for (GateLibrary::template_iterator iter = gate_lib->begin();
		     iter != gate_lib->end(); ++iter)
		{
			debug(TM, "Will grab template image for gate template ID: %d", iter->first);
			GateTemplate_shptr tmpl = iter->second;
			assert(tmpl != NULL);

			BoundingBox const& bbox = tmpl->get_bounding_box();
			if (bbox.get_min_x() != 0 && bbox.get_min_y() != 0 &&
				bbox.get_max_x() != 0 && bbox.get_max_y() != 0)
			{
				// a heuristic
				debug(TM, "Grab template images from the background for template %s.", tmpl->get_name().c_str());
				grab_template_images(lmodel, tmpl, bbox);
			}
		}
		debug(TM, "Project loaded.");
		//prj->print_all(cout);
	}

	return prj;
}

Project_shptr ProjectImporter::import(std::string const& directory)
{
	string filename = get_project_filename(directory);
	if (RET_IS_NOT_OK(check_file(filename)))
	{
		debug(TM, "Problem: file %s not found.", filename.c_str());
		throw InvalidPathException("The ProjectImporter cannot load the project file. File does not exists.");
	}

	try
	{
		QDomDocument parser;

		QFile file(QString::fromStdString(filename));
		if (!file.open(QIODevice::ReadOnly))
		{
			debug(TM, "Problem: can't open the file %s.", filename.c_str());
			throw InvalidFileFormatException("The ProjectImporter cannot load the project file. Can't open the file.");
		}

		if (!parser.setContent(&file))
		{
			debug(TM, "Problem: can't parse the file %s.", filename.c_str());
			throw InvalidXMLException("The ProjectImporter cannot load the project file. Can't parse the file.");
		}
		file.close();

		const QDomElement root_elem = parser.documentElement();
		assert(!root_elem.isNull());

		// parse width and height
		int w = parse_number<length_t>(root_elem, "width");
		int h = parse_number<length_t>(root_elem, "height");

		Project_shptr prj(new Project(w, h, get_basedir(filename)));
		assert(prj->get_project_directory().length() != 0);

		parse_project_element(prj, root_elem);

		return prj;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
		throw InvalidXMLException(ex.what());
	}
}


void ProjectImporter::parse_layers_element(QDomElement const layers_elem, Project_shptr prj)
{
	debug(TM, "parsing layers");

	const QDomNodeList layer_list = layers_elem.elementsByTagName("layer");
	QDomElement layer_elem;
	for (int i = 0; i < layer_list.count(); i++)
	{
		layer_elem = layer_list.at(i).toElement();

		if (!layer_elem.isNull())
		{
			const std::string image_filename(layer_elem.attribute("image-filename").toStdString());
			const std::string layer_type_str(layer_elem.attribute("type").toStdString());
			const std::string layer_description(layer_elem.attribute("description").toStdString());
			unsigned int position = parse_number<unsigned int>(layer_elem, "position");
			const std::string layer_enabled_str = layer_elem.attribute("enabled").toStdString();

			Layer::LAYER_TYPE layer_type = Layer::get_layer_type_from_string(layer_type_str);
			layer_id_t layer_id = parse_number<layer_id_t>(layer_elem, "id", 0);

			Layer_shptr new_layer = std::make_shared<Layer>(prj->get_bounding_box(), layer_type);
			LogicModel_shptr lmodel = prj->get_logic_model();

			debug(TM, "Parsed a layer entry for type %s. This is a %s layer. Background image is %s",
			      layer_type_str.c_str(), Layer::get_layer_type_as_string(layer_type).c_str(), image_filename.c_str());

			bool layer_enabled = true;
			if (layer_enabled_str.size() != 0)
				layer_enabled = parse_bool(layer_enabled_str);
			new_layer->set_enabled(layer_enabled);

			new_layer->set_description(layer_description);
			new_layer->set_layer_id(layer_id);

			lmodel->add_layer(position, new_layer);

			load_background_image(new_layer, image_filename, prj);
		}
	}
}

void ProjectImporter::load_background_image(Layer_shptr layer,
                                            std::string const& image_filename,
                                            Project_shptr prj)
{
	debug(TM, "try to load image [%s]", image_filename.c_str());
	if (!image_filename.empty())
	{
		assert(prj->get_project_directory().length() != 0);

		std::string image_path_to_load = join_pathes(prj->get_project_directory(), image_filename);

		debug(TM, "try to load image [%s]", image_path_to_load.c_str());

		if (is_directory(image_path_to_load))
		{
			// new background image format

			debug(TM, "project importer loads an tile based image from [%s]", image_path_to_load.c_str());

			BackgroundImage_shptr bg_image =
				load_degate_image<BackgroundImage>(prj->get_width(),
				                                   prj->get_height(),
				                                   image_path_to_load);

			if (bg_image == NULL)
				throw DegateRuntimeException("Failed to load the background image");

			debug(TM, "Loading done.");
			layer->set_image(bg_image);
		}
		else if (is_file(image_path_to_load))
		{
			// old image background format

			// determine where we can store the new backgound image
			boost::format fmter("layer_%1%.dimg");
			/* We convert old degate background images to the new tile based format.
		   Therefore we need directory names. The directory name should not reflect
		   a layer position number, because it it possible that the layers become
		   reorderd later. We do not want to rename the directories in that case.
		   To avoid, that a user thinks the directory name reflects a layer position,
		   we just add a number. */
			fmter % (layer->get_layer_pos() + 0x2342);
			std::string new_dir(join_pathes(prj->get_project_directory(), fmter.str()));

			debug(TM, "project importer loads an old single file based image from [%s]",
			      image_path_to_load.c_str());

			if (!file_exists(new_dir) && !is_directory(new_dir))
			{
				// we have to check this, before we call the constructor

				// create new background image
				BackgroundImage_shptr new_bg_image(new BackgroundImage(prj->get_width(),
				                                                       prj->get_height(),
				                                                       new_dir));

				// load old single file image
				PersistentImage_RGBA_shptr old_bg_image =
					load_degate_image<PersistentImage_RGBA>(prj->get_width(),
					                                        prj->get_height(),
					                                        image_path_to_load);

				if (new_bg_image == NULL)
					throw DegateRuntimeException("Failed to load the background image");

				// convert image into new format

				debug(TM, "Copy the image into a new format. The data is stored in directory %s",
				      new_dir.c_str());

				copy_image(new_bg_image, old_bg_image);

				layer->set_image(new_bg_image);
			}
			else
			{
				debug(TM,
				      "There is already a directory named %s. It should be loaded as an image now.",
				      new_dir.c_str());

				BackgroundImage_shptr new_bg_image(new BackgroundImage(prj->get_width(),
				                                                       prj->get_height(),
				                                                       new_dir));

				layer->set_image(new_bg_image);
			}
		}
	}
	else
	{
		debug(TM, "project in %s has no layer image defined for a layer.",
		      prj->get_project_directory().c_str());
	}
}


void ProjectImporter::parse_port_colors_element(QDomElement const port_colors_elem, Project_shptr prj)
{
	const QDomNodeList color_list = port_colors_elem.elementsByTagName("port-color");

	PortColorManager_shptr port_color_manager = prj->get_port_color_manager();

	QDomElement color_elem;
	for (int i = 0; i < color_list.count(); i++)
	{
		color_elem = color_list.at(i).toElement();
		if (!color_elem.isNull())
		{
			const std::string port_name(color_elem.attribute("port-name").toStdString());
			const std::string fill_color_str(color_elem.attribute("fill-color").toStdString());
			const std::string frame_color_str(color_elem.attribute("frame-color").toStdString());

			port_color_manager->set_color(port_name,
			                              parse_color_string(frame_color_str),
			                              parse_color_string(fill_color_str));
		}
	}
}

void ProjectImporter::parse_colors_element(QDomElement const port_colors_elem,
                                           Project_shptr prj)
{
	const QDomNodeList color_list = port_colors_elem.elementsByTagName("color");

	QDomElement color_elem;
	for (int i = 0; i < color_list.count(); i++)
	{
		color_elem = color_list.at(i).toElement();
		if (!color_elem.isNull())
		{
			const std::string object_name(color_elem.attribute("object").toStdString());
			const std::string color_str(color_elem.attribute("color").toStdString());
			ENTITY_COLOR o;

			if (!object_name.compare("wire")) o = DEFAULT_COLOR_WIRE;
			else if (!object_name.compare("via-up")) o = DEFAULT_COLOR_VIA_UP;
			else if (!object_name.compare("via-down")) o = DEFAULT_COLOR_VIA_DOWN;
			else if (!object_name.compare("grid")) o = DEFAULT_COLOR_GRID;
			else if (!object_name.compare("annotation")) o = DEFAULT_COLOR_ANNOTATION;
			else if (!object_name.compare("annotation-frame")) o = DEFAULT_COLOR_ANNOTATION_FRAME;
			else if (!object_name.compare("gate")) o = DEFAULT_COLOR_GATE;
			else if (!object_name.compare("gate-frame")) o = DEFAULT_COLOR_GATE_FRAME;
			else if (!object_name.compare("gate-port")) o = DEFAULT_COLOR_GATE_PORT;
			else if (!object_name.compare("text")) o = DEFAULT_COLOR_TEXT;
			else if (!object_name.compare("emarker")) o = DEFAULT_COLOR_EMARKER;
			else
			{
				boost::format f("Can't parse object type. '%1%'");
				f % object_name;
				throw XMLAttributeParseException(f.str());
			}
			prj->set_default_color(o, parse_color_string(color_str));
		}
	}
}

void ProjectImporter::parse_grids_element(QDomElement const grids_elem, Project_shptr prj)
{
	const QDomNodeList regular_grid_list = grids_elem.elementsByTagName("regular-grid");
	const QDomNodeList irregular_grid_list = grids_elem.elementsByTagName("irregular-grid");

	QDomElement regular_grid_elem;
	for (int i = 0; i < regular_grid_list.count(); i++)
	{
		regular_grid_elem = regular_grid_list.at(i).toElement();
		if (!regular_grid_elem.isNull())
		{
			const std::string orientation(regular_grid_elem.attribute("orientation").toStdString());

			RegularGrid_shptr reg_grid = (orientation == "horizontal")
				                             ? prj->get_regular_horizontal_grid()
				                             : prj->get_regular_vertical_grid();

			reg_grid->set_distance(parse_number<unsigned int>(regular_grid_elem, "distance", 0));
			reg_grid->set_enabled(parse_bool(regular_grid_elem.attribute("enabled").toStdString()));
		}
	}

	QDomElement irregular_grid_elem;
	for (int j = 0; j < irregular_grid_list.count(); j++)
	{
		irregular_grid_elem = irregular_grid_list.at(j).toElement();
		if (!irregular_grid_elem.isNull())
		{
			const std::string orientation(irregular_grid_elem.attribute("orientation").toStdString());

			IrregularGrid_shptr irreg_grid = (orientation == "horizontal")
				                                 ? prj->get_irregular_horizontal_grid()
				                                 : prj->get_irregular_vertical_grid();

			irreg_grid->set_enabled(parse_bool(irregular_grid_elem.attribute("enabled").toStdString()));

			const QDomNodeList offsets_entry_list = irregular_grid_elem.elementsByTagName("offsets");

			if (!offsets_entry_list.isEmpty())
			{
				const QDomElement offsets_elem = offsets_entry_list.at(0).toElement();

				if (!offsets_elem.isNull())
				{
					const QDomNodeList offset_entry_list = offsets_elem.elementsByTagName("offset-entry");

					for (int k = 0; k < offset_entry_list.count(); k++)
					{
						QDomElement offset_entry_elem = offset_entry_list.at(k).toElement();
						if (!offset_entry_elem.isNull())
						{
							irreg_grid->add_offset(parse_number<int>(offset_entry_elem, "offset"));
						}
					}
				}
			}
		}
	}
}


void ProjectImporter::parse_project_element(Project_shptr parent_prj,
                                            QDomElement const project_elem)
{
	int w = parent_prj->get_width();
	int h = parent_prj->get_height();

	// Use geometry information to set up regular grid ranges.
	// The RegularGrid implementation might be changed in order to avoid this setup.
	RegularGrid_shptr reg_vert_grid = parent_prj->get_regular_vertical_grid();
	RegularGrid_shptr reg_hor_grid = parent_prj->get_regular_horizontal_grid();
	reg_vert_grid->set_range(0, w);
	reg_hor_grid->set_range(0, h);

	parent_prj->set_degate_version(project_elem.attribute("degate-version").toStdString());
	parent_prj->set_name(project_elem.attribute("name").toStdString());
	parent_prj->set_description(project_elem.attribute("description").toStdString());

	if (project_elem.attribute("server-url") != 0)
		parent_prj->set_server_url(project_elem.attribute("server-url").toStdString());

	if (project_elem.attribute("last-pulled-transaction-id") != 0)
		parent_prj->set_last_pulled_tid(parse_number<transaction_id_t>(project_elem,
		                                                               "last-pulled-transaction-id"));

	parent_prj->set_lambda(parse_number<length_t>(project_elem, "lambda"));
	parent_prj->set_default_pin_diameter(parse_number<diameter_t>(project_elem, "pin-diameter"));
	parent_prj->set_default_wire_diameter(parse_number<diameter_t>(project_elem, "wire-diameter"));
	parent_prj->set_default_port_diameter(parse_number<diameter_t>(project_elem, "port-diameter", 5));

	parent_prj->set_pixel_per_um(parse_number<double>(project_elem, "pixel-per-um", 0));
	parent_prj->set_template_dimension(parse_number<int>(project_elem, "template-dimension", 0));
	parent_prj->set_font_size(parse_number<unsigned int>(project_elem, "font-size", 10));

	QDomElement e = get_dom_twig(project_elem, "grids");
	if (!e.isNull()) parse_grids_element(e, parent_prj);

	e = get_dom_twig(project_elem, "layers");
	if (!e.isNull()) parse_layers_element(e, parent_prj);

	e = get_dom_twig(project_elem, "port-colors");
	if (!e.isNull()) parse_port_colors_element(e, parent_prj);

	e = get_dom_twig(project_elem, "default-colors");
	if (!e.isNull()) parse_colors_element(e, parent_prj);
}
