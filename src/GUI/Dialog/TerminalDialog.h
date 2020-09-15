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
     * @struct TerminalCommand
     * @brief Define a terminal command to use with a TerminalWidget.
     *
     * @see TerminalWidget
     * @see TerminalDialog
     */
    struct TerminalCommand
    {
        QString program;       /**< The program name to run */
        QStringList arguments; /**< The program arguments */
    };

    /**
     * Define a list of terminal commands.
     *
     * @see TerminalWidget
     * @see TerminalDialog
     */
    typedef std::vector<TerminalCommand> TerminalCommands;

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
         * @param commands : terminal commands (@see TerminalCommands).
         */
        TerminalWidget(QWidget* parent, TerminalCommands& commands);
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
        TerminalCommands& commands;
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
         * @param commands : terminal commands (@see TerminalCommands).
         */
        TerminalDialog(QWidget* parent, TerminalCommands& commands);
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
