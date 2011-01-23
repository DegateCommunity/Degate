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

#include "GateConfigWin.h"
#include "GladeFileLoader.h"
#include <VHDLCodeTemplateGenerator.h>
#include <VHDLTBCodeTemplateGenerator.h>
#include <VerilogCodeTemplateGenerator.h>
#include <RenderWindow.h>

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
			     GateTemplate_shptr gate_template,
			     degate::color_t default_frame_col, 
			     degate::color_t default_fill_col) :
  GladeFileLoader("gate_create.glade", "gate_create_dialog"),
  _default_frame_col(default_frame_col),
  _default_fill_col(default_fill_col),
  render_window_transistor(editor_transistor, false, false),
  render_window_m1(editor_m1, false, false),
  render_window_transistor_m1(editor_transistor_m1, false, false) {

  this->lmodel = lmodel;
  this->gate_template = gate_template;

  this->parent = parent;

  if(get_dialog()) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;

    // connect signals
    get_widget("cancel_button", pButton);
    assert(pButton != NULL);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_cancel_button_clicked));

    get_widget("ok_button", ok_button);
    assert(ok_button != NULL);
    if(ok_button)
      ok_button->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_ok_button_clicked) );


    get_widget("port_add_button", pButton);
    assert(pButton != NULL);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_port_add_button_clicked) );

    get_widget("port_remove_button", pButton);
    assert(pButton != NULL);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_port_remove_button_clicked) );


    refListStore_ports = Gtk::ListStore::create(port_model_columns);

    get_widget("treeview_ports", pTreeView_ports);
    assert(pTreeView_ports != NULL);
    if(pTreeView_ports) {
      pTreeView_ports->set_model(refListStore_ports);
      pTreeView_ports->append_column("Port ID", port_model_columns.m_col_id);
      pTreeView_ports->append_column_editable("Port Name", port_model_columns.m_col_name);
      pTreeView_ports->append_column_editable("Port Description", port_model_columns.m_col_description);
      pTreeView_ports->append_column_editable("In", port_model_columns.m_col_inport);
      pTreeView_ports->append_column_editable("Out", port_model_columns.m_col_outport);
    }
    
    
    get_widget("colorbutton_fill_color", colorbutton_fill_color);
    assert(colorbutton_fill_color != NULL);
    if(colorbutton_fill_color != NULL) {
      set_color_button(colorbutton_fill_color,  
		       gate_template->has_fill_color() ? 
		       gate_template->get_fill_color() : _default_fill_col);
    }
    
    get_widget("colorbutton_frame_color", colorbutton_frame_color);
    assert(colorbutton_frame_color != NULL);
    if(colorbutton_frame_color != NULL) {
      set_color_button(colorbutton_frame_color,  
		       gate_template->has_frame_color() ? 
		       gate_template->get_frame_color() : _default_frame_col);
    }

    get_widget("reset_frame_color_button", pButton);
    assert(pButton != NULL);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_reset_frame_color_clicked) );

    get_widget("reset_fill_color_button", pButton);
    assert(pButton != NULL);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_reset_fill_color_clicked) );


    for(GateTemplate::port_iterator iter = gate_template->ports_begin();
	iter != gate_template->ports_end();
	++iter) {
      
      Gtk::TreeModel::Row row = *(refListStore_ports->append());
      
      GateTemplatePort_shptr tmpl_port = *iter;
      
      debug(TM, "PORT NAME: [%s]", tmpl_port->get_name().c_str());
      
      row[port_model_columns.m_col_inport] = tmpl_port->is_inport();
      row[port_model_columns.m_col_outport] = tmpl_port->is_outport();
      row[port_model_columns.m_col_name] = tmpl_port->get_name();
      row[port_model_columns.m_col_description] = tmpl_port->get_description();
      row[port_model_columns.m_col_id] = tmpl_port->get_object_id();
      
      original_ports.push_back(tmpl_port);
    }
    
    get_widget("entry_short_name", entry_short_name);
    assert(entry_short_name != NULL);
    if(entry_short_name) {
      entry_short_name->set_text(gate_template->get_name());
      entry_short_name->signal_changed().connect(sigc::mem_fun(*this,
							       &GateConfigWin::on_entry_short_name_changed) );
    }
    
    get_widget("entry_description", entry_description);
    assert(entry_description != NULL);
    if(entry_description)
      entry_description->set_text(gate_template->get_description());
    
    get_widget("combobox_logic_class", combobox_logic_class);
    assert(combobox_logic_class != NULL);
    if(combobox_logic_class) {
      refListStore_lclass = Gtk::ListStore::create(lclass_model_columns);
      combobox_logic_class->set_model(refListStore_lclass);
      insert_logic_classes();
      combobox_logic_class->pack_start(lclass_model_columns.m_col_descr);
      selected_logic_class = gate_template->get_logic_class();
      type_children children = refListStore_lclass->children();
      for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
	Gtk::TreeModel::Row row = *iter;
	if(row[lclass_model_columns.m_col_ident] == selected_logic_class)
	  combobox_logic_class->set_active(iter);
      }
      
      combobox_logic_class->signal_changed().connect(sigc::mem_fun(*this, &GateConfigWin::on_logic_class_changed));
    }
    
    /*
     * page 2
     */
    
    get_widget("combobox_lang", combobox_lang);
    assert(combobox_lang != NULL);
    if(combobox_lang != NULL) {
      for(GateTemplate::implementation_iter iter = gate_template->implementations_begin();
	  iter != gate_template->implementations_end(); ++iter)
	code_text[iter->first] = iter->second;
      combobox_lang->set_active(TEXT);
    }
    
    
    get_widget("generate_code_button", codegen_button);
    assert(codegen_button != NULL);
    if(codegen_button) {
      codegen_button->signal_clicked().connect(sigc::mem_fun(*this, &GateConfigWin::on_codegen_button_clicked));
      codegen_button->set_sensitive(false);
    }
    
    get_widget("code_textview", code_textview);
    assert(code_textview != NULL);
    if(code_textview) {
      code_textview->modify_font(Pango::FontDescription("courier"));
      
      code_textview->get_buffer()->set_text(code_text[GateTemplate::TEXT]);
      code_textview->get_buffer()->signal_changed().connect(sigc::mem_fun(*this, &GateConfigWin::on_code_changed));
    }
    
    
    combobox_lang->signal_changed().connect(sigc::mem_fun(*this, &GateConfigWin::on_language_changed));
    
    /*
     * page 3
     */
    Gtk::HBox * hbox_renderer = get_widget<Gtk::HBox>("hbox_renderer");
    assert(hbox_renderer != NULL);
    if(hbox_renderer) {
      
      hbox_renderer->pack_start(render_window_m1, Gtk::PACK_EXPAND_WIDGET);
      hbox_renderer->pack_start(render_window_transistor_m1, Gtk::PACK_EXPAND_WIDGET);
      hbox_renderer->pack_start(render_window_transistor, Gtk::PACK_EXPAND_WIDGET);
      hbox_renderer->show_all();
      
      editor_transistor.set_gate_template(gate_template);
      editor_transistor.add_layer_type(Layer::TRANSISTOR);
      
      editor_transistor_m1.set_gate_template(gate_template);
      editor_transistor_m1.add_layer_type(Layer::TRANSISTOR);
      editor_transistor_m1.add_layer_type(Layer::LOGIC);
      
      editor_m1.set_gate_template(gate_template);
      editor_m1.add_layer_type(Layer::LOGIC);
    }

  }
}

