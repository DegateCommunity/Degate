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

#include <degate.h>
#include <RCVBlacklistImporter.h>
#include <ImageHelper.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <list>
#include <tr1/memory>

using namespace std;
using namespace degate;

void RCVBlacklistImporter::import_into(std::string const& filename,
				  RCBase::container_type & blacklist) {

  if(RET_IS_NOT_OK(check_file(filename))) {
    debug(TM, "Problem: file %s not found.", filename.c_str());
    throw InvalidPathException("Can't load gate library from file.");
  }
  
  std::string directory = get_basedir(filename);

  try {

    xmlpp::DomParser parser;
    parser.set_substitute_entities(); // We just want the text to be resolved/unescaped automatically.

    parser.parse_file(filename);
    assert(parser == true);

    const xmlpp::Document * doc = parser.get_document();
    assert(doc != NULL);

    const xmlpp::Element * root_elem = doc->get_root_node(); // deleted by DomParser
    assert(root_elem != NULL);

    parse_list(root_elem, blacklist);

  }
  catch(const std::exception& ex) {
    std::cout << "Exception caught: " << ex.what() << std::endl;
    throw;
  }



}


void RCVBlacklistImporter::parse_list(const xmlpp::Element * const elem,
				      RCBase::container_type & blacklist) {

  const xmlpp::Node::NodeList rcv_list = elem->get_children("rc-violation");
  for(xmlpp::Node::NodeList::const_iterator iter = rcv_list.begin();
      iter != rcv_list.end(); ++iter) {

    if(const xmlpp::Element* e = dynamic_cast<const xmlpp::Element*>(*iter)) {

      object_id_t object_id = parse_number<object_id_t>(e, "object-id");

      const Glib::ustring rcv_class(e->get_attribute_value("rc-violation-class"));
      const Glib::ustring description(e->get_attribute_value("description"));
      const Glib::ustring severity(e->get_attribute_value("severity"));

      RCViolation_shptr rcv(new RCViolation(_lmodel->get_object(object_id),
					    description,
					    rcv_class,
					    RCViolation::get_severity_from_string(severity)));
      
      blacklist.push_back(rcv);
    }
  }
}

