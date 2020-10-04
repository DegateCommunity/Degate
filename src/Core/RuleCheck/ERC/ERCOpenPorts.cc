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

#include "ERCOpenPorts.h"
#include "Core/RuleCheck/RCBase.h"

#include <memory>


using namespace degate;

ERCOpenPorts::ERCOpenPorts()
        : RCBase("Check for unconnected ports.", RC_WARNING)
{
}

void ERCOpenPorts::run(LogicModel_shptr lmodel)
{
	clear_rc_violations();

	if (lmodel == nullptr) return;

	// iterate over Gates
	debug(TM, "\tRC: iterate over gates.");

	for (LogicModel::gate_collection::iterator g_iter = lmodel->gates_begin();
	     g_iter != lmodel->gates_end(); ++g_iter)
	{
		Gate_shptr gate = g_iter->second;

		for (Gate::port_const_iterator p_iter = gate->ports_begin();
		     p_iter != gate->ports_end(); ++p_iter)
		{
			GatePort_shptr port = *p_iter;
			assert(port != nullptr);

			Net_shptr net = port->get_net();
			if (net == nullptr || net->size() <= 1)
			{
                add_rc_violation(std::make_shared<RCViolation>(port, "open_port", get_severity()));
            }
		}
	}
}

std::string ERCOpenPorts::generate_description(const RCViolation& violation)
{
    auto res = tr("Port %1 is unconnected.");

    return res.arg(QString::fromStdString(violation.get_object()->get_descriptive_identifier())).toStdString();
}
