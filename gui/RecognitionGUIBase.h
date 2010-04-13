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


class RecognitionGUIBase {
private:

  std::string name;

public:
  RecognitionGUIBase(std::string const& _name) : name(_name) {}

  virtual ~RecognitionGUIBase() {}
  virtual void init(Gtk::Window *parent, degate::BoundingBox const& bouding_box, degate::Project_shptr project) = 0;
  virtual bool before_dialog() = 0;
  virtual void run() = 0;
  virtual void after_dialog() = 0;
  virtual std::string get_name() const { return name; }
};





#endif
