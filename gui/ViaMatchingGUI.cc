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

#include <ViaMatchingGUI.h>
#include <ViaMatchingParamsWin.h>
#include <gtkmm.h>

using namespace degate;

ViaMatchingGUI::ViaMatchingGUI(degate::ViaMatching_shptr _matching, 
			       std::string const& name) : 
  RecognitionGUIBase(name, _matching),
  matching(_matching) {
}
  
ViaMatchingGUI::~ViaMatchingGUI() {
}
  
 
void ViaMatchingGUI::init(Gtk::Window *parent, 
			  degate::BoundingBox const& bounding_box, 
			  degate::Project_shptr project) {
  this->parent = parent;
  this->bounding_box = bounding_box;
  this->project = project;    
}

bool ViaMatchingGUI::before_dialog() { 
  
  assert(project != NULL);
  
  LogicModel_shptr lmodel = project->get_logic_model();
  assert(lmodel != NULL); // always has a logic model
  
  Layer_shptr layer = lmodel->get_current_layer();
  assert(layer != NULL);

  if(bounding_box.get_max_x() == 0 && bounding_box.get_max_y() == 0) {
    Gtk::MessageDialog dialog(*parent, 
			      "You did not select an area for the via matching. "
			      "Do you really wan't to run via matching on the whole "
			      "image? It will take a lot of time.",
			      true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Warning");      
    if(dialog.run() == Gtk::RESPONSE_NO) return false;
    else bounding_box = BoundingBox(project->get_width(), project->get_height());
  }
  
  unsigned int median_filter_width = 3;
  double sigma = 0.5;

  ViaMatchingParamsWin wm(parent, median_filter_width, sigma);

  if(wm.run(&median_filter_width, &sigma)) {
    
    matching->set_median_filter_width(median_filter_width);
    matching->set_sigma(sigma);

    return true;
  }
  else return false;
}

void ViaMatchingGUI::run() {
  matching->init(bounding_box, project);
  matching->run();
}

void ViaMatchingGUI::after_dialog() {
}

