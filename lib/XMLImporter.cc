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

#include "XMLImporter.h"
#include "Image.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

using namespace degate;

const xmlpp::Element * XMLImporter::get_dom_twig(const xmlpp::Element * const start_node, std::string const & element_name) const {
  xmlpp::Node::NodeList node_list = start_node->get_children(element_name);
  if(!node_list.empty()) {
    const xmlpp::Element * element = dynamic_cast<const xmlpp::Element*>(node_list.front());
    return element;
  }
  return NULL;
}


color_t XMLImporter::parse_color_string(std::string const& color_string) const {
  const unsigned int correct_length = 1 + 4 * 2;
  if(color_string.size() != correct_length) return 0;

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

  return MERGE_CHANNELS(r,g,b,a);
}
