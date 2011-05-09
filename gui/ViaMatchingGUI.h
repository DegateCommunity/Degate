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

#ifndef __VIAMATCHINGGUI_H__
#define __VIAMATCHINGGUI_H__


#include <tr1/memory>
#include <gdkmm/window.h>
#include <libglademm.h>

#include <BoundingBox.h>
#include <Project.h>
#include <ViaMatching.h>
#include <RecognitionGUIBase.h>
#include <Layer.h>


class ViaMatchingGUI : public RecognitionGUIBase {
  
private:

  Gtk::Window *parent;
  degate::BoundingBox bounding_box;
  degate::Project_shptr project;
  
  degate::ViaMatching_shptr matching;
  
private:

  bool check_vias(degate::LogicModel_shptr lmodel, degate::Layer_shptr layer);

public:

  ViaMatchingGUI(degate::ViaMatching_shptr _matching, std::string const& name);

  virtual ~ViaMatchingGUI();

  virtual void init(Gtk::Window *parent,
		    degate::BoundingBox const& bounding_box,
		    degate::Project_shptr project);

  virtual bool before_dialog();

  virtual void run();

  virtual void after_dialog();

};


#endif
