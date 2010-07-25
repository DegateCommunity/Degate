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

#ifndef __RECOGNITIONMANAGER_H__
#define __RECOGNITIONMANAGER_H__

#include <RecognitionGUIBase.h>
#include <list>
#include <SingletonBase.h>

class RecognitionManager : public degate::SingletonBase<RecognitionManager> {

  friend class degate::SingletonBase<RecognitionManager>;

 public:

  typedef std::vector<RecognitionGUIBase_shptr> plugin_list;

 private:
   
  plugin_list plugins;
  
  RecognitionManager();
  
 public:
  
  ~RecognitionManager();
   
  plugin_list get_plugins();
  
  void init(unsigned int slot, 
	    Gtk::Window *parent, 
	    degate::BoundingBox const& bounding_box, 
	    degate::Project_shptr project) {

    assert(slot < plugins.size());
    plugins[slot]->init(parent, bounding_box, project);
  }
  
  bool before_dialog(unsigned int slot) {
    assert(slot < plugins.size());
    return plugins[slot]->before_dialog();
  }
  
  void run(unsigned int slot) {
    assert(slot < plugins.size());
    return plugins[slot]->run();
  }

  void after_dialog(unsigned int slot) {
    assert(slot < plugins.size());
    plugins[slot]->after_dialog();
  }
  
  degate::ProgressControl_shptr get_progress_control(unsigned int slot) { 
    assert(slot < plugins.size());
    return plugins[slot];
  }

};


#endif
