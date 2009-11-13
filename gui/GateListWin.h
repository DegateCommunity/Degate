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

#ifndef __GATELISTWIN_H__
#define __GATELISTWIN_H__

#include <gtkmm.h>

#include <degate.h>
#include "GladeFileLoader.h"

class GateListWin : private GladeFileLoader {

  class GateListModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    GateListModelColumns() { 
      add(m_col_id); 
      add(m_col_refcount);
      add(m_col_width);
      add(m_col_height);
      add(m_col_short_name); 
      add(m_col_description); 
      add(color_fill_);
      add(color_frame_);
      add(padding_);
    }
    
    Gtk::TreeModelColumn<degate::object_id_t> m_col_id;
    Gtk::TreeModelColumn<int> m_col_refcount;
    Gtk::TreeModelColumn<int> m_col_width;
    Gtk::TreeModelColumn<int> m_col_height;
    Gtk::TreeModelColumn<Glib::ustring> m_col_short_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_description;
    Gtk::TreeModelColumn<Gdk::Color> color_fill_; 
    Gtk::TreeModelColumn<Gdk::Color> color_frame_; 
    Gtk::TreeModelColumn<int> padding_; 
  };


 public:
  GateListWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);
  virtual ~GateListWin();
  void run();

 private:
  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;

  GateListModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  Gdk::Color get_color(degate::color_t col, degate::color_t default_col);
  void fill_row(Gtk::TreeModel::Row const& row, degate::GateTemplate_shptr & tmpl);

  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual void on_add_button_clicked();
  virtual void on_remove_button_clicked();
  virtual void on_edit_button_clicked();

};

#endif
