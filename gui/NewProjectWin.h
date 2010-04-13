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

#ifndef __NEWRPROJECTWINWIN_H__
#define __NEWPROJECTWIN_H__

#include "globals.h"
#include <gtkmm.h>

class NewProjectWin : public Gtk::Window {
 public:
  NewProjectWin(Gtk::Window *parent);
  virtual ~NewProjectWin();
    
  // return 0 on error
  unsigned int get_width();
  unsigned int get_height();
  unsigned int get_layers();

  private:

  // Child widgets
  Gtk::VBox m_Box;

  Gtk::Frame m_Frame_Size;
  Gtk::VBox  m_Box_Size;
  Gtk::Entry m_Entry_Width;
  Gtk::Entry m_Entry_Height;
  Gtk::Label m_Label_Width;
  Gtk::Label m_Label_Height;


  Gtk::Frame m_Frame_Layers;
  Gtk::VBox  m_Box_Layers;
  Gtk::Entry m_Entry_Layers;
  
  Gtk::Button m_Button_Ok;

  // Signal handlers:
  virtual void on_ok_button_clicked();
};

#endif
