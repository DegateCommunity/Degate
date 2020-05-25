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

#ifndef __VIAEDITDIALOG_H__
#define __VIAEDITDIALOG_H__

#include <GUI/Dialog/ColorPickerDialog.h>
#include <Core/LogicModel/Via/Via.h>
#include <Core/Project/Project.h>

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>

namespace degate
{
    /**
	 * @class ViaEditDialog
	 * @brief Dialog to edit a via.
	 *
	 * @see QDialog
	 */
    class ViaEditDialog : public QDialog
    {
        Q_OBJECT

    public:

        /**
		 * Create the dialog, to show it call the exec function.
		 *
		 * @param via : the via to edit.
		 * @param parent : the parent of the dialog.
		 *
		 * @see QDialog
		 */
        ViaEditDialog(Via_shptr& via, QWidget* parent, Project_shptr& project);
        ~ViaEditDialog();

    public slots:

        /**
         * Save all changes and close the dialog.
         */
        void validate();

        /**
         * Called when the direction changed.
         */
        void direction_changed();

    private:
        Project_shptr project = nullptr;
        QGridLayout layout;
        Via_shptr via;

        // Buttons
        QHBoxLayout buttons_layout;
        QPushButton validate_button;
        QPushButton cancel_button;

        // Elements
        QLabel name_label;
        QLineEdit name_edit;

        QLabel fill_color_label;
        ColorSelectionButton fill_color_edit;

        QLabel direction_label;
        QComboBox direction_edit;
    };
}

#endif //__VIAEDITDIALOG_H__
