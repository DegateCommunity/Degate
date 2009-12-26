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

#ifndef __WIREMATCHINGGUI_H__
#define __WIREMATCHINGGUI_H__


#include <tr1/memory>
#include <gdkmm/window.h>
#include <libglademm.h>

#include <BoundingBox.h>
#include <Project.h>
#include <WireMatching.h>
#include <RecognitionGUIBase.h>
#include <GateTemplate.h>

class WireMatchingGUI : public RecognitionGUIBase {
  
 private:

  Gtk::Window *parent;
  degate::BoundingBox bounding_box;
  degate::Project_shptr project;

  degate::WireMatching_shptr matching;

 private:

  /**
   * Run a dialog to aquire matching params and set it to the matching algorithm.
   * @return Returns false, if the dialog was canceled.
   */

  bool run_matching_param_dialog();

 public:
  
  WireMatchingGUI(degate::WireMatching_shptr _matching, std::string const& name) : 
    RecognitionGUIBase(name),
    matching(_matching) {}
  
  virtual ~WireMatchingGUI() {}
  
 
  virtual void init(Gtk::Window *parent, 
		    degate::BoundingBox const& bounding_box, 
		    degate::Project_shptr project) {
    this->parent = parent;
    this->bounding_box = bounding_box;
    this->project = project;
    
  }

  virtual bool before_dialog() { return true; }

  virtual void run() {
    matching->init(bounding_box, project);
    matching->run();
  }
  virtual void after_dialog() {}
  
};



#endif
