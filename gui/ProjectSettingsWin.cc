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

  if(pDialog) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    refXml->get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &ProjectSettingsWin::on_cancel_button_clicked));
    
    refXml->get_widget("ok_button", pButton);
    if(pButton) {
      pButton->grab_focus();
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &ProjectSettingsWin::on_ok_button_clicked) );
    }

    char str[100];

    refXml->get_widget("entry_project_name", entry_project_name);
    if(entry_project_name != NULL) {
      entry_project_name->set_text(project->get_name());
    }

    refXml->get_widget("entry_project_description", entry_project_description);
    if(entry_project_description != NULL) {
      entry_project_description->set_text(project->get_description());
    }

    refXml->get_widget("entry_lambda", entry_lambda);
    if(entry_lambda) {
      snprintf(str, sizeof(str), "%d", project->get_lambda());
      entry_lambda->set_text(str);
    }
    refXml->get_widget("entry_via_diameter", entry_via_diameter);
    if(entry_via_diameter) {
      snprintf(str, sizeof(str), "%d", project->get_default_pin_diameter());
      entry_via_diameter->set_text(str);
    }
    refXml->get_widget("entry_wire_diameter", entry_wire_diameter);
    if(entry_wire_diameter) {
      snprintf(str, sizeof(str), "%d", project->get_default_wire_diameter());
      entry_wire_diameter->set_text(str);
    }
    refXml->get_widget("entry_server_url", entry_server_url);
    if(entry_server_url) {
      entry_server_url->set_text(project->get_server_url());
    }
  }
}

ProjectSettingsWin::~ProjectSettingsWin() {
}


bool ProjectSettingsWin::run() {
  pDialog->run();
  if(ok_clicked) {
    long r;
    project->set_name(entry_project_name->get_text().c_str());
    project->set_description(entry_project_description->get_text().c_str());
    project->set_server_url(entry_server_url->get_text().c_str());

    if((r = atol(entry_lambda->get_text().c_str())) > 0)
      project->set_lambda(r);
    if((r = atol(entry_via_diameter->get_text().c_str())) > 0)
      project->set_default_pin_diameter(r);
    if((r = atol(entry_wire_diameter->get_text().c_str())) > 0)
      project->set_default_wire_diameter(r);

    project->set_changed();

    return true;
  }
  else return false;
}

void ProjectSettingsWin::on_ok_button_clicked() {
  ok_clicked = true;
  pDialog->hide();
}

void ProjectSettingsWin::on_cancel_button_clicked() {
  ok_clicked = false;
  pDialog->hide();
}

