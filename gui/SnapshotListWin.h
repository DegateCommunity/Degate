/*

This file is part of the IC reverse engineering tool degate.

Copyright 2012 Robert Nitsch

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

#ifndef __SNAPSHOTLISTWIN_H__
#define __SNAPSHOTLISTWIN_H__

#include <gtkmm.h>

#include <degate.h>
#include "GladeFileLoader.h"
#include "Project.h"

class MainWin;

class SnapshotListWin : private GladeFileLoader {

  class SnapshotListModelColumns : public Gtk::TreeModelColumnRecord {
  public:
    SnapshotListModelColumns() {
      add(m_col_timestamp);
      add(m_col_datetime);
      add(m_col_title);
      add(m_col_ptr);
    }

    Gtk::TreeModelColumn<long> m_col_timestamp;
    Gtk::TreeModelColumn<Glib::ustring> m_col_datetime;
    Gtk::TreeModelColumn<Glib::ustring> m_col_title;
    Gtk::TreeModelColumn<degate::ProjectSnapshot_shptr> m_col_ptr;
  };

public:
  SnapshotListWin(Gtk::Window *parent);
  virtual ~SnapshotListWin();
  void run();

private:
  Gtk::Window *parent;
  MainWin *mainwin;

  SnapshotListModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeView* pTreeView;
  
  degate::ProjectSnapshot_shptr get_selected_snapshot() const;
  void fill_row(Gtk::TreeModel::Row const& row, const degate::ProjectSnapshot_shptr &ss);

  // Signal handlers:
  //@{
  virtual void on_close_button_clicked();
  virtual void on_add_button_clicked();
  virtual void on_remove_button_clicked();
  virtual void on_clear_button_clicked();
  virtual void on_revert_button_clicked();
  virtual void on_selection_changed();
  virtual void on_snapshot_title_edited(const Glib::ustring& path, const Glib::ustring& new_text);
  //@}

};

#endif
