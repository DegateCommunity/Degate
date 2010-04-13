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

#ifndef __GATETEMPLATE_H__
#define __GATETEMPLATE_H__

#include <degate.h>

#include <Layer.h>
#include <Image.h>
#include <GateTemplatePort.h>

#include <set>
#include <tr1/memory>
#include <map>

namespace degate {

  /**
   * A gate template is a container for common properties, that physically placed gates of that type share.
   */
  
  class GateTemplate : public LogicModelObjectBase, public ColoredObject {
    
    friend class Gate;

  public:

    /**
     * Implementation types for a template.
     */
    enum IMPLEMENTATION_TYPE {
      UNDEFINED = 0,
      TEXT = 1,
      VHDL = 2,
      VHDL_TESTBENCH = 3,
      VERILOG = 4,
      VERILOG_TESTBENCH = 5
    };

    typedef std::map<IMPLEMENTATION_TYPE, std::string /* code */> implementation_collection;
    typedef implementation_collection::iterator implementation_iter;

    typedef std::map<Layer::LAYER_TYPE, GateTemplateImage_shptr> image_collection;
    typedef image_collection::iterator image_iterator;

  private:

    BoundingBox bounding_box;
    unsigned int reference_counter;
    
    std::set<GateTemplatePort_shptr> ports;
    
    implementation_collection implementations;
    image_collection images;

    std::string logic_class; // e.g. nand, xor, flipflop, buffer, oai

  protected:
    
    /**
     * Increment the reference counter.
     */
    
    virtual void increment_reference_counter();
    
    /** 
     * Decrement the reference counter.
     */
    
    virtual void decrement_reference_counter();


  public:
    
    typedef std::set< GateTemplatePort_shptr >::iterator port_iterator;

    /**
     * The constructor to set up a new gate template.
     * @deprecated A gate template should not rely on positions. Instead it should
     *   have a the region as a copy.
     */

    GateTemplate(unsigned int width, unsigned int height);

    
    /**
     * The constructor to set up a new gate template.
     * @deprecated A gate template should not rely on positions. Instead it should
     *   have a the region as a copy.
     */

    GateTemplate(int _min_x, int _max_x, int _min_y, int _max_y);

    /**
     * Default constructor.
     */

    GateTemplate();
    
    /**
     * The destructor.
     */

    virtual ~GateTemplate();
    
    /**
     * Get the width of a gate template.
     */

    virtual unsigned int get_width() const;

    /**
     * Get the height of a gate template.
     */
    
    virtual unsigned int get_height() const;


    /**
     * Set the width of a gate template.
     * It does not adjust dimensions of gates that reference this template.
     */

    virtual void set_width(unsigned int width);

    /**
     * Set the height of a gate template.
     * It does not adjust dimensions of gates that reference this template.
     */
    
    virtual void set_height(unsigned int);

    /**
     * Get the bounding box of the template.
     * @deprecated
     */

    virtual BoundingBox const & get_bounding_box() const;

    /**
     * Set a reference image for the template.
     * You can store reference images for different layers, that is for
     * the transistor layer, for the logic layer and for a metal layer.
     * The images must have equal image dimensions. It is not checked here.
     *
     * @todo: Here we might need a hook for storing different image types,
     *   e.g. a template image that is an average image or sth. like that.
     */

    virtual void set_image(Layer::LAYER_TYPE layer_type, GateTemplateImage_shptr img)
      throw(InvalidPointerException);

    /**
     * Get a reference image for the template.
     * @see set_image()
     */

    virtual GateTemplateImage_shptr get_image(Layer::LAYER_TYPE layer_type) 
      throw(CollectionLookupException);


    /**
     * Check if there is a reference image for a layer type.
     */

    virtual bool has_image(Layer::LAYER_TYPE layer_type) const;