GateConfigWin::~GateConfigWin() {
}

void GateConfigWin::set_color_button(Gtk::ColorButton * button, degate::color_t col) {
  Gdk::Color c;
  c.set_red(MASK_R(col) << 8);
  c.set_green(MASK_G(col) << 8);
  c.set_blue(MASK_B(col) << 8);
  button->set_alpha(MASK_A(col) << 8);
  button->set_color(c);
}

void GateConfigWin::insert_logic_classes() {

  /** @todo This list of logic classes is hardcoded and that way not flexibile.
      In case this feature is really used, it should be changed.  */
  append_logic_class("undefined");
  append_logic_class("inverter");
  append_logic_class("tristate-inverter");
  append_logic_class("tristate-inverter-lo-active");
  append_logic_class("tristate-inverter-hi-active");
  append_logic_class("nand");
  append_logic_class("nor");
  append_logic_class("and");
  append_logic_class("or");
  append_logic_class("xor");
  append_logic_class("xnor");
  append_logic_class("buffer");
  append_logic_class("latch-generic", "latch (generic, transparent)");
  append_logic_class("latch-sync-enable", "latch (generic, with synchronous enable)");
  append_logic_class("latch-async-enable", "latch (generic, with asynchronous enable)");
  append_logic_class("flipflop", "flipflop (generic)");
  append_logic_class("flipflop-sync-rst", "flipflop (synchronous reset, edge-triggert)");
  append_logic_class("flipflop-async-rst", "flipflop (asynchronous reset, edge-triggert)");
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
  if(lang_idx_to_impl(idx) == GateTemplate::VHDL ||
     lang_idx_to_impl(idx) == GateTemplate::VHDL_TESTBENCH ||
     lang_idx_to_impl(idx) == GateTemplate::VERILOG)
    codegen_button->set_sensitive(true);
  else
    codegen_button->set_sensitive(false);
}

