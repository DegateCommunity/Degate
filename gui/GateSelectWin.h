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

#ifndef __GATESELECTWIN_H__
#define __GATESELECTWIN_H__

#include <gtkmm.h>

#include "Project.h"
#include "LogicModel.h"

#include "GladeFileLoader.h"

class GateSelectWin : private GladeFileLoader  {

  class GateListModelColumns : public Gtk::TreeModelColumnRecord {
  public:

    GateListModelColumns() {
      add(m_col_id);
      add(m_col_refcount);
      add(m_col_width);
      add(m_col_height);
      add(m_col_short_name);
      add(m_col_description);
    }

    Gtk::TreeModelColumn<degate::object_id_t> m_col_id;
    Gtk::TreeModelColumn<int> m_col_refcount;
    Gtk::TreeModelColumn<int> m_col_width;
    Gtk::TreeModelColumn<int> m_col_height;
    Gtk::TreeModelColumn<Glib::ustring> m_col_short_name;
    Gtk::TreeModelColumn<Glib::ustring> m_col_description;

  };

 public:

  GateSelectWin(Gtk::Window *parent, degate::LogicModel_shptr lmodel);

  virtual ~GateSelectWin();

  std::list<degate::GateTemplate_shptr> get_selection(bool allow_multiple_selection = false);

 private:

  Gtk::Window *parent;
  degate::LogicModel_shptr lmodel;

  //Gtk::Dialog* pDialog;
  Gtk::Button* pOkButton;

  GateListModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;

  bool ok_clicked;

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();
  virtual void on_selection_changed();
};

#endif
