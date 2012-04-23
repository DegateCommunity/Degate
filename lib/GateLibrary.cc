/* -*-c++-*-

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
#include <GateLibrary.h>

using namespace degate;

GateLibrary::GateLibrary() {
}

GateLibrary::~GateLibrary() {
}


void GateLibrary::remove_template(GateTemplate_shptr gate_template) {
  templates.erase(gate_template->get_object_id());
}

void GateLibrary::add_template(GateTemplate_shptr gate_template) {

  if(gate_template == NULL) throw InvalidPointerException();
  if(!gate_template->has_valid_object_id())
    throw InvalidObjectIDException("Can't add a gate template to the gate library, "
				   "if the template has no valid object ID.");
  else
    templates[gate_template->get_object_id()] = gate_template;
}

bool GateLibrary::exists_template(object_id_t id) const {
  return templates.find(id) != templates.end();
}


GateTemplate_shptr GateLibrary::get_template(object_id_t id) {

  if(id == 0)
    throw InvalidObjectIDException("Error in get_template(): Can't lookup template with id == 0");

  template_iterator found = templates.find(id);
  if(found == templates.end()) {
    boost::format f("Error in get_template(): Can't lookup gate template with ID %1%");
    f % id;
    throw CollectionLookupException(f.str());
  }
  return found->second;
}


bool GateLibrary::is_name_in_use(std::string const & name) const {

  for(const_template_iterator iter = begin(); iter != end(); ++iter) {
    if((*iter).second->has_name() && (*iter).second->get_name() == name)
      return true;
  }

  return false;
}

bool GateLibrary::exists_template_port(object_id_t port_id) {

  for(template_iterator iter = begin(); iter != end(); ++iter) {

    GateTemplate_shptr tmpl((*iter).second);

    for(GateTemplate::port_iterator piter = tmpl->ports_begin();
	piter != tmpl->ports_end();
	piter++) {
      if((*piter)->get_object_id() == port_id) return true;
    }
  }

  return false;
}


GateTemplatePort_shptr GateLibrary::get_template_port(object_id_t port_id) {

  for(template_iterator iter = begin(); iter != end(); ++iter) {

    GateTemplate_shptr tmpl((*iter).second);

    for(GateTemplate::port_iterator piter = tmpl->ports_begin();
	piter != tmpl->ports_end();
	piter++) {
      if((*piter)->get_object_id() == port_id) return *piter;
    }
  }

  std::ostringstream stm;
  stm << "There is no template port with ID. " << port_id << " in the gate library.";
  throw CollectionLookupException(stm.str());
}

GateLibrary::template_iterator GateLibrary::begin() {
  return templates.begin();
}


GateLibrary::template_iterator GateLibrary::end() {
  return templates.end();
}

GateLibrary::const_template_iterator GateLibrary::begin() const {
  return templates.begin();
}


GateLibrary::const_template_iterator GateLibrary::end() const {
  return templates.end();
}


void GateLibrary::print(std::ostream & os) {
  for(template_iterator iter = begin(); iter != end(); ++iter) {
    GateTemplate_shptr tmpl = (*iter).second;

    tmpl->print(os);
    os << std::endl;

  }
}
