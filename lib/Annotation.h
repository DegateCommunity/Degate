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

#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

#include "globals.h"
#include "LogicModelObjectBase.h"
#include "Layer.h"
#include "PlacedLogicModelObject.h"

#include "Rectangle.h"

#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

namespace degate {

  /**
   * An annotation is a descriptive meta object that can be
   * placed on a logic model's layer to mark a region of interest.
   *
   * The semantics of an annotation is user defined. The libdegate
   * does not establish a relationship from an annotaion to another
   * logic model object or to an background image region.
   *
   * Each annotation should have a class ID. This might be used
   * to classify the kind of annotation. For example you can place
   * an annotation 'above' a distored part of the background image to
   * remember yourself, that this image part should be rephotographed.
   * An image recognition algorithm may auto-annotate, that it is unsure
   * e.g. if there is a via or not due to fuzzy thresholds.
   *
   * You can set a name and a description for the annotation as well.
   *
   * This class is designed to be derived for concrete annotations. 
   *
   * @see set_name()
   * @see set_description()
   */
  
  class Annotation : public Rectangle, public PlacedLogicModelObject {
    
  public:

    typedef unsigned int class_id_t;

    /**
     * Enums to declare the type of annotation.
     */
    
    enum ANNOTATION_TYPE {
      UNDEFINED = 0,
      SUBPROJECT = 1
    };
    
    typedef std::map<std::string, /* param name */
		     std::string  /* param value */ > parameter_set_type;

  private:

    class_id_t class_id;
    parameter_set_type parameters;

  protected:


    /**
     * Set a parameter.
     */

    void set_parameter(std::string const& parameter_name, 
		       std::string const& parameter_value) {
      parameters[parameter_name] = parameter_value;
    }

  public:
    
    /**
     * Create a new annotation.
     */

    Annotation(int _min_x, int _max_x, int _min_y, int _max_y, 
	       class_id_t _class_id = UNDEFINED);
   
    
    /**
     * The destructor for an annotaion.
     */

    virtual ~Annotation();
    
    /**
     * Get the class ID for an annotation.
     */

    virtual class_id_t get_class_id() const;
    
    /**
     * Set the class ID for an annotation.
     */

    virtual void set_class_id(class_id_t _class_id);
    
    /**
     * Get a human readable string that describes the whole
     * logic model object. The string should be unique in order
     * to let the user identify the concrete object. But that
     * is not a must.
     */
    
    virtual const std::string get_descriptive_identifier() const;

    /**
     * Get a human readable string that names the object type.
     * Here it is "Annotation".
     */

    virtual const std::string get_object_type_name() const;

    /**
     * Print annotation.
     */
    void print(std::ostream & os = std::cout, int n_tabs = 0) const;



    void shift_x(int delta_x) {
      Rectangle::shift_x(delta_x);
      notify_shape_change();
    }

    void shift_y(int delta_y) {
      Rectangle::shift_y(delta_y);
      notify_shape_change();
    }

    virtual bool in_bounding_box(BoundingBox const& bbox) const {
      return in_bounding_box(bbox);
    }

    virtual BoundingBox const& get_bounding_box() const {
      return Rectangle::get_bounding_box();
    }

    virtual bool in_shape(int x, int y) const {
      return Rectangle::in_shape(x, y);
    }


    /**
     * Get a parameter value.
     *
     * @exception boost::bad_lexical_cast This exception is thrown if the parameter value
     *   cannot be converted to the desired type.
     * @exception CollectionLookupException This exception is thrown if the parameter is
     *   not stored in the lookup table.
     */

    template<typename NewType>
    NewType get_parameter(std::string parameter_name) 
      const throw(boost::bad_lexical_cast, CollectionLookupException) {

      parameter_set_type::const_iterator iter = parameters.find(parameter_name);
      if(iter == parameters.end()) {
	boost::format f("Failed to lookup parameter %1%.");
	f % parameter_name;
	throw CollectionLookupException(f.str());
      }

      if(typeid(NewType) == typeid(std::string) ||
	 typeid(NewType) == typeid(boost::filesystem::path)) {

	return NewType(iter->second);
      }

      try {
	return boost::lexical_cast<NewType>(iter->second);
      }
      catch(boost::bad_lexical_cast &) {
	debug(TM, "Failed to convert value string '%s'.", iter->second.c_str());
	throw;
      }
    }

    /**
     * Get an iterator to iterate over parameters.
     */
    parameter_set_type::const_iterator parameters_begin() const;

    /**
     * Get an end marker for the parameter iteration.
     */
    parameter_set_type::const_iterator parameters_end() const;
  };

}
#endif
