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

#include "DOTExporter.h"

#include <iostream>
#include <fstream>

using namespace degate;

void DOTExporter::add_header_line(std::string header_line) {
  header_lines.push_back(std::string("# ") + header_line);
}

void DOTExporter::add_graph_setting(std::string line) {
  graph_setting_lines.push_back(line);
}


void DOTExporter::add_node(std::string node_id, std::string node_params) {
  node_lines.push_back(node_id + node_params);
}


void DOTExporter::add_edge(std::string from_node_id, 
			   std::string to_node_id, 
			   std::string edge_params) {
  std::string txt(from_node_id);
  txt += std::string(" -- ");
  txt += to_node_id;
  txt += edge_params;
  edge_lines.push_back(txt);
}


void DOTExporter::dump_to_file(std::string const& filename) const {

  std::ofstream dot_file;

  dot_file.open(filename.c_str(), std::ios::trunc | std:: ios::out);

  // write header

  for(std::list<std::string>::const_iterator iter = header_lines.begin();
      iter != header_lines.end(); ++iter) {
    dot_file << *iter << std::endl;
  }
  
  dot_file << "graph LogicModel {" << std::endl;

  for(std::list<std::string>::const_iterator iter = graph_setting_lines.begin();
      iter != graph_setting_lines.end(); ++iter) {
    dot_file << "\t" << *iter << std::endl;
  }

  
  // nodes
  for(std::list<std::string>::const_iterator iter = node_lines.begin();
      iter != node_lines.end(); ++iter)
    dot_file << "\t" << *iter << std::endl;
  
  // edges
  for(std::list<std::string>::const_iterator iter = edge_lines.begin();
      iter != edge_lines.end(); ++iter)
    dot_file << "\t" << *iter << std::endl;
  
  
  dot_file << "}" << std::endl; 
  
  dot_file.close();
}

void DOTExporter::clear() {
  header_lines.clear();
  graph_setting_lines.clear();
  node_lines.clear();
  edge_lines.clear();
}
