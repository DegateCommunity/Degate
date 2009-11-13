/*                                                                              
                                                                                
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

#include "PortSelectWin.h"

#include <assert.h>
#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <stdlib.h>

using namespace degate;

PortSelectWin::PortSelectWin(Gtk::Window *parent, Gate_shptr gate) :
  GladeFileLoader("port_select.glade", "port_select_dialog"){

  assert(parent != NULL);
  assert(gate != NULL);
  assert(gate->has_template() == true);

  this->parent = parent;
  this->gate = gate;

  if(pDialog) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    refXml->get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &PortSelectWin::on_cancel_button_clicked));
    
    refXml->get_widget("ok_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &PortSelectWin::on_ok_button_clicked) );
      pButton->grab_focus();
    }

    refListStore = Gtk::ListStore::create(m_Columns);
  
    refXml->get_widget("treeview", pTreeView);
    if(pTreeView) {
      pTreeView->set_model(refListStore);
      pTreeView->append_column("ID", m_Columns.m_col_id);
      pTreeView->append_column("Port Name", m_Columns.m_col_name);
    }
    
    GateTemplate_shptr tmpl = gate->get_gate_template();
    for(GateTemplate::port_iterator iter = tmpl->ports_begin();
	iter != tmpl->ports_end(); ++iter) {

      GateTemplatePort_shptr tmpl_port = *iter;

      Gtk::TreeModel::Row row = *(refListStore->append()); 
	
      row[m_Columns.m_col_id] = tmpl_port->get_object_id();
      row[m_Columns.m_col_name] = tmpl_port->get_name();
      
    }
  }
}

PortSelectWin::~PortSelectWin() {
}


GateTemplatePort_shptr PortSelectWin::run() {
  pDialog->run();
  return template_port;
}

void PortSelectWin::on_ok_button_clicked() {

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(iter && *iter) {
      Gtk::TreeModel::Row row = *iter; 
      object_id_t port_id = row[m_Columns.m_col_id];
      assert(gate->has_template());

      GateTemplate_shptr tmpl = gate->get_gate_template();
      template_port = tmpl->get_template_port(port_id);
      pDialog->hide();
    }
  }
}

void PortSelectWin::on_cancel_button_clicked() {
  pDialog->hide();
}

