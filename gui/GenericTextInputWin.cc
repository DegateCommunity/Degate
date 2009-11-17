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

#include "GenericTextInputWin.h"

#include <assert.h>
#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <stdlib.h>
#include "GladeFileLoader.h"


GenericTextInputWin::GenericTextInputWin(Gtk::Window *parent,
					 Glib::ustring title,
					 Glib::ustring label_text, 
					 Glib::ustring preset_text) :
  GladeFileLoader("generic_text_input.glade", "generic_text_input_dialog") {
  
  assert(parent);
  this->parent = parent;
  ok_clicked = false;
  orig_text = preset_text;

  if(pDialog) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    pDialog->set_title(title);

    // connect signals
    refXml->get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GenericTextInputWin::on_cancel_button_clicked));
    
    refXml->get_widget("ok_button", p_ok_button);
    if(p_ok_button != NULL) {
      p_ok_button->signal_clicked().connect(sigc::mem_fun(*this, &GenericTextInputWin::on_ok_button_clicked) );
      p_ok_button->grab_focus();
    }
    else return;


    refXml->get_widget("label", label);
    if(label != NULL) {
      label->set_text(label_text);
    }

    refXml->get_widget("entry", entry);
    if(entry != NULL) {
      entry->grab_focus();
      entry->set_text(preset_text);
      entry->signal_changed().connect(sigc::mem_fun(*this, &GenericTextInputWin::on_entry_text_changed) );
      if(preset_text.length() == 0) p_ok_button->set_sensitive(false);
    }

  }
}

GenericTextInputWin::~GenericTextInputWin() {
}


void GenericTextInputWin::on_entry_text_changed() {
  if(entry->get_text().length() > 0) p_ok_button->set_sensitive(true);
  else p_ok_button->set_sensitive(false);
}

bool GenericTextInputWin::run(Glib::ustring & new_value) {
  pDialog->run();
  if(ok_clicked) {
    new_value = entry->get_text();
    return true;
  }
  else {
    new_value = orig_text;
    return false;
  }
}

void GenericTextInputWin::on_ok_button_clicked() {
  ok_clicked = true;
  pDialog->hide();
}

void GenericTextInputWin::on_cancel_button_clicked() {
  ok_clicked = false;
  pDialog->hide();
}

