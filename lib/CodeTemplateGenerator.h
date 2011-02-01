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

#ifndef __CODETEMPLATEGENERATOR_H__
#define __CODETEMPLATEGENERATOR_H__

#include <tr1/memory>
#include <map>
#include <vector>
#include <string>

#include <boost/foreach.hpp>

namespace degate {

  class CodeTemplateGenerator {
  public:
    typedef std::map<std::string, bool> port_direction_type;

  protected:
    std::string entity_name, description, logic_class;
    port_direction_type port_direction;

    /**
     * Get a list of input ports.
     */
    virtual std::vector<std::string> get_inports() const;

    /**
     * Get a list of output ports.
     */
    virtual std::vector<std::string> get_outports() const;

    /**
     * Get a list of ports.
     */
    virtual std::vector<std::string> get_ports() const;

    /**
     * Get the name of the port, that is most likely the clock port.
     * @return Returns an empty string, if no port name matches.
     */
    std::string get_clock_port_name() const;

    /**
     * Get the name of the port, that is most likely the reset port.
     * @return Returns an empty string, if no port name matches.
     */
    std::string get_reset_port_name() const;

    /**
     * Get the name of the port, that is most likely the enable port.
     * @return Returns an empty string, if no port name matches.
     */
    std::string get_enable_port_name() const;

    /**
     * Get the first port name from \p ports that is not contained in \p blacklist.
     * @return Returns an empty string, if no port name matches.
     */
    std::string get_first_port_name_not_in(std::vector<std::string> const& ports,
					   std::vector<std::string> const& blacklist) const;

    /**
     * Get the first port name from \p ports that is not equal to \p blacklist_item.
     * @return Returns an empty string, if no port name matches.
     */
    std::string get_first_port_name_not_in(std::vector<std::string> const& ports,
					   std::string const& blacklist_item) const;  



    virtual std::string generate_identifier(std::string const& name, std::string const& prefix = "") const = 0;

    template<typename Container>
    Container generate_identifier(Container const& c, std::string const& prefix = "") const {
      Container new_c;
      BOOST_FOREACH(typename Container::value_type const& s, c) {
	new_c.push_back(generate_identifier(s, prefix));
      }
      return new_c;
    }


  public:

    CodeTemplateGenerator(std::string const& _entity_name,
			  std::string const& _description,
			  std::string const& _logic_class);

    virtual ~CodeTemplateGenerator();

    virtual void add_port(std::string port_name, bool is_inport);

    virtual std::string generate() const = 0;
  };

  typedef std::tr1::shared_ptr<CodeTemplateGenerator> CodeTemplateGenerator_shptr;
}

#endif
