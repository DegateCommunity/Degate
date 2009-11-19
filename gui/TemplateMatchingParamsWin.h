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

#ifndef __TEMPLATEMATCHINGPARAMSWIN_H__
#define __TEMPLATEMATCHINGPARAMSWIN_H__

#include <gtkmm.h>

#include "Project.h"
#include "LogicModel.h"
#include "GladeFileLoader.h"
#include "ScalingManager.h"

class TemplateMatchingParamsWin : private GladeFileLoader {

 protected:  

  class BGScaleModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    
    BGScaleModelColumns() { add(m_col_scaling); }
    
    Gtk::TreeModelColumn<unsigned int> m_col_scaling;
  };
  
  BGScaleModelColumns m_Columns_scalings;

  class OrientationsModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    
    OrientationsModelColumns() { 
      add(m_col_orientation_str);
      add(m_col_orientations);
    }
    
    Gtk::TreeModelColumn<Glib::ustring> m_col_orientation_str;
    Gtk::TreeModelColumn<std::list<degate::Gate::ORIENTATION> > m_col_orientations;
  };
  
  OrientationsModelColumns m_Columns_orientations;


 public:

  TemplateMatchingParamsWin(Gtk::Window *parent, 
			    degate::ScalingManager_shptr scaling_manager,
			    double threshold_hc,
			    double threshold_detection,
			    unsigned int max_step_size_search,
			    unsigned int preselected_scale_down);
  ~TemplateMatchingParamsWin();
  
  /**
   * Start the dialog window.
   * @return Returns \p true if he the user clicked Ok. It
   *   will return false if the user clicked cancel.
   */
  bool run(double * threshold_hc,
	   double * threshold_detection,
	   unsigned int * max_step_size_search,
	   unsigned int * scale_down,
	   std::list<degate::Gate::ORIENTATION> & tmpl_orientations);
  
 private:
  Gtk::Window *parent;

  Gtk::HScale * hscale_threshold_hc;
  Gtk::HScale * hscale_threshold_detection;
  Gtk::Entry * entry_step_size_search;
  Gtk::ComboBox * combobox_scale_down;
  Gtk::ComboBox * combobox_tmpl_orientations;

  Glib::RefPtr<Gtk::ListStore> m_refTreeModel_scalings;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel_orientations;

  bool ok_clicked;

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();
  virtual void on_combo_changed();

  void populate_orientations_combobox();
};

#endif
