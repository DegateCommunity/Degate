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

#ifndef __TERMINALDIALOG_H__
#define __TERMINALDIALOG_H__

#include <QWidget>
#include <QDialog>
#include <QProcess>
#include <QTextEdit>
#include <QVBoxLayout>

#include <vector>
#include <string>
#include <QtWidgets/QDialogButtonBox>

namespace degate
{
    /**
     * @class TerminalWidget
     * @brief Run commands and show outputs in the widget.
     */
    class TerminalWidget : public QWidget
    {
        Q_OBJECT

    public:
        /**
         * Create the terminal widget.
         *
         * @param parent : the parent of the widget.
         * @param commands : vector of commands (std::string type).
         */
        TerminalWidget(QWidget* parent, std::vector<std::string>& commands);
        ~TerminalWidget() override;

        /**
         * Get current available outputs (that are shown).
         *
         * @return Returns actual available outputs.
         */
        std::string get_output();

    public slots:
        /**
         * Start the execution of all commands (will follow the vector entry order).
         */
        void start();

    private slots:
        /**
         * Get new outputs from current running command.
         */
        void write_output();

        /**
         * Get new errors from current running command.
         */
        void write_error();

        /**
         * Run next command after current command finished running (if last command, stop).
         */
        void process_finished();

    private:
        std::vector<std::string>& commands;
        QProcess process;
        QVBoxLayout layout;

        QTextEdit terminal;
    };

    /**
     * @class TerminalDialog
     * @brief Run commands and show outputs in a dialog window.
     */
    class TerminalDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * Create the terminal dialog.
         *
         * @param parent : the parent of the widget.
         * @param commands : vector of commands (std::string type).
         */
        TerminalDialog(QWidget* parent, std::vector<std::string> &commands);
        ~TerminalDialog() override = default;

        /**
         * Get current available outputs (that are shown).
         *
         * @return Returns actual available outputs.
         */
        std::string get_output();

    public slots:
        /**
         * Start the execution of all commands (will follow the vector entry order).
         */
        void start();

        /**
         * Close the dialog window when the Ok button is clicked.
         */
        void finish();

    private:
        TerminalWidget terminal;
        QDialogButtonBox button_box;
        QVBoxLayout layout;

    };
}

#endif //__TERMINALDIALOG_H__
