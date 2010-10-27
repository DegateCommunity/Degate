
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

#ifndef __TEMPLATEMATCHINGGUI_H__
#define __TEMPLATEMATCHINGGUI_H__


#include <tr1/memory>
#include <gdkmm/window.h>
#include <libglademm.h>

#include <BoundingBox.h>
#include <Project.h>
#include <TemplateMatching.h>
#include <RecognitionGUIBase.h>
#include <GateTemplate.h>

class TemplateMatchingGUI : public RecognitionGUIBase {
  
 private:

  Gtk::Window *parent;
  degate::BoundingBox bounding_box;
  degate::Project_shptr project;

  degate::TemplateMatching_shptr matching;

  std::list<degate::GateTemplate_shptr> tmpl_set;

  degate::LogicModel_shptr lmodel;
  degate::Layer_shptr current_layer;


 private:

  /**
   * Check if gate templates will fit into the bounding box and if there a template
   * images for the layer type. In case there is only one template to match an error
   * dialog is displayed. If there is more then one template to match, the template
   * is ignored in the search.
   * @return Returns true if templates are ok. If it returns false, the template
   *   matching should be aborted.
   */
  bool check_template_selection(std::list<degate::GateTemplate_shptr> & tmpl_set, 
				degate::Layer::LAYER_TYPE layer_type);


  /**
   * Run a dialog to aquire matching params and set it to the matching algorithm.
   * @return Returns false, if the dialog was canceled.
   */

  bool run_matching_param_dialog();

 public:
  
  TemplateMatchingGUI(degate::TemplateMatching_shptr _matching, std::string const& name);
  
  virtual ~TemplateMatchingGUI();
  
 
  virtual void init(Gtk::Window *parent, 
		    degate::BoundingBox const& bounding_box, 
		    degate::Project_shptr project);
  virtual bool before_dialog();
  virtual void run();
  virtual void after_dialog();

};



#endif
