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

#ifndef __CONNECTIONINSPECTORWIN_H__
#define __CONNECTIONINSPECTORWIN_H__

#include <degate.h>
#include "GladeFileLoader.h"
#include <gtkmm.h>

#include <list>

class ConnectionInspectorWin : public Gtk::Window, private GladeFileLoader {

  class ConnectionInspectorModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    ConnectionInspectorModelColumns() { 

      //add(m_col_curr_object_type);
      add(m_col_curr_object_ptr);
      add(m_col_curr_name);
      add(m_col_curr_name_sort);

      //add(m_col_prev_object_type);
      add(m_col_prev_object_ptr);
      add(m_col_prev_name);

      //add(m_col_next_object_type);
      add(m_col_next_object_ptr);
      add(m_col_next_name);

      add(color_);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_curr_object_type_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_curr_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_curr_name_sort;
    Gtk::TreeModelColumn<degate::PlacedLogicModelObject_shptr> m_col_curr_object_ptr;

    Gtk::TreeModelColumn<Glib::ustring> m_col_next_name;
    Gtk::TreeModelColumn<degate::PlacedLogicModelObject_shptr> m_col_next_object_ptr;

    Gtk::TreeModelColumn<Glib::ustring> m_col_prev_name;
    Gtk::TreeModelColumn<degate::PlacedLogicModelObject_shptr> m_col_prev_object_ptr;

    Gtk::TreeModelColumn<Glib::ustring> color_;
  };


 public:

  ConnectionInspectorWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);

  virtual ~ConnectionInspectorWin();

  /**
   * Set an object that should be inspected.
   */
  void set_object(degate::PlacedLogicModelObject_shptr obj_ptr);

  /**
   * Disable the inspection. That is to remove any logic model related
   * content from the window and to show en empty window.
   */
  void disable_inspection();

  /**
   * Display the window.
   */
  void show();

  
  /**
   * Indicate, that a logic model object is or will
   * be removed and that the object should be removed from the inspection.
   */
  void object_removed(degate::PlacedLogicModelObject_shptr obj_ptr);

  /**
   * Indicate, that the current shown logic model object is or will
   * be removed and that the inspection should be disabled.
   * @todo descr is not correct - plural
   */
  void objects_removed();

  /**
   * Set up a callback mechanism for the case a user wants
   * to jump to a logic model object.
   */
  sigc::signal<void, degate::PlacedLogicModelObject_shptr>& signal_goto_button_clicked();

 private:

  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;

  std::list<degate::PlacedLogicModelObject_shptr> back_list;

  Gtk::Button* pBackButton;
  Gtk::Button* pGotoButton;
  Gtk::Button* pCloseButton;


  ConnectionInspectorModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  Gtk::Label * current_object_label;
  Gtk::Label * current_object_type_label;

  sigc::signal<void, degate::PlacedLogicModelObject_shptr>  signal_goto_button_clicked_;

  void clear_list();
  void show_connections(degate::ConnectedLogicModelObject_shptr src_curr_obj,
			Glib::ustring current_color);



  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual void on_goto_button_clicked();
  virtual void on_back_button_clicked();

  virtual void on_selection_changed();
};

#endif
