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

#ifndef __GENERICTEXTINPUTWIN_H__
#define __GENERICTEXTINPUTWIN_H__

#include <gtkmm.h>

#include "GladeFileLoader.h"

class GenericTextInputWin : private GladeFileLoader {

 public:
  GenericTextInputWin(Gtk::Window *parent, 
		      Glib::ustring title,
		      Glib::ustring label_text, 
		      Glib::ustring preset_text);
  virtual ~GenericTextInputWin();
        
  bool run(Glib::ustring & new_value);

 private:
  Gtk::Window *parent;
  Gtk::Entry * entry;
  Gtk::Label * label;
  Glib::ustring orig_text;
  Gtk::Button * p_ok_button;

  bool ok_clicked;
  

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();
  virtual void on_entry_text_changed();
};

#endif
