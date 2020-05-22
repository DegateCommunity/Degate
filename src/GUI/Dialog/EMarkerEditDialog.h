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

#ifndef __EMARKEREDITDIALOG_H__
#define __EMARKEREDITDIALOG_H__

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include <GUI/Dialog/ColorPickerDialog.h>
#include <Core/LogicModel/EMarker/EMarker.h>

namespace degate
{

    /**
	 * @class EMarkerEditDialog
	 * @brief Dialog to edit an emarker.
	 *
	 * @see QDialog
	 */
    class EMarkerEditDialog : public QDialog
    {
    Q_OBJECT

    public:

        /**
		 * Create the dialog, to show it call the exec function.
		 *
		 * @param emarker : the emarker to edit.
		 * @param parent : the parent of the dialog.
		 *
		 * @see QDialog
		 */
        EMarkerEditDialog(EMarker_shptr& emarker, QWidget* parent);
        ~EMarkerEditDialog();

    public slots:

        /**
         * Save all changes and close the dialog.
         */
        void validate();

    private:
        QGridLayout layout;
        EMarker_shptr emarker;

        QLabel name_label;
        QLineEdit name;
        QLabel fill_color_label;
        ColorSelectionButton fill_color;
        QPushButton validate_button;
        QPushButton cancel_button;
    };
}

#endif //__EMARKEREDITDIALOG_H__
