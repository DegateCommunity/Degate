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

#ifndef __PORTSELECTWIN_H__
#define __PORTSELECTWIN_H__

#include <gtkmm.h>

#include "Project.h"
#include "LogicModel.h"
#include "GladeFileLoader.h"

class PortSelectWin : private GladeFileLoader {

  class PortListModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    PortListModelColumns() { 
      add(m_col_id); 
      add(m_col_name); 
    }
    
    Gtk::TreeModelColumn<degate::object_id_t> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };

 public:
  PortSelectWin(Gtk::Window *parent, degate::Gate_shptr gate);

  virtual ~PortSelectWin();
  
  degate::GateTemplatePort_shptr run();

 private:

  Gtk::Window *parent;
  degate::Gate_shptr gate;
  degate::GateTemplatePort_shptr template_port;


  PortListModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();

};

#endif