    /**
     * Add a template port to a gate template.
     * This is an isolated function. The port is just added to the gate template.
     * Nothing else. Adding a port to a template requires some updates in the logic
     * model. Therefore you should prefer the corresponding method from the LogicModel
     * class.
     * @exception InvalidObjectIDException This exception is thrown if the template port
     *   has no valid object ID.
     * @todo In order to make the API hard to misuse, this method might be made private.
     * @see LogicModel::add_template_port
     */
    
    virtual void add_template_port(GateTemplatePort_shptr template_port) 
      throw(InvalidObjectIDException);
    
    /**
     * Remove a port from a gate template.
     * This is an isolated function. The port is just removed from the gate template.
     * Nothing else. Removing a port from a template requires some updates in the logic
     * model. Therefore you should prefer the corresponding method from the LogicModel
     * class.
     * @todo In order to make the API hard to misuse, this method might be made private.
     * @see LogicModel::remove_template_port
     */

    virtual bool remove_template_port(GateTemplatePort_shptr template_port);
    
    /**
     * Remove a port from a gate template.
     * @exception InvalidObjectIDException This exception is thrown if the template port
     *   has no valid object ID.
     * @see remove_template_port(GateTemplatePort_shptr template_port)
     */
    
    virtual bool remove_template_port(object_id_t object_id) throw(InvalidObjectIDException);
    
    /**
     * Get a template port.
     * @exception InvalidObjectIDException This exception is thrown if the
     *   object ID is invalid.
     * @exception CollectionLookupException This exception is thrown if a
     *   template port with object ID \p object_id was not found.
     */
    
    virtual GateTemplatePort_shptr get_template_port(object_id_t object_id) 
      throw(CollectionLookupException, InvalidObjectIDException);


    /**
     * Check if a template has a specific template port.
     * @exception InvalidObjectIDException This exception is thrown if the
     *   object ID is invalid.
     */

    virtual bool has_template_port(object_id_t object_id) 
      const throw(InvalidObjectIDException);
    
    /**
     * Get an iterator.
     */

    virtual port_iterator ports_begin();

    
    /**
     * Get end marker for the iteration over ports.
     */

    virtual port_iterator ports_end();
    

    /**
     * Get an iterator to iterate over images.
     */

    virtual image_iterator images_begin();

    /**
     * Get end marker for the iteration over images.
     */

    virtual image_iterator images_end();

    /**
     * Get the reference counter.
     * @return Returns how many gates reference this gate template.
     */

    virtual unsigned int get_reference_counter() const;


    /**
     * Get an iterator to iterate over implementations.
     */

    virtual implementation_iter implementations_begin();

    /**
     * Get an end marker for the iteration.
     */
    
    virtual implementation_iter implementations_end();


    /**
     * Set VHDL/Verilog implementation for the gate template.
     * @param impl_type Set VHDL or Verilog.
     * @param code The implementation.
     */

    virtual void set_implementation(IMPLEMENTATION_TYPE impl_type, std::string const& code);

    /**
     * Get code for an implementation type.
     */

    std::string get_implementation(IMPLEMENTATION_TYPE impl_type) const throw(CollectionLookupException);

    /**
     * Print gate template's meta information to an output stream.
     */

    virtual void print(std::ostream & os);


    /**
     * Get number of defined ports.
     */

    virtual unsigned int get_number_of_ports() const;


    /**
     * Set logic class for a standard cell.
     *
     * There are two reasons for having this kind of tagging. First we
     * want to render dedicated electronic symbols for standard gates, e.g.
     * nands, xors and flipflops, independed of the standard cells name.
     * Second we want to search for common building blocks, e.g.
     * linear feedback shift registers, that we basically describe as
     * a set of connected flipflops with some xor gates between them,
     * independend of the gate's naming.
     */

    virtual void set_logic_class(std::string const& logic_class);

    /**
     * Get logic class for a standard cell.
     */

    virtual std::string get_logic_class() const;


    static std::string get_impl_type_as_string(IMPLEMENTATION_TYPE impl_type);
    static IMPLEMENTATION_TYPE get_impl_type_from_string(std::string const& impl_type_str) 
      throw(DegateRuntimeException);

  };

  

}

#endif
