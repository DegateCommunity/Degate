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

#include "SetOrientationWin.h"

#include <assert.h>
#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <stdlib.h>
#include "Gate.h"

using namespace degate;

SetOrientationWin::SetOrientationWin(Gtk::Window *parent, Gate::ORIENTATION orientation ) :
  GladeFileLoader("set_orientation.glade", "set_orientation_dialog") {

  assert(parent);
  this->parent = parent;
  this->orig_orientation = orientation;
  ok_clicked = false;

  if(get_dialog()) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SetOrientationWin::on_cancel_button_clicked));
    
    get_widget("ok_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &SetOrientationWin::on_ok_button_clicked) );
  
    get_widget("combobox1", entry);
    if(entry) {

      m_refTreeModel = Gtk::ListStore::create(m_Columns);
      entry->set_model(m_refTreeModel);

      Gtk::TreeModel::Row row = *(m_refTreeModel->append());
      row[m_Columns.m_col_id] = Gate::ORIENTATION_UNDEFINED;
      row[m_Columns.m_col_name] = "undefined";

      row = *(m_refTreeModel->append());
      row[m_Columns.m_col_id] = Gate::ORIENTATION_NORMAL;
      row[m_Columns.m_col_name] = "normal";

      row = *(m_refTreeModel->append());
      row[m_Columns.m_col_id] = Gate::ORIENTATION_FLIPPED_UP_DOWN;
      row[m_Columns.m_col_name] = "flipped up-down";

      row = *(m_refTreeModel->append());
      row[m_Columns.m_col_id] = Gate::ORIENTATION_FLIPPED_LEFT_RIGHT;
      row[m_Columns.m_col_name] = "flipped left-right";

      row = *(m_refTreeModel->append());
      row[m_Columns.m_col_id] = Gate::ORIENTATION_FLIPPED_BOTH;
      row[m_Columns.m_col_name] = "flipped left-right and up-down";

      entry->pack_start(m_Columns.m_col_name);
      entry->set_active(orientation);
    }

  }
}

SetOrientationWin::~SetOrientationWin() {
}


Gate::ORIENTATION SetOrientationWin::run() {
  get_dialog()->run();
  if(ok_clicked) return orientation;
  else return orig_orientation;
}

void SetOrientationWin::on_ok_button_clicked() {
  ok_clicked = true;

  Gtk::TreeModel::iterator iter = entry->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
      orientation = row[m_Columns.m_col_id];
      get_dialog()->hide();
    }
  }
}

void SetOrientationWin::on_cancel_button_clicked() {
  ok_clicked = false;
  get_dialog()->hide();
}

