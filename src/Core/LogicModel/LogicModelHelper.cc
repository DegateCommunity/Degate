/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Globals.h"
#include "Core/LogicModel/LogicModelHelper.h"
#include "Core/LogicModel/LogicModelObjectBase.h"
#include "Core/Utils/TangencyCheck.h"
#include "GUI/Preferences/PreferencesHandler.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/range/counting_range.hpp>

#include <QMutex>
#include <QtConcurrent/QtConcurrent>

using namespace degate;

Layer_shptr degate::get_first_layer(LogicModel_shptr lmodel, Layer::LAYER_TYPE layer_type)
{
    if (layer_type == Layer::UNDEFINED)
        throw DegateLogicException("Invalid layer type.");

    if (lmodel == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_first_layer()");

    for (LogicModel::layer_collection::iterator iter = lmodel->layers_begin();
         iter != lmodel->layers_end(); ++iter)
    {
        Layer_shptr layer = *iter;

        if (layer->is_enabled() && layer->get_layer_type() == layer_type)
            return layer;
    }

    boost::format fmter("Can't lookup layer of type %1% in logic model.");
    fmter % Layer::get_layer_type_as_string(layer_type);
    throw CollectionLookupException(fmter.str());
}

Layer_shptr degate::get_first_logic_layer(LogicModel_shptr lmodel)
{
    if (lmodel == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_first_logic_layer()");
    try
    {
        return get_first_layer(lmodel, Layer::LOGIC);
    }
    catch (CollectionLookupException const&)
    {
        throw;
    }
}


Gate_shptr degate::get_gate_by_name(LogicModel_shptr lmodel,
                                    std::string const& gate_name)
{
    for (LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
         iter != lmodel->gates_end(); ++iter)
    {
        Gate_shptr gate = (*iter).second;
        if (gate->get_name() == gate_name) return gate;
    }

    return Gate_shptr();
}

void degate::apply_colors_to_gate_ports(LogicModel_shptr lmodel,
                                        PortColorManager_shptr pcm)
{
    // iterate over gates
    for (LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
         iter != lmodel->gates_end(); ++iter)
    {
        Gate_shptr gate = (*iter).second;

        // iterate over gate ports
        for (Gate::port_iterator iter = gate->ports_begin();
             iter != gate->ports_end(); ++iter)
        {
            GatePort_shptr gate_port = *iter;

            if (gate_port->has_template_port())
            {
                GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
                std::string port_name = tmpl_port->get_name();
                if (pcm->has_color_definition(port_name))
                {
                    color_t fill_c = pcm->get_fill_color(port_name);
                    color_t frame_c = pcm->get_frame_color(port_name);

                    gate_port->set_fill_color(fill_c);
                    gate_port->set_frame_color(frame_c);
                }
            }
        }
    }
}


std::list<Layer_shptr> degate::get_available_standard_layers(LogicModel_shptr lmodel)
{
    std::list<Layer_shptr> layers;
    Layer_shptr l;

    try
    {
        l = get_first_layer(lmodel, Layer::TRANSISTOR);
        if (l != nullptr) layers.push_back(l);
    }
    catch (CollectionLookupException const& ex)
    {
        debug(TM, "Got an exception. A layer is not available. I will ignore it: %s", ex.what());
    }

    try
    {
        l = get_first_logic_layer(lmodel);
        if (l != nullptr) layers.push_back(l);
    }
    catch (CollectionLookupException const& ex)
    {
        debug(TM, "Got an exception. A layer is not available. I will ignore it: %s", ex.what());
    }

    try
    {
        l = get_first_layer(lmodel, Layer::METAL);
        if (l != nullptr) layers.push_back(l);
    }
    catch (CollectionLookupException const& ex)
    {
        debug(TM, "Got an exception. A layer is not available. I will ignore it: %s", ex.what());
    }

    return layers;
}


void degate::grab_template_images(LogicModel_shptr lmodel,
                                  GateTemplate_shptr gate_template,
                                  BoundingBox const& bounding_box,
                                  Gate::ORIENTATION orientation)
{
    std::list<Layer_shptr> layers = get_available_standard_layers(lmodel);

    for (std::list<Layer_shptr>::iterator iter = layers.begin();
         iter != layers.end(); ++iter)
    {
        Layer_shptr layer = *iter;
        assert(layer->get_layer_type() != Layer::UNDEFINED);
        debug(TM, "grab image from %s layer",
              layer->get_layer_type_as_string().c_str());

        // extract image

        GateTemplateImage_shptr tmpl_img =
            grab_image<GateTemplateImage>(lmodel, layer, bounding_box);
        assert(tmpl_img != nullptr);

        // flip
        switch (orientation)
        {
        case Gate::ORIENTATION_FLIPPED_UP_DOWN:
            flip_up_down<GateTemplateImage>(tmpl_img);
            break;
        case Gate::ORIENTATION_FLIPPED_LEFT_RIGHT:
            flip_left_right<GateTemplateImage>(tmpl_img);
            break;
        case Gate::ORIENTATION_FLIPPED_BOTH:
            flip_up_down<GateTemplateImage>(tmpl_img);
            flip_left_right<GateTemplateImage>(tmpl_img);
            break;
        default:
            // do nothing
            break;
        }

        // set as master image
        gate_template->set_image(layer->get_layer_type(), tmpl_img);
    }
}


void degate::load_background_image(Layer_shptr layer,
                                   std::string const& project_dir,
                                   std::string const& image_file)
{
    if (layer == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to load_background_image()");

    boost::format fmter("layer_%1%.dimg");
    fmter % layer->get_layer_id(); // was get_layer_pos()

    std::string dir(join_pathes(project_dir, fmter.str()));

    if (layer->has_background_image())
        layer->unset_image();

    debug(TM, "Create background image in %s", dir.c_str());
    BackgroundImage_shptr bg_image(new BackgroundImage(layer->get_width(),
                                                       layer->get_height(),
                                                       dir));

    debug(TM, "Load image %s", image_file.c_str());
    load_image<BackgroundImage>(image_file, bg_image);

    debug(TM, "Set image to layer.");
    layer->set_image(bg_image);
    debug(TM, "Done.");
}


void load_tile(const QRgb* rba_data,
               unsigned int tile_size,
               unsigned int tile_index,
               const std::string& path,
               QSize local_size,
               unsigned int global_tile_x,
               unsigned int global_tile_y,
               unsigned int tile_count_x)
{
    unsigned int local_tile_x = tile_index % tile_count_x;
    unsigned int local_tile_y = tile_index / tile_count_x;

    ////////////////
    unsigned int tile_x = global_tile_x + local_tile_x;
    unsigned int tile_y = global_tile_y + local_tile_y;

    // Create a file name from tile number.
    char filename[PATH_MAX];
    snprintf(filename, sizeof(filename), "%d_%d.dat", tile_x, tile_y);

    auto data = new BackgroundImage::pixel_type[tile_size * tile_size];
    memset(data,
           0,
           static_cast<std::size_t>(tile_size) *
           static_cast<std::size_t>(tile_size) *
           sizeof(BackgroundImage::pixel_type));

    unsigned int min_x = tile_size * local_tile_x;
    unsigned int min_y = tile_size * local_tile_y;

    unsigned int default_max_x = min_x + tile_size;
    unsigned int default_max_y = min_y + tile_size;

    unsigned int max_x = default_max_x > static_cast<unsigned int>(local_size.width()) ?
                         static_cast<unsigned int>(local_size.width()) : default_max_x;
    unsigned int max_y = default_max_y > static_cast<unsigned int>(local_size.height()) ?
                         static_cast<unsigned int>(local_size.height()) : default_max_y;

    // Fill image (can be long with big images).
    QRgb rgb;
    for (unsigned int y = min_y; y < max_y; y++)
    {
        for (unsigned int x = min_x; x < max_x; x++)
        {
            rgb = rba_data[y * local_size.width() + x];
            data[(y - min_y) * tile_size + (x - min_x)] = MERGE_CHANNELS(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));
        }
    }

    assert(!file_exists(path + "/" + filename));

    auto file = std::fstream(path + "/" + filename, std::ios::out | std::ios::binary);

    file.write(reinterpret_cast<const char*>(&data[0]),
               static_cast<std::size_t>(tile_size) *
               static_cast<std::size_t>(tile_size) *
               sizeof(BackgroundImage::pixel_type));

    file.close();

    delete[] data;
}


void create_scaled_background_image(const std::string& dir, const BackgroundImage_shptr& bg_image, QSize scaled_size, unsigned int tile_image_size, QImageReader& reader)
{
    QSize read_size{0, 0};

    // Start image conversion and loading
    while (true)
    {
        QSize reading_size{static_cast<int>(tile_image_size), static_cast<int>(tile_image_size)};

        // Check width
        if (reading_size.width() + read_size.width() > scaled_size.width())
            reading_size.setWidth(scaled_size.width() - read_size.width());

        // Check height
        if (reading_size.height() + read_size.height() > scaled_size.height())
            reading_size.setHeight(scaled_size.height() - read_size.height());

        reader.device()->seek(0);

        QImageReader current_reader(reader.device(), reader.format());

        QRect rect(read_size.width(), read_size.height(), reading_size.width(), reading_size.height());

        current_reader.setScaledSize(scaled_size);
        current_reader.setScaledClipRect(rect);

        QImage img = current_reader.read();
        if (img.isNull())
        {
            debug(TM, "can't create %s (can't read image)\n", bg_image->get_directory().c_str());
        }

        // Convert to good format
        if (img.format() != QImage::Format_ARGB32 && img.format() != QImage::Format_RGB32)
        {
            img = img.convertToFormat(QImage::Format_ARGB32);
        }

        //////////////////// Process start ///////////////////////////

        unsigned int global_tile_x = static_cast<unsigned int>(read_size.width()) >> bg_image->get_tile_width_exp();
        unsigned int global_tile_y = static_cast<unsigned int>(read_size.height()) >> bg_image->get_tile_width_exp();

        auto tile_count_x = static_cast<unsigned int>(std::ceil(static_cast<double>(reading_size.width()) / static_cast<double>(bg_image->get_tile_size())));
        auto tile_count_y = static_cast<unsigned int>(std::ceil(static_cast<double>(reading_size.height()) / static_cast<double>(bg_image->get_tile_size())));

        const auto *rgb_data = reinterpret_cast<const QRgb*>(&img.constBits()[0]);

        // Multi-threaded function
        std::function<void(const unsigned int& y)> function = [&rgb_data, &bg_image, &dir, &reading_size, &global_tile_x, &global_tile_y, &tile_count_x](const unsigned int& i)
        {
            load_tile(rgb_data, bg_image->get_tile_size(), i, dir, reading_size, global_tile_x, global_tile_y, tile_count_x);
        };

        // Start multithreading
        const auto& it = boost::counting_range<unsigned int>(0, tile_count_x * tile_count_y);
        QtConcurrent::blockingMap(it, function);

        /////////////////////////////////////////////////////////////

        read_size.setWidth(read_size.width() + reading_size.width());

        if (read_size.width() >= scaled_size.width())
        {
            read_size.setWidth(0);
            read_size.setHeight(read_size.height() + reading_size.height());
        }

        debug(TM, "New scaled image loading step.");

        if (read_size.height() >= scaled_size.height())
            break;
    }
}


void create_scaled_background_images(const BackgroundImage_shptr& bg_image, unsigned int tile_image_size, QImageReader& reader, QSize default_size)
{
    auto w = static_cast<unsigned int>(default_size.width());
    auto h = static_cast<unsigned int>(default_size.height());
    unsigned int min_size = bg_image->get_tile_size();

    for (int i = 2; ((h > min_size) || (w > min_size)) && (i < static_cast<int>(1u << 24u)); i *= 2) // max 24 scaling levels
    {
        w >>= 1u;
        h >>= 1u;

        // create a new image
        char dir_name[PATH_MAX];
        snprintf(dir_name, sizeof(dir_name), "scaling_%d.dimg", i);
        std::string dir_path = join_pathes(bg_image->get_directory(), std::string(dir_name));
        create_directory(dir_path);

        reader.device()->seek(0);

        create_scaled_background_image(dir_path, bg_image, QSize(static_cast<int>(w), static_cast<int>(h)), tile_image_size, reader);
    }
}


void degate::load_new_background_image(Layer_shptr layer, std::string const& project_dir, std::string const& image_file)
{
    if (layer == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to load_background_image()");

    // Loading cache size (in mb)
    static const unsigned int loading_cache_size = PREFERENCES_HANDLER.get_preferences().image_importer_cache_size;
    ///////

    // Maximum image tile size for reading, regarding the maximum allowed loading cache size.
    unsigned int tile_image_size = std::floor<unsigned int>((std::sqrt<unsigned int>(loading_cache_size) * std::sqrt<unsigned int>(1024 * 1024)) / sizeof(BackgroundImage::pixel_type));

    // Layer directory
    boost::format fmter("layer_%1%.dimg");
    fmter % layer->get_layer_id(); // was get_layer_pos()

    std::string dir(join_pathes(project_dir, fmter.str()));

    // Remove old background image
    if (layer->has_background_image())
        layer->unset_image();

    // Create background image
    debug(TM, "Create background image in %s", dir.c_str());
    BackgroundImage_shptr bg_image = std::make_shared<BackgroundImage>(static_cast<unsigned int>(layer->get_width()),
                                                                       static_cast<unsigned int>(layer->get_height()),
                                                                       dir);

    // Adjust image size to be a multiple of tile size.
    tile_image_size = (tile_image_size / bg_image->get_tile_size()) * bg_image->get_tile_size();

    debug(TM, "%d", tile_image_size);

    if (tile_image_size < bg_image->get_tile_size())
        tile_image_size = bg_image->get_tile_size();

    //////////////// Convert new image to Degate internal format.

    // Create reader
    QImageReader reader(image_file.c_str());

    // If the image is a multi-page/multi-res, we take the page with the biggest resolution.
    int best_image_number = -1;
    if (reader.imageCount() > 1)
    {
        QSize best_size{0, 0};
        for (int i = 0; i < reader.imageCount(); i++)
        {
            if (best_size.width() < reader.size().width() || best_size.height() < reader.size().height())
            {
                best_size = reader.size();
                best_image_number = reader.currentImageNumber();
            }

            reader.jumpToNextImage();
        }

        reader.jumpToImage(best_image_number);
    }

    // Size
    QSize size = reader.size();
    if (!size.isValid())
    {
        debug(TM, "can't read size of %s\n", image_file.c_str());
        return;
    }

    QSize read_size{0, 0};

    // Start image conversion and loading
    while (true)
    {
        QSize reading_size{static_cast<int>(tile_image_size), static_cast<int>(tile_image_size)};

        // Check width
        if (reading_size.width() + read_size.width() > size.width())
            reading_size.setWidth(size.width() - read_size.width());

        // Check height
        if (reading_size.height() + read_size.height() > size.height())
            reading_size.setHeight(size.height() - read_size.height());

        reader.device()->seek(0);

        QImageReader current_reader(reader.device(), reader.format());

        QRect rect(read_size.width(), read_size.height(), reading_size.width(), reading_size.height());

        current_reader.setClipRect(rect);
        QImage img = current_reader.read();
        if (img.isNull())
        {
            debug(TM, "can't read %s\n", image_file.c_str());
        }

        // Convert to good format
        if (img.format() != QImage::Format_ARGB32 && img.format() != QImage::Format_RGB32)
        {
            img = img.convertToFormat(QImage::Format_ARGB32);
        }

        //////////////////// Process start ///////////////////////////

        unsigned int global_tile_x = static_cast<unsigned int>(read_size.width()) >> bg_image->get_tile_width_exp();
        unsigned int global_tile_y = static_cast<unsigned int>(read_size.height()) >> bg_image->get_tile_width_exp();

        auto tile_count_x = static_cast<unsigned int>(std::ceil(static_cast<double>(reading_size.width()) / static_cast<double>(bg_image->get_tile_size())));
        auto tile_count_y = static_cast<unsigned int>(std::ceil(static_cast<double>(reading_size.height()) / static_cast<double>(bg_image->get_tile_size())));

        const auto *rgb_data = reinterpret_cast<const QRgb*>(&img.constBits()[0]);

        // Multi-threaded function
        std::function<void(const unsigned int& y)> function = [&rgb_data, &bg_image, &dir, &reading_size, &global_tile_x, &global_tile_y, &tile_count_x](const unsigned int& i)
        {
            load_tile(rgb_data, bg_image->get_tile_size(), i, dir, reading_size, global_tile_x, global_tile_y, tile_count_x);
        };

        // Start multithreading
        const auto& it = boost::counting_range<unsigned int>(0, tile_count_x * tile_count_y);
        QtConcurrent::blockingMap(it, function);

        /////////////////////////////////////////////////////////////

        read_size.setWidth(read_size.width() + reading_size.width());

        if (read_size.width() >= size.width())
        {
            read_size.setWidth(0);
            read_size.setHeight(read_size.height() + reading_size.height());
        }

        debug(TM, "New image loading step.");

        if (read_size.height() >= size.height())
            break;
    }

    ///////////////

    debug(TM, "Create scaled images.");
    create_scaled_background_images(bg_image, tile_image_size, reader, size);
    debug(TM, "Finished creating scaled images.");

    ///////////////

    debug(TM, "Set image to layer.");
    layer->set_image(bg_image);
    debug(TM, "Done.");
}


void degate::clear_logic_model(LogicModel_shptr lmodel, Layer_shptr layer)
{
    if (lmodel == nullptr || layer == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to clear_logc_model()");

    // iterate over all objects that are placed on a specific layer and remove them

    for (LogicModel::object_collection::iterator iter = lmodel->objects_begin();
         iter != lmodel->objects_end(); ++iter)
    {
        PlacedLogicModelObject_shptr lmo = (*iter).second;
        if (lmo->get_layer() == layer)
        {
            lmodel->remove_object(lmo);
        }
    }
}

Layer_shptr degate::get_first_enabled_layer(LogicModel_shptr lmodel)
{
    if (lmodel == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_first_enabled_layer()");

    for (LogicModel::layer_collection::iterator iter = lmodel->layers_begin();
         iter != lmodel->layers_end(); ++iter)
    {
        Layer_shptr layer = *iter;

        if (layer->is_enabled()) return layer;
    }
    throw InvalidPointerException("Error: all layers are disabled.");
}

Layer_shptr degate::get_next_enabled_layer(LogicModel_shptr lmodel)
{
    if (lmodel == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_next_enabled_layer()");

    Layer_shptr curr_layer = lmodel->get_current_layer();
    if (curr_layer == nullptr)
        throw DegateRuntimeException("Error: there is no current layer.");

    for (unsigned int l_pos = curr_layer->get_layer_pos() + 1;
         l_pos < lmodel->get_num_layers(); l_pos++)
    {
        Layer_shptr layer = lmodel->get_layer(l_pos);
        if (layer->is_enabled()) return layer;
    }

    return curr_layer;

    /*
    for (unsigned int l_pos = curr_layer->get_layer_pos() + 1;
        l_pos <= curr_layer->get_layer_pos() + lmodel->get_num_layers(); l_pos++) {
      Layer_shptr layer = lmodel->get_layer(l_pos % lmodel->get_num_layers());
      if (layer->is_enabled()) return layer;
    }

    throw InvalidPointerException("Error: all layers are disabled.");
    return Layer_shptr(); // to avoid compiler warning
    */
}


Layer_shptr degate::get_next_enabled_layer(LogicModel_shptr lmodel, Layer_shptr layer)
{
    if (lmodel == nullptr || layer == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_next_enabled_layer()");

    for (unsigned int l_pos = layer->get_layer_pos() + 1;
         l_pos < lmodel->get_num_layers(); l_pos++)
    {
        Layer_shptr l = lmodel->get_layer(l_pos);
        if (l->is_enabled()) return l;
    }

    return Layer_shptr();
}

Layer_shptr degate::get_prev_enabled_layer(LogicModel_shptr lmodel)
{
    if (lmodel == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_prev_enabled_layer()");

    Layer_shptr curr_layer = lmodel->get_current_layer();
    if (curr_layer == nullptr)
        throw DegateRuntimeException("Error: there is no current layer.");


    if (curr_layer->get_layer_pos() == 0) return curr_layer;

    for (int l_pos = curr_layer->get_layer_pos() - 1; l_pos >= 0; l_pos--)
    {
        Layer_shptr layer = lmodel->get_layer(l_pos);
        if (layer->is_enabled()) return layer;
    }
    return curr_layer;

    /*
    if (lmodel->get_num_layers() == 1) return curr_layer;
  
    for (unsigned int l_pos = curr_layer->get_layer_pos() + lmodel->get_num_layers() - 1;
        l_pos > 0; l_pos--) {
      Layer_shptr layer = lmodel->get_layer(l_pos % lmodel->get_num_layers());
      if (layer->is_enabled()) return layer;
    }
    throw InvalidPointerException("Error: all layers are disabled.");
    return Layer_shptr(); // to avoid compiler warning
    */
}

Layer_shptr degate::get_prev_enabled_layer(LogicModel_shptr lmodel, Layer_shptr layer)
{
    if (lmodel == nullptr || layer == nullptr)
        throw InvalidPointerException("Error: you passed an invalid pointer to get_prev_enabled_layer()");

    for (unsigned int l_pos = layer->get_layer_pos(); l_pos > 0; l_pos--)
    {
        Layer_shptr l = lmodel->get_layer(l_pos - 1);
        if (l->is_enabled()) return l;
    }
    return Layer_shptr();
}

Layer_shptr degate::get_current_layer(Project_shptr project)
{
    if (project == nullptr)
        throw InvalidPointerException("Invalid parameter for get_curent_layer()");

    LogicModel_shptr lmodel = project->get_logic_model();
    assert(lmodel != nullptr);
    return lmodel->get_current_layer();
}

bool degate::is_logic_class(Gate_shptr gate, std::string const& logic_class)
{
    if (gate == nullptr)
        throw InvalidPointerException("Invalid parameter for is_logic_class()");

    if (gate->has_template())
    {
        GateTemplate_shptr gate_tmpl = gate->get_gate_template();
        std::string const& lclass = gate_tmpl->get_logic_class();
        if (logic_class == lclass) return true;
        if (logic_class.size() < lclass.size())
        {
            if (lclass.substr(0, logic_class.size()) == logic_class) return true;
        }
    }

    return false;
}


GateTemplatePort::PORT_TYPE degate::get_port_type(GatePort_shptr gate_port)
{
    if (gate_port == nullptr)
        throw InvalidPointerException("Invalid parameter for get_port_type()");

    if (gate_port->has_template_port())
    {
        GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
        return tmpl_port->get_port_type();
    }

    return GateTemplatePort::PORT_TYPE_UNDEFINED;
}


std::string degate::get_template_port_name(GatePort_shptr gate_port)
{
    if (gate_port == nullptr)
        throw InvalidPointerException("Invalid parameter for get_template_port_name()");

    if (gate_port->has_template_port())
    {
        GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
        return tmpl_port->get_name();
    }

    return "";
}

void degate::apply_port_color_settings(LogicModel_shptr lmodel, PortColorManager_shptr pcm)
{
    if (lmodel == nullptr || pcm == nullptr)
        throw InvalidPointerException("Invalid parameter for apply_port_color_settings()");

    // iterate over gates

    for (LogicModel::gate_collection::iterator gate_iter = lmodel->gates_begin();
         gate_iter != lmodel->gates_end(); ++gate_iter)
    {
        Gate_shptr gate = gate_iter->second;

        // iterate over ports

        for (Gate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter)
        {
            GatePort_shptr port = *iter;

            if (port->has_template_port())
            {
                GateTemplatePort_shptr tmpl_port = port->get_template_port();

                std::string port_name = tmpl_port->get_name();
                if (pcm->has_color_definition(port_name))
                {
                    tmpl_port->set_frame_color(pcm->get_frame_color(port_name));
                    tmpl_port->set_fill_color(pcm->get_fill_color(port_name));
                }
            }
        }
    }
}


void degate::merge_gate_images(LogicModel_shptr lmodel,
                               Layer_shptr layer,
                               GateTemplate_shptr tmpl,
                               std::list<Gate_shptr> const& gates)
{
    if (gates.empty()) return;

    std::list<GateTemplateImage_shptr> images;

    BOOST_FOREACH(const Gate_shptr g, gates)
    {
        GateTemplateImage_shptr tmpl_img =
            grab_image<GateTemplateImage>(lmodel, layer, g->get_bounding_box());
        assert(tmpl_img != nullptr);

        // flip
        switch (g->get_orientation())
        {
        case Gate::ORIENTATION_FLIPPED_UP_DOWN:
            flip_up_down<GateTemplateImage>(tmpl_img);
            break;
        case Gate::ORIENTATION_FLIPPED_LEFT_RIGHT:
            flip_left_right<GateTemplateImage>(tmpl_img);
            break;
        case Gate::ORIENTATION_FLIPPED_BOTH:
            flip_up_down<GateTemplateImage>(tmpl_img);
            flip_left_right<GateTemplateImage>(tmpl_img);
            break;
        default:
            // do nothing
            break;
        }

        images.push_back(tmpl_img);
    }

    GateTemplateImage_shptr merged_img = merge_images<GateTemplateImage>(images);

    tmpl->set_image(layer->get_layer_type(), merged_img);
}

void degate::merge_gate_images(LogicModel_shptr lmodel,
                               ObjectSet gates)
{
    /*
     * Classify gates by their standard cell object ID.
     */
    typedef std::map<object_id_t, std::list<Gate_shptr>> gate_sets_type;
    gate_sets_type gate_sets;

    BOOST_FOREACH(PlacedLogicModelObject_shptr plo, gates)
    {
        if (Gate_shptr gate = std::dynamic_pointer_cast<Gate>(plo))
        {
            GateTemplate_shptr tmpl = gate->get_gate_template();
            if (tmpl) // ignore gates, that have no standard cell
                gate_sets[tmpl->get_object_id()].push_back(gate);
        }
    }

    /*
     * Iterate over layers.
     */

    BOOST_FOREACH(Layer_shptr layer, get_available_standard_layers(lmodel))
    {
        /*
         * Iterate over standard cell classes.
         */
        for (gate_sets_type::iterator iter = gate_sets.begin(); iter != gate_sets.end(); ++iter)
        {
            Gate_shptr g = iter->second.front();
            assert(g != nullptr);

            merge_gate_images(lmodel, layer, g->get_gate_template(), iter->second);
        }
    }
}


void degate::remove_entire_net(LogicModel_shptr lmodel, Net_shptr net)
{
    BOOST_FOREACH(object_id_t oid, *net)
    {
        PlacedLogicModelObject_shptr plo = lmodel->get_object(oid);
        assert(plo != nullptr);
        if (ConnectedLogicModelObject_shptr clmo =
            std::dynamic_pointer_cast<ConnectedLogicModelObject>(plo))
            clmo->remove_net();
    }

    lmodel->remove_net(net);
}

void degate::connect_objects(LogicModel_shptr lmodel,
                             ConnectedLogicModelObject_shptr o1,
                             ConnectedLogicModelObject_shptr o2)
{
    std::list<ConnectedLogicModelObject_shptr> v;
    v.push_back(o1);
    v.push_back(o2);
    connect_objects(lmodel, v.begin(), v.end());
}


void degate::autoconnect_objects(LogicModel_shptr lmodel, Layer_shptr layer,
                                 BoundingBox const& search_bbox)
{
    if (lmodel == nullptr || layer == nullptr)
        throw InvalidPointerException("You passed an invalid shared pointer.");

    // iterate over connectable objects
    for (Layer::qt_region_iterator iter = layer->region_begin(search_bbox);
         iter != layer->region_end(); ++iter)
    {
        ConnectedLogicModelObject_shptr clmo1;

        if ((clmo1 = std::dynamic_pointer_cast<ConnectedLogicModelObject>(*iter)) != nullptr)
        {
            BoundingBox const& bb = clmo1->get_bounding_box();

            /* Iterate over connectable objects in the region identified
           by bounding box bb.
            */
            for (Layer::qt_region_iterator siter = layer->region_begin(bb);
                 siter != layer->region_end(); ++siter)
            {
                ConnectedLogicModelObject_shptr clmo2;
                if ((clmo2 =
                    std::dynamic_pointer_cast<ConnectedLogicModelObject>(*siter)) != nullptr)
                {
                    if ((clmo1->get_net() == nullptr ||
                            clmo2->get_net() == nullptr ||
                            clmo1->get_net() != clmo2->get_net()) && // excludes identical objects, too
                        check_object_tangency(std::dynamic_pointer_cast<PlacedLogicModelObject>(clmo1),
                                              std::dynamic_pointer_cast<PlacedLogicModelObject>(clmo2)))

                        connect_objects(lmodel, clmo1, clmo2);
                }
            }
        }
    }
}

void autoconnect_interlayer_objects_via_via(LogicModel_shptr lmodel,
                                            Layer_shptr adjacent_layer,
                                            BoundingBox const& search_bbox,
                                            Via_shptr v1,
                                            Via::DIRECTION v1_dir_criteria,
                                            Via::DIRECTION v2_dir_criteria)
{
    Via_shptr v2;

    for (Layer::qt_region_iterator siter = adjacent_layer->region_begin(search_bbox);
         siter != adjacent_layer->region_end(); ++siter)
    {
        if ((v2 = std::dynamic_pointer_cast<Via>(*siter)) != nullptr)
        {
            if ((v1->get_net() == nullptr || v2->get_net() == nullptr ||
                    v1->get_net() != v2->get_net()) &&
                v1->get_direction() == v1_dir_criteria &&
                v2->get_direction() == v2_dir_criteria &&
                check_object_tangency(std::dynamic_pointer_cast<Circle>(v1),
                                      std::dynamic_pointer_cast<Circle>(v2)))
                connect_objects(lmodel,
                                std::dynamic_pointer_cast<ConnectedLogicModelObject>(v1),
                                std::dynamic_pointer_cast<ConnectedLogicModelObject>(v2));
        }
    }
}

void autoconnect_interlayer_objects_via_gport(LogicModel_shptr lmodel,
                                              Layer_shptr adjacent_layer,
                                              BoundingBox const& search_bbox,
                                              Via_shptr v1,
                                              Via::DIRECTION v1_dir_criteria)
{
    GatePort_shptr v2;

    for (Layer::qt_region_iterator siter = adjacent_layer->region_begin(search_bbox);
         siter != adjacent_layer->region_end(); ++siter)
    {
        if ((v2 = std::dynamic_pointer_cast<GatePort>(*siter)) != nullptr)
        {
            if ((v1->get_net() == nullptr || v2->get_net() == nullptr ||
                    v1->get_net() != v2->get_net()) &&
                v1->get_direction() == v1_dir_criteria &&
                check_object_tangency(std::dynamic_pointer_cast<Circle>(v1),
                                      std::dynamic_pointer_cast<Circle>(v2)))
                connect_objects(lmodel,
                                std::dynamic_pointer_cast<ConnectedLogicModelObject>(v1),
                                std::dynamic_pointer_cast<ConnectedLogicModelObject>(v2));
        }
    }
}

void degate::autoconnect_interlayer_objects(LogicModel_shptr lmodel,
                                            Layer_shptr layer,
                                            BoundingBox const& search_bbox)
{
    if (lmodel == nullptr || layer == nullptr)
        throw InvalidPointerException("You passed an invalid shared pointer.");

    Layer_shptr
        layer_above = get_next_enabled_layer(lmodel, layer),
        layer_below = get_prev_enabled_layer(lmodel, layer);

    Via_shptr v1;

    // iterate over objects
    for (Layer::qt_region_iterator iter = layer->region_begin(search_bbox);
         iter != layer->region_end(); ++iter)
    {
        if ((v1 = std::dynamic_pointer_cast<Via>(*iter)) != nullptr)
        {
            BoundingBox const& bb = v1->get_bounding_box();

            /* Iterate over vias one layer above and one layer below
           in the region identified by bounding box bb. */

            if (layer_above != nullptr)
                autoconnect_interlayer_objects_via_via(lmodel, layer_above, bb, v1,
                                                       Via::DIRECTION_UP, Via::DIRECTION_DOWN);

            if (layer_below != nullptr)
            {
                autoconnect_interlayer_objects_via_via(lmodel, layer_below, bb, v1,
                                                       Via::DIRECTION_DOWN, Via::DIRECTION_UP);
                autoconnect_interlayer_objects_via_gport(lmodel, layer_below, bb, v1,
                                                         Via::DIRECTION_DOWN);
            }
        }
    }
}

void degate::update_port_diameters(LogicModel_shptr lmodel, diameter_t new_size)
{
    // iterate over gates
    for (LogicModel::gate_collection::iterator iter = lmodel->gates_begin();
         iter != lmodel->gates_end(); ++iter)
    {
        Gate_shptr gate = (*iter).second;
        assert(gate != nullptr);

        // iterate over gate ports
        for (Gate::port_iterator iter = gate->ports_begin();
             iter != gate->ports_end(); ++iter)
        {
            GatePort_shptr gate_port = *iter;
            if (gate_port->get_diameter() != new_size)
            {
                gate_port->set_diameter(new_size);
            }
        }
    }
}
