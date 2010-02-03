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

#include "GateConfigWin.h"
#include "GladeFileLoader.h"
#include <VHDLCodeTemplateGenerator.h>

#include <gdkmm/window.h>
#include <gtkmm/stock.h>
#include <libglademm.h>

#include <stdlib.h>
#include <iostream>

#include "GateTemplate.h"
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace degate;
using namespace boost;

GateConfigWin::GateConfigWin(Gtk::Window *parent, 
			     LogicModel_shptr lmodel,
			     GateTemplate_shptr gate_template) :
  GladeFileLoader("gate_create.glade", "gate_create_dialog") {

  this->lmodel = lmodel;
  this->gate_template = gate_template;

  this->parent = parent;

  if(pDialog) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    refXml->get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_cancel_button_clicked));
    
    refXml->get_widget("ok_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_ok_button_clicked) );

    
    refXml->get_widget("port_add_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_port_add_button_clicked) );
    
    refXml->get_widget("port_remove_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_port_remove_button_clicked) );
    
   
      refListStore_ports = Gtk::ListStore::create(port_model_columns);
      
      refXml->get_widget("treeview_ports", pTreeView_ports);
      if(pTreeView_ports) {
	pTreeView_ports->set_model(refListStore_ports);
	pTreeView_ports->append_column("Port ID", port_model_columns.m_col_id);
	pTreeView_ports->append_column_editable("Port Name", port_model_columns.m_col_text);
	pTreeView_ports->append_column_editable("In", port_model_columns.m_col_inport);
	pTreeView_ports->append_column_editable("Out", port_model_columns.m_col_outport);
      }
      

      color_t frame_color = gate_template->get_frame_color();
      color_t fill_color = gate_template->get_fill_color();

      refXml->get_widget("colorbutton_fill_color", colorbutton_fill_color);
      if(colorbutton_fill_color != NULL) {
	Gdk::Color c;
	if(fill_color != 0) {
	  c.set_red(MASK_R(fill_color) << 8);
	  c.set_green(MASK_G(fill_color) << 8);
	  c.set_blue(MASK_B(fill_color) << 8);
	  colorbutton_fill_color->set_alpha(MASK_A(fill_color) << 8);
	  colorbutton_fill_color->set_color(c);
	}
	else {
	  c.set_red(0x30 << 8);
	  c.set_green(0x30 << 8);
	  c.set_blue(0x30 << 8);
	  colorbutton_fill_color->set_alpha(0xa0 << 8);
	  colorbutton_fill_color->set_color(c);
	}
      }

      refXml->get_widget("colorbutton_frame_color", colorbutton_frame_color);
      if(colorbutton_frame_color != NULL) {
	Gdk::Color c;
	if(frame_color != 0) {
	  c.set_red(MASK_R(frame_color) << 8);
	  c.set_green(MASK_G(frame_color) << 8);
	  c.set_blue(MASK_B(frame_color) << 8);
	  colorbutton_frame_color->set_alpha(MASK_A(frame_color) << 8);
	  colorbutton_frame_color->set_color(c);
	}
	else {
	  c.set_red(0xa0 << 8);
	  c.set_green(0xa0 << 8);
	  c.set_blue(0xa0 << 8);
	  colorbutton_fill_color->set_alpha(0x7f << 8);
	  colorbutton_fill_color->set_color(c);
	}
      }


      for(GateTemplate::port_iterator iter = gate_template->ports_begin();
	  iter != gate_template->ports_end();
	  ++iter) {
		
	Gtk::TreeModel::Row row = *(refListStore_ports->append());

	GateTemplatePort_shptr tmpl_port = *iter;

	debug(TM, "PORT NAME: [%s]", tmpl_port->get_name().c_str());

	row[port_model_columns.m_col_inport] = tmpl_port->is_inport();
	row[port_model_columns.m_col_outport] = tmpl_port->is_outport();
	row[port_model_columns.m_col_text] = tmpl_port->get_name();
	row[port_model_columns.m_col_id] = tmpl_port->get_object_id();

	original_ports.push_back(tmpl_port);
      }
      
      refXml->get_widget("entry_short_name", entry_short_name);
      assert(entry_short_name != NULL);
      if(entry_short_name) 
	entry_short_name->set_text(gate_template->get_name());

      refXml->get_widget("entry_description", entry_description);
      assert(entry_description != NULL);
      if(entry_description) 
	entry_description->set_text(gate_template->get_description());

      refXml->get_widget("combobox_logic_class", combobox_logic_class);
      assert(combobox_logic_class != NULL);
      if(combobox_logic_class) {
	refListStore_lclass = Gtk::ListStore::create(lclass_model_columns);
	combobox_logic_class->set_model(refListStore_lclass);
	insert_logic_classes();
	combobox_logic_class->pack_start(lclass_model_columns.m_col_descr);
	type_children children = refListStore_lclass->children();
	for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
	  Gtk::TreeModel::Row row = *iter;
	  if(row[lclass_model_columns.m_col_ident] == gate_template->get_logic_class())
	    combobox_logic_class->set_active(iter);
	}
      }

      /*
       * page 2
       */
      
      refXml->get_widget("combobox_lang", combobox_lang);
      assert(combobox_lang != NULL);
      if(combobox_lang != NULL) {
	for(GateTemplate::implementation_iter iter = gate_template->implementations_begin();
	    iter != gate_template->implementations_end(); ++iter)
	  code_text[iter->first] = iter->second;
	combobox_lang->set_active(TEXT);
      }
      

      refXml->get_widget("generate_code_button", codegen_button);
      assert(codegen_button != NULL);
      if(codegen_button) {
	codegen_button->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_codegen_button_clicked));
	codegen_button->set_sensitive(false);
      }

      refXml->get_widget("code_textview", code_textview);
      assert(code_textview != NULL);
      if(code_textview) {
	code_textview->get_buffer()->set_text(code_text[GateTemplate::TEXT]);
	code_textview->get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &GateConfigWin::on_code_changed));
      }


      combobox_lang->signal_changed().connect(sigc::mem_fun(*this, &GateConfigWin::on_language_changed));


  }
}

