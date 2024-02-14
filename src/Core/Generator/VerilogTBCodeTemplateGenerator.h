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

#ifndef __VERILOGTBCODETEMPLATEGENERATOR_H__
#define __VERILOGTBCODETEMPLATEGENERATOR_H__

#include <memory>
#include <cctype>

#include "Core/Generator/VerilogCodeTemplateGenerator.h"

namespace degate
{
    /**
     * A code template generator for Verilog.
     */
    class VerilogTBCodeTemplateGenerator : public VerilogCodeTemplateGenerator
    {
    public:

        VerilogTBCodeTemplateGenerator(std::string const& entity_name,
                                       std::string const& description,
                                       std::string const& logic_class);

        virtual ~VerilogTBCodeTemplateGenerator();
        virtual std::string generate() const;

    protected:

        virtual std::string generate_header() const;
        virtual std::string generate_module(std::string const& device_type_name) const;

        std::string generate_all_assignments(std::vector<std::string> const& in_port_idents,
                                             std::vector<std::string> const& out_port_idents) const;
    };

    typedef std::shared_ptr<VerilogTBCodeTemplateGenerator> VerilogTBCodeTemplateGenerator_shptr;
}

#endif
