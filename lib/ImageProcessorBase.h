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

#ifndef __IMAGEPROCESSORBASE_H__
#define __IMAGEPROCESSORBASE_H__

#include <string>
#include <ProgressControl.h>

namespace degate {

  /**
   * Abstract base class for an image processor.
   */


  class ImageProcessorBase : public ProgressControl {

    friend class IPPipe;

  private:

    const std::string name;
    const std::string description;

    const bool has_properties;

    std::string type_in;
    std::string type_out;

  protected:


    std::string const& get_type_in() const {
      return type_in;
    }

    std::string const& get_type_out() const {
      return type_out;
    }


  public:

    /**
     * The constructor for a plugin
     * @param _name The name of the plugin.
     */

    ImageProcessorBase(std::string const& _name,
		       std::string const& _description,
		       bool _has_properties,
		       std::type_info const& _type_in,
		       std::type_info const& _type_out) :
      name(_name),
      description(_description),
      has_properties(_has_properties),
      type_in(_type_in.name()),
      type_out(_type_out.name()) {}

    /**
     * The destructor for a plugin.
     */

    virtual ~ImageProcessorBase() {}


    /**
     * Get the processor's name.
     */

    std::string const& get_name() const {
      return name;
    }

    /**
     * Get the processor's description.
     */

    std::string const& get_description() const {
      return description;
    }

    /**
     * Start processing.
     */
    virtual ImageBase_shptr run(ImageBase_shptr _in) = 0;


    /**
     * Check if the processor can be configured.
     */
    virtual bool has_property() const {
      return has_properties;
    }

  };

  typedef std::shared_ptr<ImageProcessorBase> ImageProcessorBase_shptr;
}

#endif