GateConfigWin::~GateConfigWin() {
}

void GateConfigWin::insert_logic_classes() {

  /** @todo This list of logic classes is hardcoded and that way not flexibile. 
      In case this feature is really used, it should be changed.  */
  append_logic_class("undefined");
  append_logic_class("inverter");
  append_logic_class("tristate-inverter");
  append_logic_class("nand");
  append_logic_class("nor");
  append_logic_class("and");
  append_logic_class("or");
  append_logic_class("xor");
  append_logic_class("xnor");
  append_logic_class("buffer");
  append_logic_class("latch");
  append_logic_class("flipflop");
  append_logic_class("ao", "and-or");
  append_logic_class("aoi", "and-or-inverter");
  append_logic_class("oa", "or-and");
  append_logic_class("oai", "or-and-inverter");
  append_logic_class("isolation");
  append_logic_class("half-adder");
  append_logic_class("full-adder");
  append_logic_class("mux");
  append_logic_class("demux");
}

void GateConfigWin::append_logic_class(Glib::ustring const& ident, Glib::ustring const& descr) {
  std::cout << "add " << ident << std::endl;
  Gtk::TreeModel::Row row = *(refListStore_lclass->append());
  row[lclass_model_columns.m_col_ident] = ident;
  row[lclass_model_columns.m_col_descr] = descr;
}

void GateConfigWin::append_logic_class(Glib::ustring const& ident) {
  append_logic_class(ident, ident);
}

GateTemplate::IMPLEMENTATION_TYPE  GateConfigWin::lang_idx_to_impl(int idx) {
  switch(idx) {
  case 0: return GateTemplate::TEXT;
  case 1: return GateTemplate::VHDL;
  case 2: return GateTemplate::VHDL_TESTBENCH;
  case 3: return GateTemplate::VERILOG;
  case 4: return GateTemplate::VERILOG_TESTBENCH;
  default: return GateTemplate::UNDEFINED;
  }
}

void GateConfigWin::on_code_changed() {
  unsigned int idx = combobox_lang->get_active_row_number();
  code_text[lang_idx_to_impl(idx)] = code_textview->get_buffer()->get_text();
}

void GateConfigWin::on_language_changed() {
  unsigned int idx = combobox_lang->get_active_row_number();
  code_textview->get_buffer()->set_text(code_text[lang_idx_to_impl(idx)]);
  if(lang_idx_to_impl(idx) == GateTemplate::VHDL)
    codegen_button->set_sensitive(true);
  else
    codegen_button->set_sensitive(false);
}

void GateConfigWin::on_codegen_button_clicked() {

  if(code_textview->get_buffer()->size() > 0) {
    Gtk::MessageDialog dialog("Are you sure you want to replace the code?", 
			      true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Warning");      
    if(dialog.run() == Gtk::RESPONSE_NO) return;
  }

  CodeTemplateGenerator_shptr codegen;

  int idx = combobox_lang->get_active_row_number();
  if(lang_idx_to_impl(idx) == GateTemplate::VHDL) {
    debug(TM, "generate vhdl");
    codegen = CodeTemplateGenerator_shptr(new VHDLCodeTemplateGenerator(entry_short_name->get_text().c_str(),
									entry_description->get_text().c_str()));
  }
  else {
    return;
  }

  type_children children = refListStore_ports->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring port_name = row[port_model_columns.m_col_text];
    codegen->add_port(port_name, row[port_model_columns.m_col_inport]);
  }
  code_textview->get_buffer()->set_text(codegen->generate());
}


