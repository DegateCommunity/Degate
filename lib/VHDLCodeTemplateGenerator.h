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

#ifndef __VHDLCODETEMPLATEGENERATOR_H__
#define __VHDLCODETEMPLATEGENERATOR_H__

#include <degate.h>
#include <tr1/memory>
#include <cctype>

#include <CodeTemplateGenerator.h>
#include <boost/foreach.hpp>

namespace degate {

  /**
   * A code template generator for VHDL.
   */
  
  class VHDLCodeTemplateGenerator : public CodeTemplateGenerator {
  public:
    
    VHDLCodeTemplateGenerator(std::string const& entity_name,
			      std::string const& description,
			      std::string const& logic_class);

    virtual ~VHDLCodeTemplateGenerator();

    virtual std::string generate() const;

    typedef std::map<std::string, std::string> port_map_type;
    
  protected:

    virtual std::string generate_header() const;

    virtual std::string generate_port_description() const;

    virtual std::string generate_entity(std::string const& entity_name,
					std::string const& port_description = "") const;

    virtual std::string generate_component(std::string const& entity_name,
					   std::string const& port_description) const;

    virtual std::string generate_architecture(std::string const& entity_name,
					      std::string const& header,
					      std::string const& impl) const;

    virtual std::string generate_impl(std::string const& logic_class) const;


    virtual std::string generate_instance(std::string const& instance_name,
					  std::string const& instance_type,
					  port_map_type const& port_map) const;

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
    virtual std::string generate_identifier(std::string const& name) const;

    template<typename Container>
    Container generate_identifier(Container const& c) const {
      Container new_c;
      BOOST_FOREACH(typename Container::value_type const& s, c) {
	new_c.push_back(generate_identifier(s));
      }
      return new_c;
    }

  };

  typedef std::tr1::shared_ptr<VHDLCodeTemplateGenerator> VHDLCodeTemplateGenerator_shptr;

}

#endif
