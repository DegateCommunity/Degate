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

#include "XMLImporter.h"
#include "Core/Image/Image.h"

#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h> : Linux only
#include <cerrno>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

using namespace degate;

QDomElement XMLImporter::get_dom_twig(QDomElement const start_node, std::string const& element_name) const
{
    return start_node.elementsByTagName(QString::fromStdString(element_name)).at(0).toElement();
}


color_t XMLImporter::parse_color_string(std::string const& color_string) const
{
    const unsigned int correct_length = 1 + 4 * 2;
    if (color_string.size() != correct_length) return 0;

    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    std::istringstream iss_r(color_string.substr(1, 2));
    std::istringstream iss_g(color_string.substr(3, 2));
    std::istringstream iss_b(color_string.substr(5, 2));
    std::istringstream iss_a(color_string.substr(7, 2));

    iss_r >> std::hex >> r;
    iss_g >> std::hex >> g;
    iss_b >> std::hex >> b;
    iss_a >> std::hex >> a;

    return MERGE_CHANNELS(r, g, b, a);
}
