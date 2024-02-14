/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CODETEMPLATEGENERATOR_H__
#define __CODETEMPLATEGENERATOR_H__

#include <memory>
#include <map>
#include <vector>
#include <string>

namespace degate
{
    /**
     * Base class for code generators.
     */
    class CodeTemplateGenerator
    {
    public:
        typedef std::map<std::string, bool> port_direction_type;

        /**
         *
         */
        enum PORT_FUNCTION_TYPE
        {
            CLOCK,
            RESET,
            ENABLE,
            Q,
            NOT_Q,
            D,
            SELECT
        };

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
         * Get the name of the port, that is most likely of the type \p t.
         * @return Returns an empty string, if no port name matches.
         */
        std::string get_port_name_by_type(PORT_FUNCTION_TYPE t) const;


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

        template <typename Container>
        Container generate_identifier(Container const& c, std::string const& prefix = "") const
        {
            Container new_c;
            for (const auto& s : c)
            {
                new_c.push_back(generate_identifier(s, prefix));
            }
            return new_c;
        }


    public:

        CodeTemplateGenerator(std::string const& entity_name,
                              std::string const& description,
                              std::string const& logic_class);

        virtual ~CodeTemplateGenerator();

        virtual void add_port(std::string const& port_name, bool is_inport);

        virtual std::string generate() const = 0;
    };

    typedef std::shared_ptr<CodeTemplateGenerator> CodeTemplateGenerator_shptr;
}

#endif
