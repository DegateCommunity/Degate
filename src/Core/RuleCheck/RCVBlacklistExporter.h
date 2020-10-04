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

#ifndef __RCVBLACKLISTEXPORTER_H__
#define __RCVBLACKLISTEXPORTER_H__

#include "Globals.h"
#include "Core/RuleCheck/RCBase.h"
#include "Core/RuleCheck/RCViolation.h"
#include "Core/XML/XMLExporter.h"
#include "Core/Utils/ObjectIDRewriter.h"

#include <stdexcept>

namespace degate
{
    /**
     * The RCVBlacklistExporter exports a set of RC Violations, which should be ignored.
     */
    class RCVBlacklistExporter : public XMLExporter
    {
    private:

        void add_rcv(QDomDocument& doc, QDomElement& templates_elem, RCViolation_shptr rcv);

        ObjectIDRewriter_shptr oid_rewriter;

    public:
        RCVBlacklistExporter(ObjectIDRewriter_shptr oid_rewriter) : oid_rewriter(oid_rewriter)
        {
        }

        ~RCVBlacklistExporter()
        {
        }

        void export_data(std::string const& filename, RCBase::container_type const& violations);
    };
}

#endif
