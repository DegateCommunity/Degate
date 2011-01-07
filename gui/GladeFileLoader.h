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

#ifndef __GLADEFILELOADER_H__
#define __GLADEFILELOADER_H__

#include <gtkmm.h>
#include <libglademm.h>

/**
 * Helper class to load glade files.
 *
 * In order to use this class, you should inherit from it in private mode.
 * That is a is-implemented-in-terms-of relationship.
 */
class GladeFileLoader {

 public:

  // no public methods

 private:

  Gtk::Dialog * pDialog;
  Glib::RefPtr<Gnome::Glade::Xml> refXml;

 protected:

  GladeFileLoader(std::string const& glade_file, std::string const& dialog_name);
  virtual ~GladeFileLoader();

  template<typename DstType>
  DstType * get_widget(const Glib::ustring & name) const {
    return static_cast<DstType *>(refXml->get_widget(name));
  }

  template<class T_Widget>
  T_Widget * get_widget(const Glib::ustring &name, T_Widget *&widget) const {
    return refXml->get_widget(name, widget);
  }

  Gtk::Dialog * get_dialog() const { return pDialog; }



};

#endif
