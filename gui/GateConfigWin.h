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

#ifndef __GATECONFIGWIN_H__
#define __GATECONFIGWIN_H__

#include <gtkmm.h>
#include <list>

#include "globals.h"
#include "Project.h"
#include "LogicModel.h"
#include "GladeFileLoader.h"

class GateConfigWin : private GladeFileLoader {

  class GateConfigModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    GateConfigModelColumns() { 
      add(m_col_id); 
      add(m_col_text); 
      add(m_col_inport); 
      add(m_col_outport); 
    }
    
    Gtk::TreeModelColumn<degate::object_id_t> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_text;
    Gtk::TreeModelColumn<bool> m_col_inport; 
    Gtk::TreeModelColumn<bool> m_col_outport; 
  };

 public:
  GateConfigWin(Gtk::Window *parent, 
		degate::LogicModel_shptr lmodel, 
		degate::GateTemplate_shptr gate_template);

  virtual ~GateConfigWin();
        
  bool run();

  private:
  Gtk::Window *parent;

  degate::LogicModel_shptr lmodel;
  degate::GateTemplate_shptr gate_template;

  std::list<degate::GateTemplatePort_shptr> original_ports;

  GateConfigModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore_ports;

  Gtk::TreeView* pTreeView_ports;

  bool result;

  Gtk::Entry * entry_short_name;
  Gtk::Entry * entry_description;

  Gtk::ColorButton * colorbutton_fill_color;
  Gtk::ColorButton * colorbutton_frame_color;

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();

  virtual void on_port_add_button_clicked();
  virtual void on_port_remove_button_clicked();

};

#endif
