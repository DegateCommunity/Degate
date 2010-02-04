/* -*-c++-*-
 
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

#ifndef __CODETEMPLATEGENERATOR_H__
#define __CODETEMPLATEGENERATOR_H__

#include <tr1/memory>
#include <map>
#include <vector>
#include <string>

namespace degate {

  class CodeTemplateGenerator {
  public:
    typedef std::map<std::string, bool> port_map_type;
    
  protected:
    std::string entity_name, description, logic_class;
    port_map_type port_map;

  public:

    CodeTemplateGenerator(std::string const& _entity_name,
			  std::string const& _description,
			  std::string const& _logic_class);

    virtual ~CodeTemplateGenerator();

    virtual void add_port(std::string port_name, bool is_inport);

    virtual std::vector<std::string> get_inports() const;

    virtual std::vector<std::string> get_outports() const;

    virtual std::string generate() const = 0;
  };

  typedef std::tr1::shared_ptr<CodeTemplateGenerator> CodeTemplateGenerator_shptr;
}

#endif
