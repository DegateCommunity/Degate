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


#include <VerilogModuleGenerator.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
using namespace degate;

VerilogModuleGenerator::VerilogModuleGenerator(Module_shptr module) :
  VerilogCodeTemplateGenerator(module->get_name(), module->get_entity_name()), 
  mod(module) {
}


VerilogModuleGenerator::~VerilogModuleGenerator() {
}

std::string VerilogModuleGenerator::generate_impl(std::string const& logic_class /* unused parameter */ ) const {
  
  // place single standard cells

  for(Module::gate_collection::const_iterator iter = mod->gates_begin();
      iter != mod->gates_end(); ++iter) {
    
  }


  // place sub-modules
}
