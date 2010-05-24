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

#ifndef __PROGRESSCONTROL_H__
#define __PROGRESSCONTROL_H__

#include <tr1/memory>

namespace degate {
  
  /**
   */

  class ProgressControl {
    
  private:

    double progress;
    bool canceled;

    double step_size;

  protected:

    /**
     * Set progress.
     */
    void set_progress(double progress) {
      this->progress = progress;
    }

    /**
     * Set step size.
     */
    void set_progress_step_size(double step_size) {
      this->step_size = step_size;
    }

    /**
     *
     */
    void progress_step_done() {
      progress += step_size;
    }

  public:

    /**
     * The constructor
     */

    ProgressControl() : progress(0), canceled(false) {}

    /**
     * The destructor for a plugin.
     */

    virtual ~ProgressControl() {}

    /**
     * Check if the process is canceled.
     */
    
    virtual bool is_canceled() const {
      return canceled;
    }

    /**
     * Stop the processing.
     */

    virtual void cancel() {
      canceled = true;
    }


    /**
     * Get progress.
     * @return Returns a value between 0 and 100 percent.
     */

    virtual double get_progress() const {
      return progress;
    }

  };

  typedef std::tr1::shared_ptr<ProgressControl> ProgressControl_shptr;
}

#endif

