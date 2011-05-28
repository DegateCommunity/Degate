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

#ifndef __IMAGEWRITERBASE_H__
#define __IMAGEWRITERBASE_H__

#include <list>
#include <tr1/memory>
#include "degate_exceptions.h"

#include "TypeTraits.h"
#include "StoragePolicies.h"
#include "PixelPolicies.h"
#include "FileSystem.h"
#include <degate_exceptions.h>

namespace degate {

  /**
   * The base class for image readers.
   */

  template<class ImageType>
  class ImageWriterBase {
  private:

    std::string filename;
    unsigned int width, height;

  protected:

    /**
     * Set the width of the image.
     * This method should be called by derived image readers to set
     * the image size.
     */
    void set_width(unsigned int _width) { width = _width; }


    /**
     * Set the height of the image.
     * This method should be called by derived image readers to set
     * the image size.
     */
    void set_height(unsigned int _height) { height = _height; }


  public:

    /**
     * Constructor.
     */

    ImageWriterBase(unsigned int _width,
		    unsigned int _height,
		    std::string const & _filename) :
      filename(_filename),
      width(_width),
      height(_height) {}

    /**
     * The destructor.
     */

    virtual ~ImageWriterBase() {}

    /**
     * Get the filename.
     */

    std::string get_filename() const { return filename; }

    /**
     * Get the image width.
     * You have to call read() before.
     */

    unsigned int get_width() const { return width; }

    /**
     * Get the image height.
     * You have to call read() before.
     */

    unsigned int get_height() const { return height; }



    /**
     * Writer the image into a file.
     */

    virtual bool write_image(std::tr1::shared_ptr<ImageType> img) = 0;

  };



}

#endif
