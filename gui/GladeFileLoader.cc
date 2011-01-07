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

#include "GladeFileLoader.h"

#include <globals.h>
#include <iostream>
#include <libglademm.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <boost/format.hpp>


GladeFileLoader::GladeFileLoader(std::string const& glade_file, std::string const& dialog_name) {


  boost::format fmter("%1%/glade/%2%");
  fmter % getenv("DEGATE_HOME") % glade_file;
  std::string file(fmter.str());
  debug(TM, "Loading glade file: %s", file.c_str());

  //Load the Glade file and instiate its widgets:

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    refXml = Gnome::Glade::Xml::create(file);
  }
  catch(const Gnome::Glade::XmlError& ex) {
    std::cerr << ex.what() << std::endl;
    return;
  }
#else
  std::auto_ptr<Gnome::Glade::XmlError> error;
  refXml = Gnome::Glade::Xml::create(file, "", "", error);
  if(error.get()) {
    std::cerr << error->what() << std::endl;
    return ;
  }
#endif

  //Get the Glade-instantiated Dialog:
  refXml->get_widget(dialog_name, pDialog);
  assert(pDialog);
}

GladeFileLoader::~GladeFileLoader() {
  delete pDialog;
  pDialog = NULL;
}

