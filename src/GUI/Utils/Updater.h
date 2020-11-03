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

#ifndef __UPDATER_H__
#define __UPDATER_H__

#include <QProcess>
#include <QWidget>

namespace degate
{
    /**
     * @class CheckForUpdates
     * @brief Check for updates and let the user choose if he want to update now.
     */
    class Updater : public QObject
    {
        Q_OBJECT

    public:
        /**
         * Create the update checker.
         *
         * @param parent : the parent.
         */
        Updater(QWidget* parent);
        ~Updater() = default;

        /**
         * Run the check.
         *
         * @param notify_no_update : if true, will notify the user if no update is available.
         * @param ask_disabling_automatic_check : if true, will ask if the user want to disable automatic check for updates.
         */
        void run(bool notify_no_update, bool ask_disabling_automatic_check);

    protected slots:
        /**
         * Called when the check process finished.
         */
        void process_finished(int exit_code, QProcess::ExitStatus exit_status);

        /**
         * Called when there is an error with the process.
         */
        void process_error(QProcess::ProcessError error);

    private:
        QWidget* parent = nullptr;
        QProcess process;
        bool notify_no_update;
        QString tool_name;

        bool running = false;
        bool ask_disabling_automatic_check = true;
    };
}

#endif //__UPDATER_H__
