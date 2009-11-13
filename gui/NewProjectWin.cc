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

#include "NewProjectWin.h"

#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>

using namespace degate;

NewProjectWin::NewProjectWin(Gtk::Window *parent) : 
  m_Frame_Size("Size"),
  m_Label_Width("Width:"),
  m_Label_Height("Height:"),
  m_Frame_Layers("Number of layers"),
  m_Button_Ok("Ok") {

  set_title("Create a new project");
  set_default_size(250, 200);
  set_border_width(5);
  set_transient_for(*parent);
  set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);

  add(m_Box);

  m_Entry_Width.set_max_length(15);
  m_Entry_Height.set_max_length(15);
  m_Entry_Width.set_text("3000");
  m_Entry_Height.set_text("3000");
  m_Box_Size.pack_start(m_Label_Width, Gtk::PACK_EXPAND_WIDGET);
  m_Box_Size.pack_start(m_Entry_Width, Gtk::PACK_EXPAND_WIDGET);
  m_Box_Size.pack_start(m_Label_Height, Gtk::PACK_EXPAND_WIDGET);
  m_Box_Size.pack_start(m_Entry_Height, Gtk::PACK_EXPAND_WIDGET);
  m_Frame_Size.add(m_Box_Size);
  m_Box.pack_start(m_Frame_Size,  Gtk::PACK_EXPAND_WIDGET);


  m_Entry_Layers.set_text("3");
  m_Box_Layers.pack_start(m_Entry_Layers, Gtk::PACK_EXPAND_WIDGET);
  m_Frame_Layers.add(m_Box_Layers);
  m_Box.pack_start(m_Frame_Layers,  Gtk::PACK_EXPAND_WIDGET);

  m_Button_Ok.signal_clicked().connect( sigc::mem_fun(*this, &NewProjectWin::on_ok_button_clicked) );
  m_Button_Ok.set_label("Ok");
  m_Button_Ok.set_use_stock(true);
  m_Button_Ok.set_relief(Gtk::RELIEF_NORMAL);
  m_Box.pack_start(m_Button_Ok, Gtk::PACK_SHRINK);

  show_all_children();
}

NewProjectWin::~NewProjectWin() {
}

void NewProjectWin::on_ok_button_clicked() {
  hide();
}


unsigned int NewProjectWin::get_width() {
  int val = atoi(m_Entry_Width.get_text().c_str());
  return val > 0 ? val : 0;
}

unsigned int NewProjectWin::get_height() {
  int val = atoi(m_Entry_Height.get_text().c_str());
  return val > 0 ? val : 0;
}

unsigned int NewProjectWin::get_layers() {
  int val = atoi(m_Entry_Layers.get_text().c_str());
  return val > 0 ? val : 0;
}
