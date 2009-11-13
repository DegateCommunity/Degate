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

#include "GridConfigWin.h"

#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <set>

using namespace degate;

GridConfigWin::GridConfigWin(Gtk::Window *_parent, 
			     RegularGrid_shptr _regular_horizontal_grid,
			     RegularGrid_shptr _regular_vertical_grid,
			     IrregularGrid_shptr _irregular_horizontal_grid,
			     IrregularGrid_shptr _irregular_vertical_grid) : 
  GladeFileLoader("grid_config.glade", "grid_config_dialog"),
  parent(_parent),
  regular_horizontal_grid(_regular_horizontal_grid),
  regular_vertical_grid(_regular_vertical_grid),
  irregular_horizontal_grid(_irregular_horizontal_grid),
  irregular_vertical_grid(_irregular_vertical_grid) {

  char tmp[50];

  if(pDialog) {

    debug(TM, "Setup widgets for the GridConfigWin.");
    pDialog->set_transient_for(*parent);

    // connect signals
    
    refXml->get_widget("close_button", p_close_button);
    assert(p_close_button != NULL);
    if(p_close_button != NULL) {
      p_close_button->grab_focus();
      p_close_button->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_close_button_clicked) );
    }


    refXml->get_widget("regular_grid_radiobutton", p_regular_grid_rbutton);
    refXml->get_widget("irregular_grid_radiobutton", p_irregular_grid_rbutton);
    assert(p_regular_grid_rbutton != NULL && p_irregular_grid_rbutton != NULL);
    if(p_regular_grid_rbutton != NULL && p_irregular_grid_rbutton != NULL) {
      // XXX
      if(irregular_horizontal_grid->is_enabled()) p_irregular_grid_rbutton->set_active(true);
      else p_regular_grid_rbutton->set_active(true);

      p_regular_grid_rbutton->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_rgrid_rbutton_clicked) );
      p_irregular_grid_rbutton->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_urgrid_rbutton_clicked) );
    }

    /*
     * regular grid
     */

    debug(TM, "Setup widgets for the regular grid.");

    refXml->get_widget("horizontal_checkbutton", p_horizontal_checkbutton);
    assert(p_horizontal_checkbutton != NULL);
    if(p_horizontal_checkbutton != NULL) {
      p_horizontal_checkbutton->set_active(regular_horizontal_grid->is_enabled());
      p_horizontal_checkbutton->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_horz_checkb_clicked) );
    }

    refXml->get_widget("vertical_checkbutton", p_vertical_checkbutton);
    assert(p_vertical_checkbutton != NULL);
    if(p_vertical_checkbutton != NULL) {
      p_vertical_checkbutton->set_active(regular_vertical_grid->is_enabled());
      p_vertical_checkbutton->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_vert_checkb_clicked) );
    }

    refXml->get_widget("hscale_offset_x", p_scale_offset_x);
    assert(p_scale_offset_x != NULL);
    if(p_scale_offset_x != NULL) {
      p_adj_offset_x = new Gtk::Adjustment(regular_horizontal_grid->get_min(), 0.0, 512.0, 1);
      p_scale_offset_x->set_adjustment(*p_adj_offset_x);
      p_adj_offset_x->signal_value_changed().connect(sigc::mem_fun(*this, &GridConfigWin::on_offset_x_changed));
    }

    refXml->get_widget("hscale_offset_y", p_scale_offset_y);
    assert(p_scale_offset_y != NULL);
    if(p_scale_offset_y != NULL) {
      p_adj_offset_y = new Gtk::Adjustment(regular_vertical_grid->get_min(), 0.0, 512.0, 1);
      p_scale_offset_y->set_adjustment(*p_adj_offset_y);
      p_adj_offset_y->signal_value_changed().connect(sigc::mem_fun(*this, &GridConfigWin::on_offset_y_changed));
    }

    refXml->get_widget("hscale_distance_x", p_scale_dist_x);
    assert(p_scale_dist_x != NULL);
    if(p_scale_dist_x != NULL) {
      p_adj_dist_x = new Gtk::Adjustment(regular_horizontal_grid->get_distance(), 0.0, 512.0, 1);
      p_scale_dist_x->set_adjustment(*p_adj_dist_x);
      p_adj_dist_x->signal_value_changed().connect(sigc::mem_fun(*this, &GridConfigWin::on_dist_x_changed));
    }

    refXml->get_widget("hscale_distance_y", p_scale_dist_y);
    assert(p_scale_dist_y != NULL);
    if(p_scale_dist_y != NULL) {
      p_adj_dist_y = new Gtk::Adjustment(regular_vertical_grid->get_distance(), 0.0, 512.0, 1);
      p_scale_dist_y->set_adjustment(*p_adj_dist_y);
      p_adj_dist_y->signal_value_changed().connect(sigc::mem_fun(*this, &GridConfigWin::on_dist_y_changed));
    }

    refXml->get_widget("entry_distance_x", p_entry_dist_x);
    assert(p_entry_dist_x != NULL);
    if(p_entry_dist_x != NULL) {
      snprintf(tmp, sizeof(tmp), "%f", regular_horizontal_grid->get_distance());
      p_entry_dist_x->set_text(tmp);
      p_entry_dist_x->signal_changed().connect(sigc::mem_fun(*this, &GridConfigWin::on_entry_dist_x_changed));
    }

    refXml->get_widget("entry_distance_y", p_entry_dist_y);
    assert(p_entry_dist_y != NULL);
    if(p_entry_dist_y != NULL) {
      snprintf(tmp, sizeof(tmp), "%f", regular_vertical_grid->get_distance());
      p_entry_dist_y->set_text(tmp);
      p_entry_dist_y->signal_changed().connect(sigc::mem_fun(*this, &GridConfigWin::on_entry_dist_y_changed));
    }

    on_horz_checkb_clicked();
    on_vert_checkb_clicked();

    /*
     * irregular grid - horizontal lines
     */

    debug(TM, "Setup widgets for the irregular grid.");

    refXml->get_widget("uhg_checkbutton", p_uhg_checkbutton);
    assert(p_uhg_checkbutton != NULL);
    if(p_uhg_checkbutton != NULL) {
      p_uhg_checkbutton->set_active(irregular_horizontal_grid->is_enabled());
      p_uhg_checkbutton->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_uhg_checkb_clicked) );
    }

    ref_liststore_uhg = Gtk::ListStore::create(m_columns_uhg);
    refXml->get_widget("uhg_treeview", p_treeview_uhg);
    assert(p_treeview_uhg != NULL);
    if(p_treeview_uhg != NULL) {
      p_treeview_uhg->set_model(ref_liststore_uhg);
      Gtk::TreeView::Column * pColumn;

      Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText()); 
      p_treeview_uhg->append_column("Offset", *pRenderer);
      pColumn = p_treeview_uhg->get_column(0);
      pColumn->add_attribute(*pRenderer, "text", m_columns_uhg.m_col_offset); 
      pRenderer->property_editable() = true;
      pRenderer->signal_edited().connect(sigc::mem_fun(*this, &GridConfigWin::on_uhg_edited));

      pColumn = p_treeview_uhg->get_column(0);
      if(pColumn) pColumn->set_sort_column(m_columns_uhg.m_col_offset);

      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  p_treeview_uhg->get_selection();
      refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
    }

    refXml->get_widget("button_add_uhg", p_button_add_uhg);
    assert(p_button_add_uhg != NULL);
    if(p_button_add_uhg != NULL) {
      p_button_add_uhg->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_button_add_uhg_clicked) );
    }

    refXml->get_widget("button_remove_uhg", p_button_remove_uhg);
    assert(p_button_remove_uhg != NULL);
    if(p_button_remove_uhg != NULL) {
      p_button_remove_uhg->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_button_remove_uhg_clicked) );
    }

    /*
     * irregular grid - vertical lines
     */

    debug(TM, "Setup widgets for the irregular vertical grid.");

    refXml->get_widget("uvg_checkbutton", p_uvg_checkbutton);
    assert(p_uvg_checkbutton != NULL);
    if(p_uvg_checkbutton != NULL) {
      p_uvg_checkbutton->set_active(irregular_vertical_grid->is_enabled());
      p_uvg_checkbutton->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_uvg_checkb_clicked) );
    }

    ref_liststore_uvg = Gtk::ListStore::create(m_columns_uvg);
    refXml->get_widget("uvg_treeview", p_treeview_uvg);
    assert(p_treeview_uvg != NULL);
    if(p_treeview_uvg != NULL) {
      p_treeview_uvg->set_model(ref_liststore_uvg);
      Gtk::TreeView::Column * pColumn;

      Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText()); 
      p_treeview_uvg->append_column("Offset", *pRenderer);
      pColumn = p_treeview_uvg->get_column(0);
      pColumn->add_attribute(*pRenderer, "text", m_columns_uvg.m_col_offset); 
      pRenderer->property_editable() = true;
      pRenderer->signal_edited().connect(sigc::mem_fun(*this, &GridConfigWin::on_uvg_edited));


      pColumn = p_treeview_uvg->get_column(0);
      if(pColumn) pColumn->set_sort_column(m_columns_uvg.m_col_offset);      

      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  p_treeview_uhg->get_selection();
      refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
    }

    refXml->get_widget("button_add_uvg", p_button_add_uvg);
    assert(p_button_add_uvg != NULL);
    if(p_button_add_uvg != NULL) {
      p_button_add_uvg->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_button_add_uvg_clicked) );
    }

    refXml->get_widget("button_remove_uvg", p_button_remove_uvg);
    assert(p_button_remove_uvg != NULL);
    if(p_button_remove_uvg != NULL) {
      p_button_remove_uvg->signal_clicked().connect(sigc::mem_fun(*this, &GridConfigWin::on_button_remove_uvg_clicked) );
    }

    debug(TM, "Set states.");

    update_grid_entries();
    on_uhg_checkb_clicked();
    on_uvg_checkb_clicked();

  }

}

