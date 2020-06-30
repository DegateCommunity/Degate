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

#include "SelectGateTemplateDialog.h"

#include <utility>

namespace degate
{
    SelectGateTemplateDialog::SelectGateTemplateDialog(Project_shptr project, QWidget* parent, bool unique_selection)
            : QDialog(parent), list(std::move(project), this, unique_selection)
    {
		validate_button.setText(tr("Ok"));
		layout.addWidget(&list);
		layout.addWidget(&validate_button);

		setLayout(&layout);

		QObject::connect(&validate_button, SIGNAL(clicked()), this, SLOT(accept()));
	}

	SelectGateTemplateDialog::~SelectGateTemplateDialog()
	{
	}

	GateTemplate_shptr SelectGateTemplateDialog::get_selected_gate()
	{
		return list.get_selected_gate();
	}

    std::vector<GateTemplate_shptr> SelectGateTemplateDialog::get_selected_gates()
    {
        return list.get_selected_gates();
    }
}
