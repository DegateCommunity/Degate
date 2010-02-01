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

#ifndef __VHDLCODETEMPLATEGENERATOR_H__
#define __VHDLCODETEMPLATEGENERATOR_H__

#include <degate.h>
#include <tr1/memory>
#include <cctype>

namespace degate {

  class CodeTemplateGenerator {
  public:
    typedef std::map<std::string, bool> port_map_type;
    
  protected:
    std::string entity_name, description;
    port_map_type port_map;

  public:

    CodeTemplateGenerator(std::string const& _entity_name,
			  std::string const& _description) : 
      entity_name(_entity_name),
      description(_description) {}

    virtual ~CodeTemplateGenerator() {}

    virtual void add_port(std::string port_name, bool is_inport) {
      port_map[port_name] = is_inport;
    }

    virtual std::string generate() const = 0;
  };

  typedef std::tr1::shared_ptr<CodeTemplateGenerator> CodeTemplateGenerator_shptr;

  /**
   * A code template generator for VHDL.
   */
  
  class VHDLCodeTemplateGenerator : public CodeTemplateGenerator {
  public:
    
    VHDLCodeTemplateGenerator(std::string const& entity_name,
			      std::string const& description);

    virtual ~VHDLCodeTemplateGenerator();

    std::string generate() const;

  private:

    std::string generate_header() const;

    std::string generate_enitity() const;

    std::string generate_architecture() const;

    /**
     * Generate a VHDL complient identifier from a string.
     *
     * VHDL identifier:
     *
     * - Must begin with alphabetic characters (a-z or A-Z)
     * - Can contain alphanumeric (a-z, A-Z, 0-9) or underscore (_) characters
     * - Can be up to 1024 characters long
     * - Cannot contain white space
     * - are not case sensitive.
     */
    std::string generate_identifier(std::string const& name) const;

  };

  typedef std::tr1::shared_ptr<VHDLCodeTemplateGenerator> VHDLCodeTemplateGenerator_shptr;

}

#endif
