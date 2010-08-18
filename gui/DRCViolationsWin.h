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

#ifndef __DRCVIOLATIONSWIN_H__
#define __DRCVIOLATIONSWIN_H__

#include <degate.h>
#include "GladeFileLoader.h"
#include <gtkmm.h>

#include <list>

class DRCViolationsWin : public Gtk::Window, private GladeFileLoader {

  class DRCViolationsModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    DRCViolationsModelColumns() {

      add(m_col_object_ptr);
      add(m_col_name);
      add(m_col_violation_class);
      add(m_col_violation_description);

      add(color_);
    }

    Gtk::TreeModelColumn<degate::PlacedLogicModelObject_shptr> m_col_object_ptr;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_violation_class;
    Gtk::TreeModelColumn<Glib::ustring> m_col_violation_description;

    Gtk::TreeModelColumn<Glib::ustring> color_;
  };


 public:

  DRCViolationsWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);

  virtual ~DRCViolationsWin();

  
  /**
   * Run Design Rule Checks and display.
   */
  void run_checks();


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

  Gtk::Button* pGotoButton;
  Gtk::Button* pCloseButton;
  Gtk::Button* pIgnoreDRCButton;
  Gtk::Button* pUpdateButton;


  DRCViolationsModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  sigc::signal<void, degate::PlacedLogicModelObject_shptr>  signal_goto_button_clicked_;

  void clear_list();
  void disable_widgets();
  
  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual void on_goto_button_clicked();
  virtual void on_ignore_button_clicked();
  virtual void on_update_button_clicked();

  virtual void on_selection_changed();
};

#endif
