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

#ifndef __GATECONFIGWIN_H__
#define __GATECONFIGWIN_H__

#include <gtkmm.h>
#include <list>

#include <globals.h>
#include <Project.h>
#include <LogicModel.h>
#include <GladeFileLoader.h>
#include <Editor.h>
#include <RenderWindow.h>
#include <GateRenderer.h>

class GateConfigWin : private GladeFileLoader {

protected:
  
  class PortModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    
    PortModelColumns() { 
      add(m_col_id); 
      add(m_col_name); 
      add(m_col_description); 
      add(m_col_inport); 
      add(m_col_outport); 
    }
    
    Gtk::TreeModelColumn<degate::object_id_t> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_description;
    Gtk::TreeModelColumn<bool> m_col_inport; 
    Gtk::TreeModelColumn<bool> m_col_outport; 
  };

  class LClassModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    
    LClassModelColumns() { 
      add(m_col_ident); 
      add(m_col_descr); 
    }

    Gtk::TreeModelColumn<Glib::ustring> m_col_ident;
    Gtk::TreeModelColumn<Glib::ustring> m_col_descr;
  };


public:
  GateConfigWin(Gtk::Window *parent, 
		degate::LogicModel_shptr lmodel, 
		degate::GateTemplate_shptr gate_template);

  virtual ~GateConfigWin();
        
  bool run();

private:

  enum LANG_ROW_INDEX {
    TEXT = 0,
    VHDL = 1,
    VERILOG = 2
  };


  Gtk::Window *parent;

  degate::LogicModel_shptr lmodel;
  degate::GateTemplate_shptr gate_template;

  std::list<degate::GateTemplatePort_shptr> original_ports;

  PortModelColumns port_model_columns;
  Glib::RefPtr<Gtk::ListStore> refListStore_ports;
  Gtk::TreeView* pTreeView_ports;

  LClassModelColumns lclass_model_columns;
  Glib::RefPtr<Gtk::ListStore> refListStore_lclass;
  Gtk::ComboBox * combobox_logic_class;


  bool result;

  Gtk::Entry * entry_short_name;
  Gtk::Entry * entry_description;

  Gtk::ColorButton * colorbutton_fill_color;
  Gtk::ColorButton * colorbutton_frame_color;
  Gtk::Button * codegen_button;
  Gtk::ComboBox * combobox_lang;
  Gtk::TextView * code_textview;

  std::string selected_logic_class;


  // renderer stuff
  GfxEditor<GateRenderer> editor_transistor;
  RenderWindow<GfxEditor<GateRenderer> > render_window_transistor;

  GfxEditor<GateRenderer> editor_m1;
  RenderWindow<GfxEditor<GateRenderer> > render_window_m1;

  GfxEditor<GateRenderer> editor_transistor_m1;
  RenderWindow<GfxEditor<GateRenderer> > render_window_transistor_m1;


  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();

  virtual void on_port_add_button_clicked();
  virtual void on_port_remove_button_clicked();
  virtual void on_logic_class_changed();

  void on_codegen_button_clicked();
  void on_language_changed();
  void on_code_changed();

  typedef std::map<degate::GateTemplate::IMPLEMENTATION_TYPE, std::string> code_text_map_type;
  code_text_map_type code_text;

  typedef Gtk::TreeModel::Children type_children;

  degate::GateTemplate::IMPLEMENTATION_TYPE lang_idx_to_impl(int idx);

  void insert_logic_classes();
  void append_logic_class(Glib::ustring const& ident, Glib::ustring const& descr);
  void append_logic_class(Glib::ustring const& ident);
};

#endif
