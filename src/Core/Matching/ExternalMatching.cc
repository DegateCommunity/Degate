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

#include "Core/Image/ImageHelper.h"
#include "Core/Matching/ExternalMatching.h"
#include "Core/Primitive/BoundingBox.h"
#include "Core/Utils/DegateHelper.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>


using namespace degate;

ExternalMatching::ExternalMatching()
{
}


void ExternalMatching::init(BoundingBox const& bounding_box, Project_shptr project)
{
    this->bounding_box = bounding_box;

    if (project == nullptr)
        throw InvalidPointerException("Invalid pointer for parameter project.");

    lmodel = project->get_logic_model();
    assert(lmodel != nullptr); // always has a logic model

    layer = lmodel->get_current_layer();
    if (layer == nullptr) throw DegateRuntimeException("No current layer in project.");


    ScalingManager_shptr sm = layer->get_scaling_manager();
    assert(sm != nullptr);

    img = sm->get_image(1).second;
    assert(img != nullptr);
}


void ExternalMatching::set_command(std::string const& cmd)
{
    this->cmd = cmd;
}

std::string ExternalMatching::get_command() const
{
    return cmd;
}

void ExternalMatching::run()
{
    // create a temp dir
    std::string dir = create_temp_directory();
    assert(is_directory(dir));

    std::string image_file = dir;
    image_file.append("/image.tiff");

    std::string results_file = dir;
    results_file.append("/results.dat");

    save_part_of_image(image_file, img, bounding_box);

    boost::format f("%1% --image %2% --results %3% "
        "--start-x %4% --start-y %5% --width %6% --height %7%");
    f % cmd
        % image_file
        % results_file
        % bounding_box.get_min_x()
        % bounding_box.get_min_y()
        % bounding_box.get_width()
        % bounding_box.get_height();


    debug(TM, "start external command: %s", f.str().c_str());
    exit_code = system(f.str().c_str());
    if (exit_code == -1)
    {
        debug(TM, "system() failed");
    }
    else
    {
        for (auto plo : parse_file(results_file))
        {
            lmodel->add_object(layer, plo);
        }
    }

    // cleanup
    remove_directory(dir);
}

std::list<PlacedLogicModelObject_shptr> ExternalMatching::parse_file(std::string const& filename) const
{
    std::list<PlacedLogicModelObject_shptr> list;
    std::string line;
    std::ifstream file(filename.c_str());

    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);

            PlacedLogicModelObject_shptr plo = parse_line(line);
            if (plo != nullptr) list.push_back(plo);
        }
        file.close();
    }
    return list;
}

PlacedLogicModelObject_shptr ExternalMatching::parse_line(std::string const& line) const
{
    std::vector<std::string> tokens = tokenize(line);

    PlacedLogicModelObject_shptr plo;

    if (tokens.size() == 0) return plo;
    else if (tokens[0].at(0) == '#') return plo;
    else if (tokens[0] == "wire" &&
        tokens.size() >= 6
    )
    {
        int
            x1 = boost::lexical_cast<int>(tokens[1]),
            y1 = boost::lexical_cast<int>(tokens[2]),
            x2 = boost::lexical_cast<int>(tokens[3]),
            y2 = boost::lexical_cast<int>(tokens[4]),
            diameter = boost::lexical_cast<unsigned int>(tokens[5]);

        return std::make_shared<Wire>(x1, y1, x2, y2, diameter);
    }
    else if (tokens[0] == "via" &&
        tokens.size() >= 6
    )
    {
        int
            x = boost::lexical_cast<int>(tokens[1]),
            y = boost::lexical_cast<int>(tokens[2]),
            diameter = boost::lexical_cast<unsigned int>(tokens[3]);

        Via::DIRECTION dir = tokens[4] == "up" ? Via::DIRECTION_UP : Via::DIRECTION_DOWN;

        return std::make_shared<Via>(x, y, diameter, dir);
    }

    else
    {
        std::string err("Can't parse line: ");
        throw DegateRuntimeException(err + line);
    }
}
