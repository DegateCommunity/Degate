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

#include "LayerConfigWin.h"
#include "GladeFileLoader.h"
#include "AppHelper.h"
#include "InProgressWin.h"

#include <LogicModelHelper.h>

#include <gdkmm/window.h>
#include <gtkmm/stock.h>
#include <libglademm.h>

#include <stdlib.h>
#include <iostream>

using namespace degate;


LayerConfigWin::LayerConfigWin(Gtk::Window * parent,
			       LogicModel_shptr lmodel,
			       std::string const& project_dir) :
  GladeFileLoader("layer_config.glade", "layer_config_dialog") {

  this->parent = parent;
  this->lmodel = lmodel;
  this->project_dir = project_dir;

  if(pDialog) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals

    refXml->get_widget("cancel_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &LayerConfigWin::on_cancel_button_clicked));
    
    refXml->get_widget("ok_button", p_ok_button);
    if(p_ok_button)
      p_ok_button->signal_clicked().connect(sigc::mem_fun(*this, &LayerConfigWin::on_ok_button_clicked) );


    
    refXml->get_widget("add_button", pButton);
    if(pButton) 
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &LayerConfigWin::on_add_button_clicked) );
    
    refXml->get_widget("remove_button", p_remove_button);
    if(p_remove_button) {
      p_remove_button->signal_clicked().connect(sigc::mem_fun(*this, &LayerConfigWin::on_remove_button_clicked) );
      p_remove_button->set_sensitive(false);
    }

    refXml->get_widget("bg_file_button", p_bg_file_button);
    if(p_bg_file_button) {
      p_bg_file_button->signal_clicked().connect(sigc::mem_fun(*this, &LayerConfigWin::on_bg_file_button_clicked) );
      p_bg_file_button->set_sensitive(false);
    }

    refXml->get_widget("clear_bg_button", p_clear_bg_button);
    if(p_clear_bg_button) {
      p_clear_bg_button->signal_clicked().connect(sigc::mem_fun(*this, &LayerConfigWin::on_clear_bg_button_clicked) );
      p_clear_bg_button->set_sensitive(false);
    }

    // create and fill combo box model
    m_refTreeModelCombo = Gtk::ListStore::create(m_ColumnsCombo);
    Gtk::TreeModel::Row combo_row = *(m_refTreeModelCombo->append());
    combo_row[m_ColumnsCombo.m_col_choice] = "undefined";
    combo_row = *(m_refTreeModelCombo->append());
    combo_row[m_ColumnsCombo.m_col_choice] = "transistor";
    combo_row = *(m_refTreeModelCombo->append());
    combo_row[m_ColumnsCombo.m_col_choice] = "logic";
    combo_row = *(m_refTreeModelCombo->append());
    combo_row[m_ColumnsCombo.m_col_choice] = "metal";
    
    
   
    refListStore_layers = Gtk::ListStore::create(m_Columns);
      
    refXml->get_widget("treeview_layers", pTreeView_layers);
    if(pTreeView_layers) {
      pTreeView_layers->set_model(refListStore_layers);
      pTreeView_layers->append_column("Old #", m_Columns.m_col_old_position);
      pTreeView_layers->append_column("New #", m_Columns.m_col_new_position);
      pTreeView_layers->append_column_editable("Enabled", m_Columns.m_col_enabled);
      //pTreeView_layers->append_column_editable("Layer type", m_Columns.m_col_layer_type);

      // Create a Combo CellRenderer, instead of the default Text CellRenderer:
      Gtk::TreeView::Column* pColumn = Gtk::manage( new Gtk::TreeView::Column("Layer Type") );

      Gtk::CellRendererCombo* pRenderer = Gtk::manage(new Gtk::CellRendererCombo);
      pColumn->pack_start(*pRenderer);
      pTreeView_layers->append_column(*pColumn);

      // Make this View column represent the m_col_itemchosen model column:
      pColumn->add_attribute(pRenderer->property_text(), m_Columns.m_col_layer_type_chosen);

      pRenderer->property_model() = m_refTreeModelCombo;
      pRenderer->property_text_column() = 0; //
      pRenderer->property_editable() = true;
      pRenderer->signal_edited().connect( sigc::mem_fun(*this,
							&LayerConfigWin::on_cellrenderer_choice_edited) );

      pTreeView_layers->append_column_editable("Layer Description", m_Columns.m_col_description);
      pTreeView_layers->append_column("New Background Image", m_Columns.m_col_filename);
    }
      
    
    for(LogicModel::layer_collection::iterator iter = lmodel->layers_begin();
	iter != lmodel->layers_end(); ++iter) {
      Layer_shptr layer = *iter;

      Gtk::TreeModel::Row row = *(refListStore_layers->append());

      row[m_Columns.m_col_old_position] = layer->get_layer_pos();
      row[m_Columns.m_col_new_position] = layer->get_layer_pos();
      row[m_Columns.m_col_enabled] = layer->is_enabled();
      row[m_Columns.m_col_exists] = true;
      row[m_Columns.m_col_layer_type_chosen] = layer->get_layer_type_as_string();
      row[m_Columns.m_col_layer_type_choices] = m_refTreeModelCombo;
      row[m_Columns.m_col_filename] = "";
    }

    // signal
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView_layers->get_selection();
    refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &LayerConfigWin::on_selection_changed));

    pTreeView_layers->set_reorderable();
    pTreeView_layers->signal_drag_end().connect(sigc::mem_fun(*this, &LayerConfigWin::drag_end_handler));
  }
}

