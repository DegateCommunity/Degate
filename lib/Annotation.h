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
#include "PlacedLogicModelObject.h"

#include <set>

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
   * @see set_name
   * @see set_description
   */
  
  class Annotation : public Rectangle, public PlacedLogicModelObject {
    
  public:
    typedef unsigned int class_id_t;
    
  private:
    class_id_t class_id;
    
  public:
    
    /**
     * Create a new annotation.
     */

    Annotation(int _min_x, int _max_x, int _min_y, int _max_y, 
	       class_id_t _class_id = 0);
   
    
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

  };

}
#endif