GridConfigWin::~GridConfigWin() {

  delete p_adj_offset_x;
  delete p_adj_offset_y;
  delete p_adj_dist_x;
  delete p_adj_dist_y;
}


void GridConfigWin::update_grid_entries() {
  unsigned int i;
  if(p_treeview_uvg != NULL) {
    ref_liststore_uvg->clear();

    for(IrregularGrid::grid_iter iter = irregular_vertical_grid->begin();
	iter != irregular_vertical_grid->end();
	++iter) {
      Gtk::TreeModel::Row row = *(ref_liststore_uvg->append()); 
      row[m_columns_uvg.m_col_offset] = *iter;
    }
  }

  if(p_treeview_uhg != NULL) {
    ref_liststore_uhg->clear();

    for(IrregularGrid::grid_iter iter = irregular_horizontal_grid->begin();
	iter != irregular_horizontal_grid->end();
	++iter) {
      
      Gtk::TreeModel::Row row = *(ref_liststore_uhg->append()); 
      row[m_columns_uhg.m_col_offset] = *iter;
    }
  }
}

void GridConfigWin::on_rgrid_rbutton_clicked() {
  regular_horizontal_grid->set_enabled();
  regular_vertical_grid->set_enabled();
  irregular_horizontal_grid->set_enabled(false);
  irregular_vertical_grid->set_enabled(false);
}

