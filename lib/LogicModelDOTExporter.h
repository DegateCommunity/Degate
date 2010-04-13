/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#ifndef __LOGICMODELDOTEXPORTER_H__
#define __LOGICMODELDOTEXPORTER_H__

#include "globals.h"
#include "LogicModel.h"
#include "DOTExporter.h"
#include "ObjectIDRewriter.h"
#include "Layer.h"

#include <stdexcept>

namespace degate {

  /**
   * The LogicModelDOTExporter exports the logic model or a part
   * of the logic model as a dot graph.
   */

  class LogicModelDOTExporter : public DOTExporter {

  public:

    /**
     * Properties you can set in order to control the dot output.
     */
    enum PROPERTY {

      /** default: false */
      PRESERVE_GATE_POSITIONS, 

      /** default: true */
      ENABLE_EDGES,

      /** default: true */
      ENABLE_VIAS,

      /** default: true */
      ENABLE_WIRES,

      /** default: true */
      ENABLE_TEMPLATE_NAMES,

      /** 
       * Control whether the fill color of logic model objects is used
       * as fill color for shapes in the dot output.
       * default: false 
      */
      ENABLE_COLORS
      
    };
    
  protected:
       
    void add_gate(Gate_shptr gate) throw(std::runtime_error);
    void add_via(Via_shptr via);
    //void add_wire(Wire_shptr wire);
    void add_net(Net_shptr lmodel) throw(std::runtime_error);
    std::string add_implicit_net(Net_shptr net);

    void add_connection(Net_shptr net, std::string const& src_name, std::string const& edge_name);

    std::string oid_to_str(std::string const& prefix, object_id_t oid);


  private:

    std::map<object_id_t /* net id */, int> implicit_net_counter;

    ObjectIDRewriter_shptr oid_rewriter;

    double scaling;

    typedef std::map<PROPERTY, bool> property_map;
    property_map properties;

    unsigned int fontsize, penwidth;

  public:

    LogicModelDOTExporter(ObjectIDRewriter_shptr _oid_rewriter) : 
      oid_rewriter(_oid_rewriter), scaling(1) {

      properties[PRESERVE_GATE_POSITIONS] = false;
      properties[ENABLE_EDGES] = true;
      properties[ENABLE_VIAS] = true;
      properties[ENABLE_WIRES] = true;
      properties[ENABLE_TEMPLATE_NAMES] = true;
      properties[ENABLE_COLORS] = false;

      fontsize = 0;
      penwidth = 0;
    }
    
    ~LogicModelDOTExporter() {}

    /**
     * Export the logic model as DOT file.
     */
    void export_data(std::string const& filename, LogicModel_shptr lmodel) 
      throw( InvalidPathException, InvalidPointerException, std::runtime_error );
    
    /**
     * Set a property for the dot export.
     */
    void set_property(PROPERTY property, bool state) {
      properties[property] = state;
    }

    /**
     * Get the state of a property.
     * @see set_property()
     */
    bool get_property(PROPERTY property) const { 

      property_map::const_iterator found = properties.find(property);

      /* We can assert this, because the property-map is
	 filled up in the constructor with default settings.
	 If we forget it we might notice it here. */
      assert(found != properties.end());
      
      return (*found).second;
    }


    /**
     * Set the font size.
     * @param size The new font size in points. If you set it
     *      to zero, the default font size is used.
     * @see http://www.graphviz.org/doc/info/attrs.html#d:fontsize
     */
    void set_fontsize(unsigned int size) { fontsize = size; }

    /**
     * Get the font size.
     * @see set_fontsize()
     */
    unsigned int get_fontsize() const { return fontsize; }

    /**
     * Set the pen width.
     * @param size The new pen width in points. If you set it
     *      to zero, the default pen width is used.
     * @see http://www.graphviz.org/doc/info/attrs.html#d:penwidth
     */
    void set_penwidth(unsigned int size) { penwidth = size; }

    /**
     * Get the pen width.
     * @see set_penwidth()
     */
    unsigned int get_penwidth() const { return penwidth; }

  };

}

#endif
