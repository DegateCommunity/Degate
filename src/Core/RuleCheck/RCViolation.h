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

#ifndef __RCVIOLATION_H__
#define __RCVIOLATION_H__

#include <memory>
#include <list>
#include "Core/LogicModel/LogicModel.h"
#include "Core/RuleCheck/RCBase.h"
#include "Core/RuleCheck/RCVContainer.h"

namespace degate
{
    class RCViolation
    {
    Q_DECLARE_TR_FUNCTIONS(degate::RCViolation)

    private:

        PlacedLogicModelObject_shptr obj;
        std::string rc_violation_class;
        RC_SEVERITY severity;

    public:

        /**
         * Create a new Rule Check violation.
         *
         * @param obj : the object, which is affected from the violation.
         * @param rc_violation_class : this is a unique technical name for a rc violation, that indicates the problem class.
         * @param severity : indicates the type of problem, actually if a rc violation is just a warning or a real error.
         */
        RCViolation(PlacedLogicModelObject_shptr obj,
                    std::string const& rc_violation_class,
                    RC_SEVERITY severity = RC_ERROR);

        std::string get_problem_description() const;

        std::string get_rc_violation_class() const;

        RC_SEVERITY get_severity() const;

        std::string get_severity_as_string() const;

        std::string get_translated_severity_as_string() const;

        static RC_SEVERITY get_severity_from_string(std::string const& str);

        bool matches_filter(std::string const& filter_pattern) const;

        PlacedLogicModelObject_shptr get_object() const;

        /**
         * Check if two rc violations are conceptually equal.
         */
        bool equals(RCViolation_shptr rcv) const;

    };
}

#endif
