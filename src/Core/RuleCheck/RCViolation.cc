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

#include "RCViolation.h"

#include "ERCRegister.h"

namespace degate
{
    RCViolation::RCViolation(PlacedLogicModelObject_shptr obj, std::string const& rc_violation_class, RC_SEVERITY severity)
            : obj(obj),
              rc_violation_class(rc_violation_class),
              severity(severity)
    {

    }

    std::string RCViolation::get_problem_description() const
    {
        // Not very performant, bu useful when language change.
        return ERC_REGISTER.generate_description(*this);
    }

    std::string RCViolation::get_rc_violation_class() const
    {
        return rc_violation_class;
    }

    RC_SEVERITY RCViolation::get_severity() const
    {
        return severity;
    }

    std::string RCViolation::get_severity_as_string() const
    {
        switch (severity)
        {
            case RC_ERROR:
                return "error";
                break;
            case RC_WARNING:
                return "warning";
                break;
            case RC_UNDEFINED:
            default:
                return "undefined";
                break;
        }
    }

    std::string RCViolation::get_translated_severity_as_string() const
    {
        switch (severity)
        {
            case RC_ERROR:
                return tr("error").toStdString();
                break;
            case RC_WARNING:
                return tr("warning").toStdString();
                break;
            case RC_UNDEFINED:
            default:
                return tr("undefined").toStdString();
                break;
        }
    }

    RC_SEVERITY RCViolation::get_severity_from_string(std::string const& str)
    {
        if (str == "error")
        {
            return RC_ERROR;
        }
        else if (str == "warning")
        {
            return RC_WARNING;
        }
        return RC_UNDEFINED;
    }

    bool RCViolation::matches_filter(std::string const& filter_pattern) const
    {
        return filter_pattern.empty() ||
               ERC_REGISTER.get_translated_violation_class(*this).find(filter_pattern) != std::string::npos ||
               get_translated_severity_as_string().find(filter_pattern) != std::string::npos ||
               get_problem_description().find(filter_pattern) != std::string::npos;
    }

    PlacedLogicModelObject_shptr RCViolation::get_object() const
    {
        return obj;
    }

    bool RCViolation::equals(RCViolation_shptr rcv) const
    {
        return obj == rcv->obj &&
               rc_violation_class == rcv->rc_violation_class &&
               severity == rcv->severity;
    }
}