void GridConfigWin::on_urgrid_rbutton_clicked() {
  irregular_horizontal_grid->set_enabled();
  irregular_vertical_grid->set_enabled();
  regular_horizontal_grid->set_enabled(false);
  regular_vertical_grid->set_enabled(false);
}


void GridConfigWin::on_uhg_edited(const Glib::ustring& path, const Glib::ustring& new_text) {
  //debug(TM, "edited signal path=%s new text=%s", path.c_str(), new_text.c_str());

  Gtk::TreeModel::iterator iter = ref_liststore_uhg->get_iter(path);
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    
    unsigned int new_offset = atol(new_text.c_str());
    row[m_columns_uhg.m_col_offset] = new_offset;
    irregular_horizontal_grid->add_offset(new_offset);
    
    signal_changed_();
  }

}

void GridConfigWin::on_uvg_edited(const Glib::ustring& path, const Glib::ustring& new_text) {
  //debug(TM, "edited signal path=%s new text=%s", path.c_str(), new_text.c_str());

  Gtk::TreeModel::iterator iter = ref_liststore_uvg->get_iter(path);
  if(iter) {
    Gtk::TreeModel::Row row = *iter;
    
    unsigned int new_offset = atol(new_text.c_str());
    row[m_columns_uvg.m_col_offset] = new_offset;
    irregular_vertical_grid->add_offset(new_offset);
  }

}

void GridConfigWin::show() {
  pDialog->show();
}

void GridConfigWin::on_close_button_clicked() {
  pDialog->hide();
  signal_changed_();
}


void GridConfigWin::on_horz_checkb_clicked() {


  if(p_horizontal_checkbutton->get_active()) {
    regular_horizontal_grid->set_enabled(true);
    p_scale_offset_y->set_sensitive(true);
    p_scale_dist_y->set_sensitive(true);
    p_entry_dist_y->set_sensitive(true);
  }
  else {
    regular_horizontal_grid->set_enabled(false);
    p_scale_offset_y->set_sensitive(false);
    p_scale_dist_y->set_sensitive(false);
    p_entry_dist_y->set_sensitive(false);
  }
  signal_changed_();
}

void GridConfigWin::on_vert_checkb_clicked() {

  if(p_vertical_checkbutton->get_active()) {
    regular_vertical_grid->set_enabled(true);
    p_scale_offset_x->set_sensitive(true);
    p_scale_dist_x->set_sensitive(true);
    p_entry_dist_x->set_sensitive(true);
  }
  else {
    regular_vertical_grid->set_enabled(false);
    p_scale_offset_x->set_sensitive(false);
    p_scale_dist_x->set_sensitive(false);
    p_entry_dist_x->set_sensitive(false);
  }
  signal_changed_();
}


void GridConfigWin::on_offset_x_changed() {
  //grid->offset_x = p_adj_offset_x->get_value();

  irregular_vertical_grid->add_offset(p_adj_offset_x->get_value());

  signal_changed_();
}

