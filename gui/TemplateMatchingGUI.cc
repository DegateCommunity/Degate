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

#include <LogicModelHelper.h>

#include <TemplateMatchingGUI.h>
#include <TemplateMatchingParamsWin.h>

#include <GateSelectWin.h>

#include <algorithm>

using namespace degate;

TemplateMatchingGUI::TemplateMatchingGUI(TemplateMatching_shptr _matching,
					 std::string const& name) : 
  RecognitionGUIBase(name, _matching),
  matching(_matching) {
}
  
TemplateMatchingGUI::~TemplateMatchingGUI() {
}

void TemplateMatchingGUI::init(Gtk::Window *parent, 
			       degate::BoundingBox const& bounding_box, 
			       degate::Project_shptr project) {
  this->parent = parent;
  this->bounding_box = bounding_box;
  this->project = project;

  lmodel = project->get_logic_model();
  current_layer = lmodel->get_current_layer();
  assert(current_layer != NULL);

}


bool TemplateMatchingGUI::run_matching_param_dialog() {
  double threshold_hc;
  double threshold_detection = 0.7;
  unsigned int max_step_size_search = std::max((length_t)1, project->get_lambda() >> 1);
  unsigned int scale_down = 2;

  std::list<Gate::ORIENTATION> tmpl_orientations;

  TemplateMatchingParamsWin paramsWin(parent, 
				      current_layer->get_scaling_manager(),
				      matching->get_threshold_hc(), 
				      matching->get_threshold_detection(),
				      matching->get_max_step_size(), 
				      matching->get_scaling_factor());

  if(paramsWin.run(&threshold_hc, 
		   &threshold_detection,
		   &max_step_size_search, 
		   &scale_down,
		   tmpl_orientations)) {

    matching->set_threshold_hc(threshold_hc);
    matching->set_threshold_detection(threshold_detection);
    matching->set_max_step_size(max_step_size_search);
    matching->set_scaling_factor(scale_down);
    matching->set_orientations(tmpl_orientations);

    return true;
  }
  else return false;
}

bool TemplateMatchingGUI::before_dialog() { 

  assert(project != NULL);

  if(current_layer->get_layer_type() == Layer::UNDEFINED) {
    Gtk::MessageDialog dialog(*parent, 
			      "The layer type is not defined.", 
			      true, Gtk::MESSAGE_ERROR);
    dialog.set_title("Error");
    dialog.run();  
    return false;
  }

  Layer_shptr logic_layer;

  try{
    logic_layer = get_first_logic_layer(project->get_logic_model());
  }
  catch(CollectionLookupException const& ex) {

    Gtk::MessageDialog dialog(*parent, 
			      "There is no logic layer defined. Please define layer types.",
			      true, Gtk::MESSAGE_ERROR);
    dialog.set_title("Error");
    dialog.run();  

    return false;
  }

  if(bounding_box.get_max_x() == 0 && bounding_box.get_max_y() == 0) {
    Gtk::MessageDialog dialog(*parent, 
			      "You did not select an area for the template matching. "
			      "Do you really wan't to run template matching on the whole "
			      "image? It will take a lot of time.",
			      true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Warning");      
    if(dialog.run() == Gtk::RESPONSE_NO) return false;
    else bounding_box = BoundingBox(project->get_width(), project->get_height());
  }
  
  GateSelectWin gsWin(parent, project->get_logic_model());
  tmpl_set = gsWin.get_selection(true);
  if(tmpl_set.size() == 0) return false;

  if(!check_template_selection(tmpl_set, current_layer->get_layer_type())) return false;
  
  assert(matching != NULL);
  matching->set_templates(tmpl_set);
  matching->set_layers(current_layer, logic_layer);

  bool should_run = run_matching_param_dialog();
  debug(TM, "run? %s", should_run ? "true" : "false");
  return should_run;
}

bool TemplateMatchingGUI::check_template_selection(std::list<GateTemplate_shptr> & tmpl_set, 
						   Layer::LAYER_TYPE layer_type) {

  std::list<GateTemplate_shptr> to_remove;


  for(std::list<GateTemplate_shptr>::const_iterator iter = tmpl_set.begin();
      iter != tmpl_set.end(); ++iter) {
    
    const GateTemplate_shptr tmpl = *iter;

    if(!tmpl->has_image(layer_type)) {
      if(tmpl_set.size() == 1) { // if there is only one element
	Gtk::MessageDialog dialog(*parent, 
				  "There is no template for the selected gate type.", 
				  true, Gtk::MESSAGE_ERROR);
	dialog.set_title("Error");
	dialog.run();
	return false;
      }

      to_remove.push_back(tmpl);
    }

    if(tmpl->get_width() >= (unsigned int)bounding_box.get_width() || 
       tmpl->get_height() >= (unsigned int)bounding_box.get_height()) {

      if(tmpl_set.size() == 1) { // if there is only one element
	Gtk::MessageDialog dialog(*parent, 
				  "The template to match is larger than the search area.", 
				  true, Gtk::MESSAGE_ERROR);
	dialog.set_title("Error");
	dialog.run();
	return false;
      }
      to_remove.push_back(tmpl);
    }
    
  }

  tmpl_set.erase(to_remove.begin(), to_remove.end());
 
  return true;
}


void TemplateMatchingGUI::run() {

  matching->init(bounding_box, project);
  matching->run();
}


