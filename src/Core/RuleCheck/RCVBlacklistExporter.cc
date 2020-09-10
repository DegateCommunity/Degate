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

#include "Core/RuleCheck/RCVBlacklistExporter.h"
#include "Core/Image/ImageHelper.h"

#include <iostream>
#include <stdexcept>
#include <list>


using namespace std;
using namespace degate;

void RCVBlacklistExporter::export_data(std::string const& filename,
                                       RCBase::container_type const& violations)
{
	std::string directory = get_basedir(filename);

	try
	{
		QDomDocument doc;

		QDomProcessingInstruction head = doc.createProcessingInstruction("xml", XML_ENCODING);
		doc.appendChild(head);

		QDomElement root_elem = doc.createElement("rc-blacklist");
		assert(!root_elem.isNull());

		BOOST_FOREACH(RCViolation_shptr rcv, violations)
		{
			add_rcv(doc, root_elem, rcv);
		}

		doc.appendChild(root_elem);

		QFile file(QString::fromStdString(filename));
		if (!file.open(QIODevice::WriteOnly))
		{
			throw InvalidPathException("Can't create export file.");
		}

		QTextStream stream(&file);
        stream.setCodec("UTF-8");
		stream << doc.toString();

		file.close();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
		throw;
	}
}

void RCVBlacklistExporter::add_rcv(QDomDocument& doc,
                                   QDomElement& root_elem,
                                   RCViolation_shptr rcv)
{
	QDomElement rcv_elem = doc.createElement("rc-violation");
	if (rcv_elem.isNull()) throw(std::runtime_error("Failed to create node."));

	PlacedLogicModelObject_shptr o = rcv->get_object();
	object_id_t new_oid = oid_rewriter->get_new_object_id(o->get_object_id());

	rcv_elem.setAttribute("object-id", QString::fromStdString(number_to_string<object_id_t>(new_oid)));
	rcv_elem.setAttribute("rc-violation-class", QString::fromStdString(rcv->get_rc_violation_class()));
	rcv_elem.setAttribute("severity", QString::fromStdString(rcv->get_severity_as_string()));
	rcv_elem.setAttribute("description", QString::fromStdString(rcv->get_problem_description()));

	root_elem.appendChild(rcv_elem);
}
