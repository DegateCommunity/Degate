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

#ifndef __IPPIPE_H__
#define __IPPIPE_H__

#include <string>
#include <ImageProcessorBase.h>
#include <ProgressControl.h>

namespace degate {
  
  /**
   * Represents an image processing pipe for multiple image processors.
   */


  class IPPipe : public ProgressControl {
    
  private:

    typedef std::list<std::tr1::shared_ptr<ImageProcessorBase> > processor_list_type;
    processor_list_type processor_list;

  public:

    /**
     * The constructor for a processing pipe.
     */

    IPPipe() {
    }

    /**
     * The destructor for a plugin.
     */

    virtual ~IPPipe() {}

    
    /**
     * Add a processor.
     */

    void add(std::tr1::shared_ptr<ImageProcessorBase> processor) {
      processor_list.push_back(processor);
    }

    /**
     * Check if the pipe is empty
     * @see size()
     */

    bool is_empty() const {
      return processor_list.empty();
    }

    /**
     * Get the number of processing elements in the pipe.
     * @see empty()
     */

    size_t size() const {
      return processor_list.size();
    }


    

    /**
     * Start processing.
     */
    ImageBase_shptr run(ImageBase_shptr img_in) {

      assert(img_in != NULL);

      ImageBase_shptr last_img = img_in;

      // iterate over list
      for(processor_list_type::iterator iter = processor_list.begin();
	  iter != processor_list.end(); ++iter) {

	ImageProcessorBase_shptr ip = *iter;

	assert(last_img != NULL);
	last_img = ip->run(last_img);
	assert(last_img != NULL);
      }
      
      return last_img;
    }



  };

}

#endif

