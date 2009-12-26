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

#ifndef __WIREMATCHING_H__
#define __WIREMATCHING_H__

#include <Image.h>
#include <Project.h>

namespace degate {

  class WireMatching : public Matching {

  private:

    Layer_shptr layer;

  public:

    WireMatching() {
    }

    virtual void init(BoundingBox const& bounding_box, Project_shptr project) 
      throw(InvalidPointerException, DegateRuntimeException) {

      if(project == NULL)
	throw InvalidPointerException("Invalid pointer for parameter project.");

      LogicModel_shptr lmodel = project->get_logic_model();
      assert(lmodel != NULL); // always has a logic model

      layer = lmodel->get_current_layer();
      if(layer == NULL) throw DegateRuntimeException("No current layer in project.");


      ScalingManager_shptr sm = layer->get_scaling_manager();
      assert(sm != NULL);
      BackgroundImage_shptr img = sm->get_image(1).second;
      assert(img != NULL);

    }

    virtual void run() {
    }
  };

  typedef std::tr1::shared_ptr<WireMatching> WireMatching_shptr;
}

#endif
