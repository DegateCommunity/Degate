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

#ifndef __RCBASE_H__
#define __RCBASE_H__

#include <boost/foreach.hpp>
#include <memory>
#include <list>
#include "Core/LogicModel/LogicModel.h"
#include "Core/RuleCheck/RCVContainer.h"

namespace degate
{
    /**
     * An enum for several types of Rule Check problem types.
     */
    enum RC_SEVERITY
    {
        RC_UNDEFINED = 0,
        RC_ERROR = 1,
        RC_WARNING = 2
    };


    /**
     * Base class for Rule Checks.
     * When creating a new ERC you need to register it, with corresponding violation class, in the ERCRegister.h file.
     */
    class RCBase
    {
    public:

        typedef RCVContainer container_type;


    private:

        std::string description;
        RC_SEVERITY severity;

        container_type rc_violations;

    public:

        /**
         * The constructor.
         * @param description A decription of what the RC basically checks.
         * @param severity This parameter indicates wheather this
         *   RC violation container is for an error or just for a warning.
         */
        RCBase(std::string const& description, RC_SEVERITY severity = RC_ERROR)
                : description(description), severity(severity)
        {
        }

        virtual ~RCBase()
        {
        }

        /**
         * The run method is abstract and must be implemented in derived
         * classes. The implementation should check for design rule violations.
         * Each RC violation must be stored via method add_rc_violation().
         * Note: Because run() can be called multiple times, at the beginning of
         * run() you must clear the list of detected violations.
         */
        virtual void run(LogicModel_shptr lmodel) = 0;

        /**
         * Generate the description for a violation regarding the tuple class + object.
         * A violation needs to be unique for that tuple.
         *
         * @param violation : the violation.
         *
         * @return Returns a translated and newly generated description for the violation.
         */
        virtual std::string generate_description(const RCViolation& violation) = 0;

        /**
         * Get the list of RC violations.
         */
        container_type get_rc_violations() const
        {
            return rc_violations;
        }

        RC_SEVERITY get_severity() const
        {
            return severity;
        }

    protected:

        /**
         * Add a RC violation to the list of already detected violations.
         */
        void add_rc_violation(RCViolation_shptr violation)
        {
            rc_violations.push_back(violation);
        }

        /**
         * Clear list of detected violations.
         */
        void clear_rc_violations()
        {
            rc_violations.clear();
        }
    };

    typedef std::shared_ptr<RCBase> RCBase_shptr;
}

#include "Core/RuleCheck/RCViolation.h"

#endif
