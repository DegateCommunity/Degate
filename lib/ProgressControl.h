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

#ifndef __PROGRESSCONTROL_H__
#define __PROGRESSCONTROL_H__

namespace degate {
  
  /**
   */

  class ProgressControl {
    
  private:

    double progress;

  protected:

    /**
     * Set progress.
     */
    void set_progress(double progress) {
      this->progress = progress;
    }

  public:

    /**
     * The constructor
     */

    ProgressControl() : progress(0) {}

    /**
     * The destructor for a plugin.
     */

    virtual ~ProgressControl() {}

    /**
     * Start the processing.
     */
    
    //virtual void run() 

    /**
     * Stop the processing.
     */

    //virtual void halt() = 0;


    /**
     * Get progress.
     * @return Returns a value between 0 and 100 percent.
     */

    double get_progress() const {
      return progress;
    }

  };

}

#endif

