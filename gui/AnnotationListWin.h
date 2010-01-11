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

#ifndef __ANNOTATIONLISTWIN_H__
#define __ANNOTATIONLISTWIN_H__

#include <degate.h>
#include "GladeFileLoader.h"
#include <gtkmm.h>

#include <list>

class AnnotationListWin : public Gtk::Window, private GladeFileLoader {

  class AnnotationListModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    AnnotationListModelColumns() { 

      add(m_col_layer_pos);
      add(m_col_annotation_class);
      add(m_col_name);
      add(m_col_description);
      add(m_col_object_ptr);
    }
    
    Gtk::TreeModelColumn<degate::layer_position_t> m_col_layer_pos;
    Gtk::TreeModelColumn<degate::Annotation::class_id_t> m_col_annotation_class;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_description;
    Gtk::TreeModelColumn<degate::Annotation_shptr> m_col_object_ptr;
  };


 public:

  AnnotationListWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);

  virtual ~AnnotationListWin();

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
   * Refresh content.
   */
  void refresh();


  /**
   * Set up a callback mechanism for the case a user wants
   * to jump to a logic model object.
   */
  sigc::signal<void, degate::PlacedLogicModelObject_shptr>& signal_goto_button_clicked();

 private:

  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;

  Gtk::Button* pGotoButton;
  Gtk::Button* pCloseButton;

  Gtk::Entry * entry_filter_by_layer;
  Gtk::Entry * entry_filter_by_class;

  AnnotationListModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  sigc::signal<void, degate::PlacedLogicModelObject_shptr>  signal_goto_button_clicked_;

  void clear_list();

  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual void on_goto_button_clicked();

  virtual void on_selection_changed();
  virtual void on_entry_changed();
  
  void on_edited_name(const Glib::ustring & path, const Glib::ustring & new_text);
  void on_edited_description(const Glib::ustring & path, const Glib::ustring & new_text);
};

#endif