LayerConfigWin::~LayerConfigWin() {
}


void LayerConfigWin::on_cellrenderer_choice_edited(const Glib::ustring& path_string, 
						   const Glib::ustring& new_text) {

  Gtk::TreePath path(path_string);

  //Get the row from the path:
  Gtk::TreeModel::iterator iter = refListStore_layers->get_iter(path);
  if(iter) {
    //Store the user's new text in the model:
    Gtk::TreeRow row = *iter;
    
    try{
      std::cout << "Current: " << row[m_Columns.m_col_layer_type_chosen] << std::endl;
      std::cout << "New: " << new_text << std::endl;
      
      Layer::get_layer_type_from_string(new_text); // try to parse
      row[m_Columns.m_col_layer_type_chosen] = new_text;
    }
    catch(DegateRuntimeException const& ex) { }

  }

}


bool LayerConfigWin::run() {
  pDialog->run();
  debug(TM, "return from run()");
  return result;
}

void LayerConfigWin::on_ok_button_clicked() {
  result = true;

  // update data structures
  
  std::vector<Layer_shptr> layers;
  type_children children = refListStore_layers->children();

  // first: check if we have to import a background image
  bool need_progress_bar = false;
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring filename = row[m_Columns.m_col_filename];
    if(filename != "") need_progress_bar = true;
  }

  if(need_progress_bar) {
    debug(TM, "progress bar");
    ipWin = new InProgressWin(parent, "Importing", 
			      "Please wait while importing background image and calculating the prescaled images.");
    ipWin->show();
  }
  else {
    debug(TM, "no progress bar");
    ipWin = NULL;
  }

  image_list images_to_load;

  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    
    int old_pos = row[m_Columns.m_col_old_position];
    Layer_shptr layer;
    if(old_pos == -1) // create  new layer
      layer = Layer_shptr(new Layer(BoundingBox(lmodel->get_width(),
						lmodel->get_height())));
    else
      layer = lmodel->get_layer(old_pos);

    layer->set_enabled(row[m_Columns.m_col_enabled]);
    // will not throw an exception: already check on edit

    Glib::ustring layer_type = row[m_Columns.m_col_layer_type_chosen];
    Glib::ustring descr = row[m_Columns.m_col_description];

    layer->set_layer_type(Layer::get_layer_type_from_string(layer_type));
    layer->set_description(descr);
    layer->set_layer_pos(row[m_Columns.m_col_new_position]);
    layers.push_back(layer);

    Glib::ustring filename = row[m_Columns.m_col_filename];
    if(filename != "") {
      images_to_load.push_back(std::make_pair(layer, filename));
      row[m_Columns.m_col_filename] = "";
    }

  }

  lmodel->set_layers(layers);

  if(need_progress_bar) {
    _signal_bg_import_finished_.connect(sigc::mem_fun(*this, &LayerConfigWin::_on_background_import_finished));
    Glib::Thread::create(sigc::bind<image_list>(sigc::mem_fun(*this, &LayerConfigWin::background_import_thread), 
						images_to_load), false);
  }
  else {
    debug(TM, "closing window");
    pDialog->hide();
  }

}