void GridConfigWin::on_offset_y_changed() {
  //grid->offset_y = p_adj_offset_y->get_value();

  irregular_horizontal_grid->add_offset(p_adj_offset_y->get_value());

  signal_changed_();
}

void GridConfigWin::on_dist_x_changed() {

  regular_vertical_grid->set_distance(p_adj_dist_x->get_value());

  char tmp[50];
  snprintf(tmp, sizeof(tmp), "%f", regular_vertical_grid->get_distance());
  p_entry_dist_x->set_text(tmp);
  signal_changed_();
}

void GridConfigWin::on_dist_y_changed() {
  regular_horizontal_grid->set_distance(p_adj_dist_y->get_value());

  char tmp[50];
  //grid->dist_y = p_adj_dist_y->get_value();
  snprintf(tmp, sizeof(tmp), "%f", regular_horizontal_grid->get_distance());
  p_entry_dist_y->set_text(tmp);
  signal_changed_();
}


void GridConfigWin::on_entry_dist_x_changed() {
  double v = atof(p_entry_dist_x->get_text().c_str());
  if(v < 0 || v == HUGE_VAL) {
    p_entry_dist_x->set_text("0");
    v = 0;
  }

  //grid->dist_x = v;
  regular_vertical_grid->set_distance(v);
  p_adj_dist_x->set_value(v);
  signal_changed_();
}

void GridConfigWin::on_entry_dist_y_changed() {
  double v = atof(p_entry_dist_y->get_text().c_str());
  if(v < 0 || v == HUGE_VAL) {
    p_entry_dist_y->set_text("0");
    v = 0;
  }

  regular_horizontal_grid->set_distance(v);
  //grid->dist_y = v;
  p_adj_dist_y->set_value(v);

  signal_changed_();
}



sigc::signal<void>& GridConfigWin::signal_changed() {
  return signal_changed_;
}


void GridConfigWin::on_uhg_checkb_clicked() {
  //grid->uhg_enabled = p_uhg_checkbutton->get_active() == true ? 1 : 0;

  if(p_uhg_checkbutton->get_active()) {
    irregular_horizontal_grid->set_enabled(true);
    p_button_add_uhg->set_sensitive(true);
    p_button_remove_uhg->set_sensitive(true);
    p_treeview_uhg->set_sensitive(true);
  }
  else {
    irregular_horizontal_grid->set_enabled(false);
    p_button_add_uhg->set_sensitive(false);
    p_button_remove_uhg->set_sensitive(false);
    p_treeview_uhg->set_sensitive(false);
  }
  signal_changed_();

}

void GridConfigWin::on_uvg_checkb_clicked() {
  //grid->uvg_enabled =   == true ? 1 : 0;

  if(p_uvg_checkbutton->get_active()) {
    irregular_vertical_grid->set_enabled(true);
    p_button_add_uvg->set_sensitive(true);
    p_button_remove_uvg->set_sensitive(true);
    p_treeview_uvg->set_sensitive(true);
  }
  else {
    irregular_vertical_grid->set_enabled(false);
    p_button_add_uvg->set_sensitive(false);
    p_button_remove_uvg->set_sensitive(false);
    p_treeview_uvg->set_sensitive(false);
  }
  signal_changed_();

}



void GridConfigWin::on_button_add_uhg_clicked() {
  Gtk::TreeModel::Row row = *(ref_liststore_uhg->append()); 
  row[m_columns_uhg.m_col_offset] = 0;
}

void GridConfigWin::on_button_add_uvg_clicked() {
  Gtk::TreeModel::Row row = *(ref_liststore_uvg->append()); 
  row[m_columns_uvg.m_col_offset] = 0;
}


void GridConfigWin::on_button_remove_uhg_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> ref_tree_selection = p_treeview_uhg->get_selection();
  if(ref_tree_selection) {

    std::vector<Gtk::TreeModel::Path> pathlist = ref_tree_selection->get_selected_rows();

    for(std::vector<Gtk::TreeModel::Path>::reverse_iterator iter = pathlist.rbegin(); 
	iter != pathlist.rend(); ++iter)
      ref_liststore_uhg->erase(ref_tree_selection->get_model()->get_iter (*iter));
    
    irregular_horizontal_grid->clear();
    signal_changed_();
  }
}

void GridConfigWin::on_button_remove_uvg_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> ref_tree_selection =  p_treeview_uvg->get_selection();
  if(ref_tree_selection) {

    std::vector<Gtk::TreeModel::Path> pathlist = ref_tree_selection->get_selected_rows();

    for(std::vector<Gtk::TreeModel::Path>::reverse_iterator iter = pathlist.rbegin(); 
	iter != pathlist.rend(); ++iter)
      ref_liststore_uvg->erase(ref_tree_selection->get_model()->get_iter (*iter));
    
    irregular_vertical_grid->clear();
    signal_changed_();

  }
}
