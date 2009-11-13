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

#ifndef __PORTCOLORSWIN_H__
#define __PORTCOLORSWIN_H__

#include <gtkmm.h>
#include <degate.h>
#include "PortColorManager.h"
#include "GladeFileLoader.h"

class PortColorsWin : private GladeFileLoader {

  class PortColorsModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    PortColorsModelColumns() { 
      add(m_col_port_name); 
      add(color_);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_port_name;
    Gtk::TreeModelColumn<Gdk::Color> color_;
  };


 public:
  PortColorsWin(Gtk::Window *parent, 
		degate::LogicModel_shptr lmodel, 
		degate::PortColorManager_shptr pcm);

  ~PortColorsWin();
  void run();

 private:

  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;
  degate::PortColorManager_shptr pcm;

  Gtk::Button* pEditButton;
  Gtk::Button* pRemoveButton;

  PortColorsModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  Gdk::Color get_color(degate::color_t col);

  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual void on_add_button_clicked();
  virtual void on_remove_button_clicked();
  virtual void on_edit_button_clicked();

  virtual void on_selection_changed();
};

#endif