void GateConfigWin::on_codegen_button_clicked() {

  CodeTemplateGenerator_shptr codegen;
  int idx = combobox_lang->get_active_row_number();

  if(code_textview->get_buffer()->size() > 0) {
    Gtk::MessageDialog dialog("Are you sure you want to replace the code?",
			      true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Warning");
    if(dialog.run() == Gtk::RESPONSE_NO) return;
  }

  if(lang_idx_to_impl(idx) == GateTemplate::UNDEFINED) {
    Gtk::MessageDialog dialog("If you define a logic class under the tab 'entity', "
			      "you can auto-generate more specific code stubs.",
			      true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
    dialog.set_title("Hint");
    dialog.run();
  }


  if(lang_idx_to_impl(idx) == GateTemplate::VHDL) {
    codegen = CodeTemplateGenerator_shptr(new VHDLCodeTemplateGenerator(entry_short_name->get_text().c_str(),
									entry_description->get_text().c_str(),
									selected_logic_class));
  }
  else if(lang_idx_to_impl(idx) == GateTemplate::VHDL_TESTBENCH) {
    codegen = CodeTemplateGenerator_shptr(new VHDLTBCodeTemplateGenerator(entry_short_name->get_text().c_str(),
									  entry_description->get_text().c_str(),
									  selected_logic_class));
  }
  else if(lang_idx_to_impl(idx) == GateTemplate::VERILOG) {
    codegen = CodeTemplateGenerator_shptr(new VerilogCodeTemplateGenerator(entry_short_name->get_text().c_str(),
									   entry_description->get_text().c_str(),
									   selected_logic_class));
  }
  else {
    return;
  }

  type_children children = refListStore_ports->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring port_name = row[port_model_columns.m_col_name];
    codegen->add_port(port_name, row[port_model_columns.m_col_inport]);
  }
  code_textview->get_buffer()->set_text(codegen->generate());
}


bool GateConfigWin::run() {
  get_dialog()->run();
  return result;
}

void GateConfigWin::on_ok_button_clicked() {
  Glib::ustring name_str, descr_str;
  object_id_t id;
  GateTemplatePort::PORT_TYPE port_type;

  // get text content and set it to the gate template
  gate_template->set_name(entry_short_name->get_text().c_str());
  gate_template->set_description(entry_description->get_text().c_str());

  // get ports

  type_children children = refListStore_ports->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    name_str = row[port_model_columns.m_col_name];
    descr_str = row[port_model_columns.m_col_name];
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
      new_template_port->set_description(descr_str);

      lmodel->add_template_port_to_gate_template(gate_template, new_template_port);
    }
    else {
      GateTemplatePort_shptr tmpl_port = gate_template->get_template_port(id);
      tmpl_port->set_name(name_str);
      tmpl_port->set_description(descr_str);
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


  gate_template->set_logic_class(selected_logic_class);


  BOOST_FOREACH(code_text_map_type::value_type &p, code_text)
    gate_template->set_implementation(p.first, p.second);

  get_dialog()->hide();
  result = true;
}

void GateConfigWin::on_logic_class_changed() {
  Gtk::TreeModel::iterator iter = combobox_logic_class->get_active();
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row) {
      Glib::ustring s = row[lclass_model_columns.m_col_ident];
      selected_logic_class = s;
    }
  }
}

void GateConfigWin::on_cancel_button_clicked() {
  get_dialog()->hide();
  result = false;
}

void GateConfigWin::on_port_add_button_clicked() {


  Gtk::TreeNodeChildren::size_type children_size = refListStore_ports->children().size();

  Gtk::TreeModel::Row row = *(refListStore_ports->append());
  row[port_model_columns.m_col_id] = 0;

  if(children_size == 0) {
    row[port_model_columns.m_col_name] = "y";
    row[port_model_columns.m_col_outport] = true;
  }
  else {
    if(children_size - 1 < 'q' - 'a') {
      unsigned char symbol = 'a' + children_size - 1;
      boost::format f("%1%");
      f % symbol;
      row[port_model_columns.m_col_name] = f.str();
    }
    else
      row[port_model_columns.m_col_name] = "click to edit";

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


void GateConfigWin::on_entry_short_name_changed() {

  GateLibrary_shptr lib = lmodel->get_gate_library();
  Glib::ustring const & s = entry_short_name->get_text();

  if(!s.empty() && !lib->is_name_in_use(s.c_str())) {

    ok_button->set_sensitive(true);
  }
  else
    ok_button->set_sensitive(false);

}


void GateConfigWin::on_reset_frame_color_clicked() {
  if(colorbutton_frame_color != NULL) {
	set_color_button(colorbutton_frame_color, _default_frame_col);
  }
}

void GateConfigWin::on_reset_fill_color_clicked() {
  if(colorbutton_fill_color != NULL) {
    set_color_button(colorbutton_fill_color, _default_fill_col);
  }
}
