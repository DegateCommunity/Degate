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

#ifndef __DOTEXPORTER_H__
#define __DOTEXPORTER_H__

#include "globals.h"
#include "Exporter.h"
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <fstream>

namespace degate {


  /**
   * Base class for a .dot exporter.
   *
   * The dot language is a graph description language.
   *
   * @see http://en.wikipedia.org/wiki/DOT_language
   *
   */
  class DOTExporter : public Exporter {

  private:

    std::list<std::string> header_lines;
    std::list<std::string> graph_setting_lines;
    std::list<std::string> node_lines;
    std::list<std::string> edge_lines;

  protected:

    /**
     * Add a header line.
     *
     * Header lines begin with a hash mark ('#'). It is a comment
     * in the dot language. Header lines should be used to write
     * comments to the top of a dot file.
     * @param header_line A single comment line. You don't have to put the
     *     hash mark in front of the string. If you pass a multi line string,
     *     the hash marks for the additional lines are not added automatically.
     */

    void add_header_line(std::string header_line);

    /**
     * Add a setting for a graph
     */
    void add_graph_setting(std::string line);

    /**
     * Add a node into the graph.
     */

    void add_node(std::string node_id, std::string node_params);

    /**
     * Add an edge into the graph.
     */

    void add_edge(std::string from_node_id, 
		  std::string to_node_id, 
		  std::string edge_params);

    /**
     * Dump the content as a string into a file.
     * If the file already exists, it will be overwritten.
     */

    void dump_to_file(std::string const& filename) const;

    /**
     * Clear any internally stored data.
     */

    void clear();

  public:

    /**
     * Create a DOTExporter object.
     */
    DOTExporter() {};

    /**
     * Destroy the DOTExporter object.
     */
    virtual ~DOTExporter() {};

  };

}
#endif
