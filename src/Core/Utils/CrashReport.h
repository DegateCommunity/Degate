/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2021 Dorian Bachelot

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

#ifndef __CRASHREPORT_H__
#define __CRASHREPORT_H__

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <string>

#define ERROR_FILE_NAME "error_log.txt"

namespace degate
{
    /**
     * @class CrashReport
     * @brief Create a crash report dialog.
     */
    class CrashReport : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create a crash report.
         *
         * Call exec() after that to run the dialog.
         *
         * @param error : the error to show.
         */
        explicit CrashReport(const std::string& error);

    private:
        QVBoxLayout layout;
        QLabel error_label;
        QTextEdit error_edit;

        QHBoxLayout buttons_layout;
        QPushButton open_error_file_button;
        QPushButton report_button;
        QPushButton quit_button;
    };

    /**
     * Create a crash report.
     *
     * It will log the error in a error_log.txt file in the same folder as the executable.
     * It will also open a new dialog (in a new thread) to inform the user.
     *
     * @param error : the error to log.
     */
    void crash_report(const std::string& error);

    /**
     * Create an error report dialog.
     *
     * @param error : the error to report.
     */
    void create_error_dialog(const std::string& error);

} // namespace degate

#endif //__CRASHREPORT_H__
