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

#ifndef __RECOGNITIONGUIBASE_H__
#define __RECOGNITIONGUIBASE_H__

#include <tr1/memory>
#include <gdkmm/window.h>
#include <libglademm.h>

#include <BoundingBox.h>
#include <Project.h>
#include <ProgressControl.h>

class RecognitionGUIBase : public degate::ProgressControl {
private:

  std::string name;
  degate::ProgressControl_shptr pc;

public:
  RecognitionGUIBase(std::string const& _name, degate::ProgressControl_shptr _pc) : 
    name(_name), pc(_pc) {}

  virtual ~RecognitionGUIBase() {}
  virtual void init(Gtk::Window *parent, degate::BoundingBox const& bouding_box, 
		    degate::Project_shptr project) = 0;
  virtual bool before_dialog() = 0;
  virtual void run() = 0;
  virtual void after_dialog() = 0;
  virtual std::string get_name() const { return name; }


  /*
    Here we wrap methods of ProgressControl. So the GUI code can work with
    underlying objects from degate lib without knowing them.    
   */

  virtual double get_progress() const { 
    return pc ? pc->get_progress() : 0; 
  }

  virtual time_t get_time_passed() const {
    return pc ? pc->get_time_passed() : 0; 
  }

  virtual time_t get_time_left() const {
    return pc ? pc->get_time_left() : 0; 
  }

  virtual std::string get_time_left_as_string() const {
    return pc ? pc->get_time_left_as_string() : std::string("-");
  }

  virtual std::string get_log_message() const {
    return pc ? pc->get_log_message() : std::string("");
  }

  virtual bool has_log_message() const {
    return pc ? pc->has_log_message() : false;
  }

  virtual void cancel() { 
    if(pc) pc->cancel(); 
  }

  virtual bool is_canceled() const {
    if(pc) return pc->is_canceled(); 
    else return false;
  }

};

typedef std::tr1::shared_ptr<RecognitionGUIBase> RecognitionGUIBase_shptr;



#endif
