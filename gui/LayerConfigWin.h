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

#ifndef __LAYERCONFIGWIN_H__
#define __LAYERCONFIGWIN_H__

#include <gtkmm.h>
#include <list>
#include <utility>


#include "InProgressWin.h"
#include "GladeFileLoader.h"

#include <globals.h>
#include <Project.h>
#include <LogicModel.h>
#include <Layer.h>

class LayerConfigWin : private GladeFileLoader {

 protected:

  class LayerConfigModelColumns : public Gtk::TreeModelColumnRecord {

  public:
    
    LayerConfigModelColumns() { 
      add(m_col_old_position); 
      add(m_col_new_position); 
      add(m_col_enabled);
      add(m_col_layer_type_chosen);
      add(m_col_layer_type_choices);
      add(m_col_description);
      add(m_col_filename);
      add(m_col_exists);
    }
    
    Gtk::TreeModelColumn<int> m_col_old_position;
    Gtk::TreeModelColumn<int> m_col_new_position;
    Gtk::TreeModelColumn<bool> m_col_enabled; 
    Gtk::TreeModelColumn<Glib::ustring> m_col_layer_type_chosen;
    Gtk::TreeModelColumn<Glib::RefPtr<Gtk::TreeModel> > m_col_layer_type_choices;
    Gtk::TreeModelColumn<Glib::ustring> m_col_description;
    Gtk::TreeModelColumn<Glib::ustring> m_col_filename;
    Gtk::TreeModelColumn<bool> m_col_exists; 
  };

  class LayerConfigModelColumnsCombo : public Gtk::TreeModelColumnRecord {
  public:
    LayerConfigModelColumnsCombo() {
      add(m_col_choice);
    }
    Gtk::TreeModelColumn<Glib::ustring> m_col_choice;
  };

  void on_cellrenderer_choice_edited(const Glib::ustring& path_string, 
				     const Glib::ustring& new_text);

 public:

  LayerConfigWin(Gtk::Window *parent,
		 degate::LogicModel_shptr lmodel,
		 std::string const& project_dir);

  virtual ~LayerConfigWin();
        
  bool run();

  /**
   * Set up a callback mechanism for the case calling code should be notified.
   */

  Glib::Dispatcher& signal_on_background_import_finished();

 private:

  typedef Gtk::TreeModel::Children type_children;

  typedef std::list<std::pair<degate::Layer_shptr, std::string> > image_list;

  Gtk::Window * parent;
  degate::LogicModel_shptr lmodel;
  std::string project_dir;

  InProgressWin * ipWin;
  
  Gtk::TreeView* pTreeView_layers;

  LayerConfigModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore_layers;

  LayerConfigModelColumnsCombo m_ColumnsCombo;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModelCombo;

  Gtk::Button 
    * p_ok_button,
    * p_remove_button,
    * p_bg_file_button,
    * p_clear_bg_button;

  Glib::Dispatcher _signal_bg_import_finished_; // class internal signal

  Glib::Dispatcher signal_on_background_import_finished_; // to notify third parties

  Glib::ustring last_filename;

  bool result;

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();

  virtual void on_add_button_clicked();
  virtual void on_remove_button_clicked();

  virtual void on_clear_bg_button_clicked();
  virtual void on_bg_file_button_clicked();

  virtual void on_selection_changed();
  virtual void drag_end_handler(const Glib::RefPtr<Gdk::DragContext> & context);


  void background_import_thread(image_list l);
  void _on_background_import_finished();

  // helper
  
  virtual void update_new_positions();
  void check_at_least_one_layer_enabled();
};

#endif
