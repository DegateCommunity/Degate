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

#ifndef __GRIDCONFIGWIN_H__
#define __GRIDCONFIGWIN_H__

#include <gtkmm.h>
#include "Project.h"
#include "GladeFileLoader.h"
#include "RegularGrid.h"
#include "IrregularGrid.h"

class GridConfigWin : private GladeFileLoader {


  class GridConfigModelColumns : public Gtk::TreeModelColumnRecord {
  public:

    GridConfigModelColumns() {
      add(m_col_offset);
    }

    Gtk::TreeModelColumn<unsigned int > m_col_offset;
  };


 public:

  GridConfigWin(Gtk::Window *parent,
		degate::RegularGrid_shptr regular_horizontal_grid,
		degate::RegularGrid_shptr regular_vertical_grid,
		degate::IrregularGrid_shptr irregular_horizontal_grid,
		degate::IrregularGrid_shptr irregular_vertical_grid);

  virtual ~GridConfigWin();

  sigc::signal<void>& signal_changed();

  void show();
  void update_grid_entries();

 private:
  Gtk::Window *parent;
  degate::RegularGrid_shptr regular_horizontal_grid;
  degate::RegularGrid_shptr regular_vertical_grid;
  degate::IrregularGrid_shptr irregular_horizontal_grid;
  degate::IrregularGrid_shptr irregular_vertical_grid;

  sigc::signal<void>  signal_changed_;

  // global widgets
  Gtk::Button* p_close_button;

  Gtk::RadioButton * p_regular_grid_rbutton;
  Gtk::RadioButton * p_irregular_grid_rbutton;

  // regular grid
  Gtk::CheckButton * p_horizontal_checkbutton;
  Gtk::CheckButton * p_vertical_checkbutton;

  Gtk::HScale * p_scale_offset_x;
  Gtk::Adjustment * p_adj_offset_x;
  Gtk::HScale * p_scale_offset_y;
  Gtk::Adjustment * p_adj_offset_y;

  Gtk::HScale * p_scale_dist_x;
  Gtk::Adjustment * p_adj_dist_x;
  Gtk::HScale * p_scale_dist_y;
  Gtk::Adjustment * p_adj_dist_y;

  Gtk::Entry * p_entry_dist_x;
  Gtk::Entry * p_entry_dist_y;

  // irregular grid - horizontal lines
  Gtk::CheckButton * p_uhg_checkbutton;

  Glib::RefPtr<Gtk::ListStore> ref_liststore_uhg;
  GridConfigModelColumns m_columns_uhg;
  Gtk::TreeView* p_treeview_uhg;

  Gtk::Button* p_button_add_uhg;
  Gtk::Button* p_button_remove_uhg;

  // irregular grid - vertical lines
  Gtk::CheckButton * p_uvg_checkbutton;

  Glib::RefPtr<Gtk::ListStore> ref_liststore_uvg;
  GridConfigModelColumns m_columns_uvg;
  Gtk::TreeView* p_treeview_uvg;

  Gtk::Button* p_button_add_uvg;
  Gtk::Button* p_button_remove_uvg;


  // Signal handlers:

  virtual void on_rgrid_rbutton_clicked();
  virtual void on_urgrid_rbutton_clicked();

  virtual void on_offset_x_changed();
  virtual void on_offset_y_changed();
  virtual void on_dist_x_changed();
  virtual void on_dist_y_changed();
  virtual void on_entry_dist_x_changed();
  virtual void on_entry_dist_y_changed();

  virtual void on_close_button_clicked();

  virtual void on_horz_checkb_clicked();
  virtual void on_vert_checkb_clicked();

  // uhg
  virtual void on_uhg_checkb_clicked();
  virtual void on_button_add_uhg_clicked();
  virtual void on_button_remove_uhg_clicked();
  virtual void on_uhg_edited(const Glib::ustring& path, const Glib::ustring& new_text);

  // uvg
  virtual void on_uvg_checkb_clicked();
  virtual void on_button_add_uvg_clicked();
  virtual void on_button_remove_uvg_clicked();
  virtual void on_uvg_edited(const Glib::ustring& path, const Glib::ustring& new_text);

};

#endif