void LayerConfigWin::_on_background_import_finished() {
  if(ipWin != NULL) {
    ipWin->close();
    delete ipWin;
    ipWin = NULL;
  }
  pDialog->hide();
}

void LayerConfigWin::background_import_thread(image_list l) {

  type_children children = refListStore_layers->children();

  for(image_list::iterator iter = l.begin(); iter != l.end(); ++iter) {
    
      debug(TM, "Load background image.");
      assert(iter->first != NULL);
      load_background_image(iter->first, project_dir, iter->second);
      debug(TM, "Background image loaded.");
  }

  _signal_bg_import_finished_(); // internal signal
  signal_on_background_import_finished_(); // signal for third parties
}

void LayerConfigWin::on_cancel_button_clicked() {
  pDialog->hide();
  result = false;
}

void LayerConfigWin::on_add_button_clicked() {

  Gtk::TreeModel::Row row = *(refListStore_layers->append());

  row[m_Columns.m_col_old_position] = -1;
  row[m_Columns.m_col_new_position] = -1;
  row[m_Columns.m_col_enabled] = true;
  row[m_Columns.m_col_exists] = false;
  row[m_Columns.m_col_layer_type_chosen] = Layer::get_layer_type_as_string(Layer::UNDEFINED);
  row[m_Columns.m_col_layer_type_choices] = m_refTreeModelCombo;
  row[m_Columns.m_col_filename] = "";
  update_new_positions();
}


void LayerConfigWin::on_remove_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView_layers->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      refListStore_layers->erase(iter);
      update_new_positions();
    }
  }
}



void LayerConfigWin::on_bg_file_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView_layers->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;

      Gtk::FileChooserDialog dialog("Please choose a background image");
      dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
      dialog.add_button("Select", Gtk::RESPONSE_OK);
      add_image_file_filter_to_file_chooser(dialog);

      int result = dialog.run();
      dialog.hide();

      if(result == Gtk::RESPONSE_OK) {
	row[m_Columns.m_col_filename] = dialog.get_filename();
	p_clear_bg_button->set_sensitive(true);
      }

    }
  }
}

void LayerConfigWin::on_clear_bg_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView_layers->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;
      row[m_Columns.m_col_filename] = "";
      p_clear_bg_button->set_sensitive(false);
    }
  }
}


void LayerConfigWin::drag_end_handler(const Glib::RefPtr<Gdk::DragContext> & context) {
  update_new_positions();
}

void LayerConfigWin::update_new_positions() {

  int pos = 0;
  type_children children = refListStore_layers->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter, pos++) {
    Gtk::TreeModel::Row row = *iter;
    
    row[m_Columns.m_col_new_position] = pos;
  }
}

void LayerConfigWin::on_selection_changed() {
  debug(TM, "selection changed");
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView_layers->get_selection();


  bool new_state = refTreeSelection->count_selected_rows() == 0 ? false : true;

  p_remove_button->set_sensitive(new_state);
  p_bg_file_button->set_sensitive(new_state);
  
  
  p_clear_bg_button->set_sensitive(new_state);

  if(new_state == true && refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter;
      if(row[m_Columns.m_col_filename] != "") 
	p_clear_bg_button->set_sensitive(true);
      else p_clear_bg_button->set_sensitive(false);
    }
  }

  check_at_least_one_layer_enabled();
}


void LayerConfigWin::check_at_least_one_layer_enabled() {
  bool at_least_one_layer_active = false;
  type_children children = refListStore_layers->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;
    if(row[m_Columns.m_col_enabled]) at_least_one_layer_active = true;
  }

  p_ok_button->set_sensitive(at_least_one_layer_active);
  if(at_least_one_layer_active == false) {
    Gtk::MessageDialog dialog("At least one layer must be enabled.", true, Gtk::MESSAGE_ERROR);
    dialog.set_title("Error");
    dialog.run();
  }
}

Glib::Dispatcher& LayerConfigWin::signal_on_background_import_finished() {
  return signal_on_background_import_finished_;
}