bool GateConfigWin::run() {
  pDialog->run();
  return result;
}

void GateConfigWin::on_ok_button_clicked() {
  Glib::ustring name_str;
  object_id_t id;
  GateTemplatePort::PORT_TYPE port_type;

  // get text content and set it to the gate template
  gate_template->set_name(entry_short_name->get_text().c_str());
  gate_template->set_description(entry_description->get_text().c_str());

  // get ports

  type_children children = refListStore_ports->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    name_str = row[port_model_columns.m_col_text];
    id = row[port_model_columns.m_col_id];
    
    if(row[port_model_columns.m_col_inport] == true &&
       row[port_model_columns.m_col_outport] == true) port_type = GateTemplatePort::PORT_TYPE_TRISTATE;
    else if(row[port_model_columns.m_col_inport] == true) port_type = GateTemplatePort::PORT_TYPE_IN;
    else if(row[port_model_columns.m_col_outport] == true) port_type = GateTemplatePort::PORT_TYPE_OUT;
    else port_type = GateTemplatePort::PORT_TYPE_UNDEFINED;

    if(id == 0) {
      // create a new template port
      GateTemplatePort_shptr new_template_port(new GateTemplatePort(port_type));

      new_template_port->set_object_id(lmodel->get_new_object_id());
      new_template_port->set_name(name_str);

      lmodel->add_template_port_to_gate_template(gate_template, new_template_port);
    }
    else {
      GateTemplatePort_shptr tmpl_port = gate_template->get_template_port(id);
      tmpl_port->set_name(name_str);
      tmpl_port->set_port_type(port_type);
      original_ports.remove(tmpl_port);      
    }


  }


  // remaining entries in original_ports are not in list store. we can
  // remove them from the logic model
  std::list<GateTemplatePort_shptr>::iterator i;
  for(i = original_ports.begin(); i != original_ports.end(); ++i) {
    GateTemplatePort_shptr tmpl_port = *i;
    debug(TM, "remove port from templates / gates with id=%d", 
	  tmpl_port->get_object_id());

    lmodel->remove_template_port_from_gate_template(gate_template, tmpl_port);
  }


  Gdk::Color fill_color = colorbutton_fill_color->get_color();
  Gdk::Color frame_color = colorbutton_frame_color->get_color();

  gate_template->set_fill_color(MERGE_CHANNELS(fill_color.get_red() >> 8,
					       fill_color.get_green() >> 8,
					       fill_color.get_blue() >> 8,
					       colorbutton_fill_color->get_alpha() >> 8));
  gate_template->set_frame_color(MERGE_CHANNELS(frame_color.get_red() >> 8,
						frame_color.get_green() >> 8,
						frame_color.get_blue() >> 8,
						colorbutton_frame_color->get_alpha() >> 8));


  
  Gtk::TreeModel::iterator iter = combobox_logic_class->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
      Glib::ustring cl_name = row[lclass_model_columns.m_col_ident];
      gate_template->set_logic_class(cl_name);
    }
  }


  BOOST_FOREACH(code_text_map_type::value_type &p, code_text)
    gate_template->set_implementation(p.first, p.second);

  pDialog->hide();
  result = true;
}

void GateConfigWin::on_cancel_button_clicked() {
  pDialog->hide();
  result = false;
}

void GateConfigWin::on_port_add_button_clicked() {

  
  Gtk::TreeNodeChildren::size_type children_size = refListStore_ports->children().size();

  Gtk::TreeModel::Row row = *(refListStore_ports->append()); 
  row[port_model_columns.m_col_id] = 0;

  if(children_size == 0) {
    row[port_model_columns.m_col_text] = "y";
    row[port_model_columns.m_col_outport] = true;
  }
  else {
    if(children_size - 1 < 'q' - 'a') {
      unsigned char symbol = 'a' + children_size - 1;
      boost::format f("%1%");
      f % symbol;
      row[port_model_columns.m_col_text] = f.str();
    }
    else
      row[port_model_columns.m_col_text] = "click to edit";

    row[port_model_columns.m_col_inport] = true;
  }

}

void GateConfigWin::on_port_remove_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView_ports->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) refListStore_ports->erase(iter);
  }
}

