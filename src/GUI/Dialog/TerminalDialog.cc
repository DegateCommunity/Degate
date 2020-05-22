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


#include <GUI/Dialog/TerminalDialog.h>

namespace degate
{

    // Widget

    TerminalWidget::TerminalWidget(QWidget *parent, std::vector<std::string>& commands) : process(parent), commands(commands)
    {
        terminal.setReadOnly(true);
        layout.addWidget(&terminal);

        QObject::connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(write_output()));
        QObject::connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(write_error()));
        QObject::connect(&process, SIGNAL(finished(int)), this, SLOT(process_finished()));

        setLayout(&layout);
    }

    TerminalWidget::~TerminalWidget()
    {
        terminal.close();
    }

    void TerminalWidget::write_output()
    {
        terminal.append(process.readAllStandardOutput());
    }

    void TerminalWidget::write_error()
    {
        terminal.append(process.readAllStandardError());
    }

    void TerminalWidget::process_finished()
    {
        if(commands.size() <= 0)
            return;

        start();
    }

    void TerminalWidget::start()
    {
        std::vector<std::string>::iterator pop = commands.begin();
        std::string temp = *pop;
        commands.erase(pop);

        process.start(QString::fromStdString(temp));
    }

    std::string TerminalWidget::get_output()
    {
        return terminal.toPlainText().toStdString();
    }


    // Dialog

    TerminalDialog::TerminalDialog(QWidget *parent, std::vector<std::string> &commands) : terminal(parent, commands), button_box(QDialogButtonBox::Ok)
    {
        setWindowTitle("Degate terminal");
        resize(500, 400);

        layout.addWidget(&terminal);
        layout.addWidget(&button_box);

        setLayout(&layout);

        QObject::connect(&button_box, SIGNAL(accepted()), this, SLOT(finish()));
    }

    TerminalDialog::~TerminalDialog()
    {

    }

    void TerminalDialog::start()
    {
        if(!isVisible())
            open();

        terminal.start();
    }

    void TerminalDialog::finish()
    {
        close();
    }

    std::string TerminalDialog::get_output()
    {
        return terminal.get_output();
    }
}
