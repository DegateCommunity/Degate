/*                                                                              
                                                                                
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

#include "ProjectSettingsWin.h"

#include <assert.h>
#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <stdlib.h>

#include "Project.h"

using namespace degate;

ProjectSettingsWin::ProjectSettingsWin(Gtk::Window *parent, Project_shptr project) : 
  GladeFileLoader("project_settings.glade", "project_settings_dialog") {

  assert(parent);
  this->parent = parent;
  ok_clicked = false;
  this->project = project;

  if(get_dialog()) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &ProjectSettingsWin::on_cancel_button_clicked));
    
    get_widget("ok_button", pButton);
    if(pButton) {
      pButton->grab_focus();
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &ProjectSettingsWin::on_ok_button_clicked) );
    }

    char str[100];

    get_widget("entry_project_name", entry_project_name);
    if(entry_project_name != NULL) {
      entry_project_name->set_text(project->get_name());
    }

    get_widget("entry_project_description", entry_project_description);
    if(entry_project_description != NULL) {
      entry_project_description->set_text(project->get_description());
    }

    get_widget("entry_lambda", entry_lambda);
    if(entry_lambda) {
      snprintf(str, sizeof(str), "%d", project->get_lambda());
      entry_lambda->set_text(str);
    }
    get_widget("entry_via_diameter", entry_via_diameter);
    if(entry_via_diameter) {
      snprintf(str, sizeof(str), "%d", project->get_default_pin_diameter());
      entry_via_diameter->set_text(str);
    }
    get_widget("entry_wire_diameter", entry_wire_diameter);
    if(entry_wire_diameter) {
      snprintf(str, sizeof(str), "%d", project->get_default_wire_diameter());
      entry_wire_diameter->set_text(str);
    }
    get_widget("entry_server_url", entry_server_url);
    if(entry_server_url) {
      entry_server_url->set_text(project->get_server_url());
    }
    get_widget("entry_pixel_per_um", entry_pixel_per_um);
    if(entry_pixel_per_um) {
      snprintf(str, sizeof(str), "%f", project->get_pixel_per_um());
      entry_pixel_per_um->set_text(str);
    }


    Gtk::ColorButton * pCButton;

    get_widget("colorbutton_wire", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_WIRE));

    get_widget("colorbutton_via_up", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_VIA_UP));

    get_widget("colorbutton_via_down", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_VIA_DOWN));

    get_widget("colorbutton_grid", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_GRID));

    get_widget("colorbutton_annotation", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_ANNOTATION));

    get_widget("colorbutton_annotation_frame", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_ANNOTATION_FRAME));

    get_widget("colorbutton_gate", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_GATE));

    get_widget("colorbutton_gate_frame", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_GATE_FRAME));

    get_widget("colorbutton_gate_port", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_GATE_PORT));

    get_widget("colorbutton_text", pCButton);
    assert(pCButton != NULL);
    if(pCButton) set_color_for_button(pCButton, project->get_default_color(DEFAULT_COLOR_TEXT));
   
  }
}

void ProjectSettingsWin::set_color_for_button(Gtk::ColorButton * pCButton, color_t c) {
  Gdk::Color col;
  col.set_red(MASK_R(c) << 8);
  col.set_green(MASK_G(c) << 8);
  col.set_blue(MASK_B(c) << 8);
  pCButton->set_color(col);
  pCButton->set_alpha(MASK_A(c) << 8);
}


ProjectSettingsWin::~ProjectSettingsWin() {
}


degate::color_t ProjectSettingsWin::get_color_for_button(std::string const & button_name) const {
  Gtk::ColorButton * pCButton;
  get_widget(button_name, pCButton);
  if(pCButton) {
    Gdk::Color col = pCButton->get_color();
    color_t c = MERGE_CHANNELS( (col.get_red() >> 8),				
				(col.get_green() >> 8),
				(col.get_blue() >> 8),
				(pCButton->get_alpha() >> 8));
    return c;
  }

  return 0;
}

bool ProjectSettingsWin::run() {
  get_dialog()->run();
  if(ok_clicked) {
    long r;
    double d;
    project->set_name(entry_project_name->get_text().c_str());
    project->set_description(entry_project_description->get_text().c_str());
    project->set_server_url(entry_server_url->get_text().c_str());

    if((r = atol(entry_lambda->get_text().c_str())) > 0)
      project->set_lambda(r);
    if((r = atol(entry_via_diameter->get_text().c_str())) > 0)
      project->set_default_pin_diameter(r);
    if((r = atol(entry_wire_diameter->get_text().c_str())) > 0)
      project->set_default_wire_diameter(r);

    if((d = atof(entry_pixel_per_um->get_text().c_str())) >= 0)
      project->set_pixel_per_um(d);


    project->set_default_color(DEFAULT_COLOR_WIRE, 
			       get_color_for_button("colorbutton_wire"));
    project->set_default_color(DEFAULT_COLOR_VIA_UP, 
			       get_color_for_button("colorbutton_via_up"));
    project->set_default_color(DEFAULT_COLOR_VIA_DOWN, 
			       get_color_for_button("colorbutton_via_down"));
    project->set_default_color(DEFAULT_COLOR_GRID, 
			       get_color_for_button("colorbutton_grid"));
    project->set_default_color(DEFAULT_COLOR_ANNOTATION, 
			       get_color_for_button("colorbutton_annotation"));
    project->set_default_color(DEFAULT_COLOR_ANNOTATION_FRAME, 
			       get_color_for_button("colorbutton_annotation_frame"));
    project->set_default_color(DEFAULT_COLOR_GATE, 
			       get_color_for_button("colorbutton_gate"));
    project->set_default_color(DEFAULT_COLOR_GATE_FRAME, 
			       get_color_for_button("colorbutton_gate_frame"));
    project->set_default_color(DEFAULT_COLOR_GATE_PORT, 
			       get_color_for_button("colorbutton_gate_port"));
    project->set_default_color(DEFAULT_COLOR_TEXT, 
			       get_color_for_button("colorbutton_text"));


    project->set_changed();

    return true;
  }
  else return false;
}

void ProjectSettingsWin::on_ok_button_clicked() {
  ok_clicked = true;
  get_dialog()->hide();
}

void ProjectSettingsWin::on_cancel_button_clicked() {
  ok_clicked = false;
  get_dialog()->hide();
}

