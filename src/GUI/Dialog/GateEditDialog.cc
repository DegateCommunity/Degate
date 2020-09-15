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

#include "Core/Utils/DegateHelper.h"
#include "Core/Generator/CodeTemplateGenerator.h"
#include "Core/Generator/VHDLCodeTemplateGenerator.h"
#include "Core/Generator/VHDLTBCodeTemplateGenerator.h"
#include "Core/Generator/VerilogCodeTemplateGenerator.h"
#include "Core/Generator/VerilogTBCodeTemplateGenerator.h"
#include "GateEditDialog.h"
#include "TerminalDialog.h"

namespace degate
{
	// Entity tab

    GateEditEntityTab::GateEditEntityTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project)
            : QWidget(parent), gate(gate), fill_color(parent), frame_color(parent), project(project)
	{
		// Name
		name_label.setText(tr("Name:"));
		name.setText(QString::fromStdString(gate->get_name()));

		// Description
		description_label.setText(tr("Description:"));
		description.setText(QString::fromStdString(gate->get_description()));

		// Logic class
        logic_classes["undefined"] = tr("Undefined");
        logic_classes["inverter"] = tr("Inverter");
        logic_classes["tristate-inverter"] = tr("Tristate inverter");
        logic_classes["tristate-inverter-lo-active"] = tr("Tristate inverter (low active)");
        logic_classes["tristate-inverter-hi-active"] = tr("Tristate inverter (hight active)");
        logic_classes["nand"] = tr("NAND");
        logic_classes["nor"] = tr("NOR");
        logic_classes["and"] = tr("AND");
        logic_classes["or"] = tr("OR");
        logic_classes["xor"] = tr("XOR");
        logic_classes["xnor"] = tr("XNOR");
        logic_classes["buffer"] = tr("Buffer");
        logic_classes["buffer-tristate-lo-active"] = tr("Tristate buffer (low active)");
        logic_classes["buffer-tristate-hi-active"] = tr("Tristate buffer (high active)");
        logic_classes["latch-generic"] = tr("Latch (generic, transparent)");
        logic_classes["latch-sync-enable"] = tr("Latch (generic, with synchronous enable)");
        logic_classes["latch-async-enable"] = tr("Latch (generic, with asynchronous enable)");
        logic_classes["flipflop"] = tr("Flipflop (generic)");
        logic_classes["flipflop-sync-rst"] = tr("Flipflop (synchronous reset, edge-triggert)");
        logic_classes["flipflop-async-rst"] = tr("Flipflop (asynchronous reset, edge-triggert)");
        logic_classes["generic-combinational-logic"] = tr("Generic combinational logic");
        logic_classes["ao"] = tr("AND-OR");
        logic_classes["aoi"] = tr("AND-OR-Inverter");
        logic_classes["oa"] = tr("OR-AND");
        logic_classes["oai"] = tr("OR-AND-Inverter");
        logic_classes["isolation"] = tr("Isolation");
        logic_classes["half-adder"] = tr("Half adder");
        logic_classes["full-adder"] = tr("Full adder");
        logic_classes["mux"] = tr("Multiplexer");
        logic_classes["demux"] = tr("Demultiplexer");

		logic_class_label.setText(tr("Logic Class:"));

		for (auto& e : logic_classes)
            logic_class.addItem(e.second);

		logic_class.setCurrentText(logic_classes[gate->get_logic_class()]);

		// Ports list
		ports_label.setText(tr("Ports:"));
		ports.setColumnCount(6);
        ports.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		QStringList list;
		list.append(tr("ID"));
		list.append(tr("Name"));
		list.append(tr("Description"));
		list.append(tr("Color"));
		list.append(tr("In"));
		list.append(tr("Out"));
		ports.setHorizontalHeaderLabels(list);
		ports.resizeColumnsToContents();
		ports.resizeRowsToContents();

		update_ports_list();

		// Ports buttons
		ports_buttons_layout.addWidget(&ports_place);
		ports_buttons_layout.addWidget(&ports_add_button);
		ports_buttons_layout.addWidget(&ports_remove_button);
		ports_place.setText(tr("Place"));
		ports_add_button.setText(tr("Add"));
		ports_remove_button.setText(tr("Remove"));

		// Fill color
		fill_color_label.setText(tr("Fill color:"));
		fill_color_layout.addWidget(&fill_color);
		fill_color_layout.addWidget(&fill_color_reset_button);
		fill_color.set_color(gate->get_fill_color());
		fill_color_reset_button.setText(tr("Reset Color"));

		// Frame color
		frame_color_label.setText(tr("Frame color:"));
		frame_color_layout.addWidget(&frame_color);
		frame_color_layout.addWidget(&frame_color_reset_button);
		frame_color.set_color(gate->get_frame_color());
		frame_color_reset_button.setText(tr("Reset Color"));

		// Layout
		layout.addWidget(&name_label, 0, 0);
		layout.addWidget(&name, 0, 1);
		layout.addWidget(&description_label, 1, 0);
		layout.addWidget(&description, 1, 1);
		layout.addWidget(&logic_class_label, 2, 0);
		layout.addWidget(&logic_class, 2, 1);
		layout.addWidget(&ports_label, 3, 0);
		layout.addWidget(&ports, 3, 1);
		layout.addLayout(&ports_buttons_layout, 4, 1);
		layout.addWidget(&fill_color_label, 5, 0);
		layout.addLayout(&fill_color_layout, 5, 1);
		layout.addWidget(&frame_color_label, 6, 0);
		layout.addLayout(&frame_color_layout, 6, 1);

		setLayout(&layout);

		QObject::connect(&ports_place, SIGNAL(clicked()), this, SLOT(on_port_place()));
		QObject::connect(&ports_add_button, SIGNAL(clicked()), this, SLOT(add_port()));
		QObject::connect(&ports_remove_button, SIGNAL(clicked()), this, SLOT(remove_port()));

		QObject::connect(&fill_color_reset_button, SIGNAL(clicked()), this, SLOT(reset_fill_color()));
		QObject::connect(&frame_color_reset_button, SIGNAL(clicked()), this, SLOT(reset_frame_color()));
	}

	void GateEditEntityTab::validate()
	{
		if (gate == nullptr)
			return;

		gate->set_name(name.text().toStdString());
		gate->set_description(description.text().toStdString());

		unsigned index = 0;
		for (GateTemplate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter)
		{
			GateTemplatePort_shptr tmpl_port = *iter;

			// Name
			tmpl_port->set_name(ports.item(index, 1)->text().toStdString());

			// Description
			tmpl_port->set_description(ports.item(index, 2)->text().toStdString());

			// Color
			tmpl_port->set_fill_color(static_cast<ColorSelectionButton*>(ports.cellWidget(index, 3))->get_color());

			// In
			bool in;
			in = ports.item(index, 4)->checkState() == Qt::CheckState::Checked ? true : false;

			// Out
			bool out;
			out = ports.item(index, 5)->checkState() == Qt::CheckState::Checked ? true : false;

			// In/Out set
			if (in && !out)
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_IN);
			else if (!in && out)
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_OUT);
			else if (in && out)
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_INOUT);
			else
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_UNDEFINED);

			index++;
		}

        for (auto& e : logic_classes)
        {
            if (e.second == logic_class.currentText())
                gate->set_logic_class(e.first);
        }

		gate->set_fill_color(fill_color.get_color());
		gate->set_frame_color(frame_color.get_color());
	}

	void GateEditEntityTab::add_port() // Todo: even with cancel modifications will persist
	{
		validate();

		GateTemplatePort_shptr new_port(new GateTemplatePort());
        new_port->set_fill_color(project->get_default_color(DEFAULT_COLOR_GATE_PORT));
		new_port->set_object_id(project->get_logic_model()->get_new_object_id());
		gate->add_template_port(new_port);

		project->get_logic_model()->update_ports(gate);

		update_ports_list();
	}

	void GateEditEntityTab::remove_port() // Todo: even with cancel modifications will persist
	{
		validate();

		QItemSelectionModel* select = ports.selectionModel();

		if (!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for (auto & sel : index)
		{
			if (sel.isValid())
				gate->remove_template_port(ports.item(sel.row(), 0)->text().toInt());
		}

		project->get_logic_model()->update_ports(gate);

		update_ports_list();
	}

	void GateEditEntityTab::update_ports_list()
	{
		ports.setRowCount(0);

		for (GateTemplate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter)
		{
			GateTemplatePort_shptr tmpl_port = *iter;

			ports.insertRow(ports.rowCount());

			// Id
			QTableWidgetItem* id_item = new QTableWidgetItem(QString::number(tmpl_port->get_object_id()));
			id_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			ports.setItem(ports.rowCount() - 1, 0, id_item);

			// name and description
			ports.setItem(ports.rowCount() - 1, 1, new QTableWidgetItem(QString::fromStdString(tmpl_port->get_name())));
			ports.setItem(ports.rowCount() - 1, 2, new QTableWidgetItem(QString::fromStdString(tmpl_port->get_description())));

			// Color
			ColorSelectionButton* cb = new ColorSelectionButton(this);
			cb->set_color(tmpl_port->get_fill_color());
			ports.setCellWidget(ports.rowCount() - 1, 3, cb);

			// In
			QTableWidgetItem* in = new QTableWidgetItem();
			in->setCheckState(tmpl_port->is_inport() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
			ports.setItem(ports.rowCount() - 1, 4, in);

			// Out
			QTableWidgetItem* out = new QTableWidgetItem();
			out->setCheckState(tmpl_port->is_outport() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
			ports.setItem(ports.rowCount() - 1, 5, out);
		}

		ports.resizeColumnsToContents();
		ports.resizeRowsToContents();
	}

	void GateEditEntityTab::reset_fill_color()
	{
		fill_color.set_color(project->get_default_color(DEFAULT_COLOR_GATE));
	}

	void GateEditEntityTab::reset_frame_color()
	{
		frame_color.set_color(project->get_default_color(DEFAULT_COLOR_GATE_FRAME));
	}

	void GateEditEntityTab::on_port_place()  // Todo: even with cancel modifications will persist
	{
		validate();

		QItemSelectionModel* select = ports.selectionModel();

		if (!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for (auto & sel : index)
		{
			if (sel.isValid())
			{
				PortPlacementDialog dialog(this, project, gate, gate->get_template_port(ports.item(sel.row(), 0)->text().toInt()));
				dialog.exec();
			}
		}

		update_ports_list();
	}


	// Behaviour tab

    GateEditBehaviourTab::GateEditBehaviourTab(QWidget* parent,
                                               GateTemplate_shptr gate,
                                               Project_shptr project,
                                               GateEditEntityTab& entity_tab)
            : QWidget(parent), gate(gate), project(project), entity_tab(entity_tab)
    {
	    // Languages
        languages[tr("Free text")] = GateTemplate::TEXT;
        languages[tr("VHDL")] = GateTemplate::VHDL;
        languages[tr("VHDL/Testbench")] = GateTemplate::VHDL_TESTBENCH;
        languages[tr("Verilog")] = GateTemplate::VERILOG;
        languages[tr("Verilog/Testbench")] = GateTemplate::VERILOG_TESTBENCH;

        language_label.setText(tr("Language:"));

        for (auto& e : languages)
            language_selector.addItem(e.first);

        QObject::connect(&language_selector, SIGNAL(currentIndexChanged(int)), this, SLOT(on_language_selector_changed(int)));

        generate_button.setText(tr("Generate code template"));
        QObject::connect(&generate_button, SIGNAL(clicked()), this, SLOT(generate()));

        compile_button.setText(tr("Compile"));
        QObject::connect(&compile_button, SIGNAL(clicked()), this, SLOT(compile()));

        compile_save_button.setText(tr("Compile and Save"));
        QObject::connect(&compile_save_button, SIGNAL(clicked()), this, SLOT(compile_save()));

        buttons_layout.addWidget(&language_label);
        buttons_layout.addWidget(&language_selector);
        buttons_layout.addWidget(&generate_button);
        buttons_layout.addWidget(&compile_button);
        buttons_layout.addWidget(&compile_save_button);

        for (GateTemplate::implementation_iter iter = gate->implementations_begin(); iter != gate->implementations_end(); ++iter)
            code_text[iter->first] = iter->second;

        text_area.setText(QString::fromStdString(code_text[GateTemplate::IMPLEMENTATION_TYPE::TEXT]));
        old_index = GateTemplate::TEXT;

        // Default buttons state (selector on plain text by default)
        compile_button.setEnabled(false);
        compile_save_button.setEnabled(false);

        layout.addLayout(&buttons_layout);
        layout.addWidget(&text_area);

        setLayout(&layout);
	}

	void GateEditBehaviourTab::validate()
	{
        code_text[languages[language_selector.currentText()]] = text_area.toPlainText().toStdString();

        BOOST_FOREACH(code_text_map_type::value_type &p, code_text)
            gate->set_implementation(static_cast<GateTemplate::IMPLEMENTATION_TYPE>(p.first), p.second);
	}

    void GateEditBehaviourTab::generate()
    {
        CodeTemplateGenerator_shptr generator;

        if (code_text[languages[language_selector.currentText()]].length() != 0)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this,
                                          tr("Warning"),
                                          tr("Are you sure you want to replace the code?"),
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No)
                return;
        }

        auto index = languages[language_selector.currentText()];

        if (index == GateTemplate::VHDL)
        {
            generator = CodeTemplateGenerator_shptr(new VHDLCodeTemplateGenerator(entity_tab.name.text().toStdString().c_str(),
                                                                                  entity_tab.description.text().toStdString().c_str(),
                                                                                  entity_tab.logic_class.currentText().toStdString()));
        }
        else if (index == GateTemplate::VHDL_TESTBENCH)
        {
            generator = CodeTemplateGenerator_shptr(new VHDLTBCodeTemplateGenerator(entity_tab.name.text().toStdString().c_str(),
                                                                                    entity_tab.description.text().toStdString().c_str(),
                                                                                    entity_tab.logic_class.currentText().toStdString()));
        }
        else if (index == GateTemplate::VERILOG)
        {
            generator = CodeTemplateGenerator_shptr(new VerilogCodeTemplateGenerator(entity_tab.name.text().toStdString().c_str(),
                                                                                     entity_tab.description.text().toStdString().c_str(),
                                                                                     entity_tab.logic_class.currentText().toStdString()));
        }
        else if (index == GateTemplate::VERILOG_TESTBENCH)
        {
            generator = CodeTemplateGenerator_shptr(new VerilogTBCodeTemplateGenerator(entity_tab.name.text().toStdString().c_str(),
                                                                                       entity_tab.description.text().toStdString().c_str(),
                                                                                       entity_tab.logic_class.currentText().toStdString()));
        }
        else
        {
            return;
        }

        for (unsigned int i = 0; i < entity_tab.ports.rowCount(); i++)
        {
            generator->add_port(entity_tab.ports.item(i, 1)->text().toStdString(),
                                entity_tab.ports.item(i, 4)->checkState() == Qt::CheckState::Checked ? true : false);
        }

        try
        {
            std::string c = generator->generate();
            text_area.setText(QString::fromStdString(c));
            code_text[languages[language_selector.currentText()]] = c;
        }
        catch (std::exception const& e)
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to create a code template: %1").arg(e.what()));
        }
    }

    void GateEditBehaviourTab::compile()
    {
        auto index = languages[language_selector.currentText()];

        if (index == GateTemplate::VHDL || index == GateTemplate::VHDL_TESTBENCH)
        {
            show_vhdl_warning();

            return;
        }

        if (text_area.toPlainText().length() == 0)
        {
            show_missing_code_warning();

            return;
        }

	    if (index == GateTemplate::VERILOG)
        {
            int result = std::system("iverilog -V");
            if (result != 0)
            {
                show_iverilog_warning();

                return;
            }

	        std::string dir = create_temp_directory();
            std::string in_file = join_pathes(dir, "cell.v");
            std::string out_file = join_pathes(dir, "cell");
            std::ofstream file;

            degate::write_string_to_file(in_file, text_area.toPlainText().toStdString());

            TerminalCommands cmd;
            QStringList args;
            args << "-v" << "-o" << QString::fromStdString(out_file) << QString::fromStdString(in_file);
            cmd.push_back({"iverilog", args});

            TerminalDialog terminal(this, cmd);
            terminal.start();
            terminal.exec();

            remove_directory(dir);
        }
	    else if (index == GateTemplate::VERILOG_TESTBENCH)
        {
	        if (code_text[GateTemplate::VERILOG].size() == 0)
            {
                show_missing_verilog_warning();

                return;
            }

            int iverilog_result = std::system("iverilog -V");
            int gtkwave_result = std::system("gtkwave -V");
            if (iverilog_result != 0 || gtkwave_result != 0)
            {
                show_iverilog_gtkwave_warning();

                return;
            }

            std::string dir = create_temp_directory();
            std::string in_file = join_pathes(dir, "cell.v");
            std::string tb_file = join_pathes(dir, "test_cell.v");

            std::string file_list_file = join_pathes(dir, "combinded.v");
            std::string out_file = join_pathes(dir, "cell");

            write_string_to_file(in_file, code_text[GateTemplate::VERILOG]);
            write_string_to_file(tb_file, text_area.toPlainText().toStdString());
            write_string_to_file(file_list_file, in_file + "\n");

            TerminalCommands cmd;
            QStringList args;

            args << "-v" << "-o" << QString::fromStdString(out_file) << "-f" << QString::fromStdString(file_list_file) << QString::fromStdString(tb_file);
            cmd.push_back({"iverilog", args});
            args.clear();

            args << QString::fromStdString(out_file);
            cmd.push_back({"vvp", args});
            args.clear();

            //WARNING: the path "test.vcd" to the simulation file (that gtkwave will use) is hardcoded in the VerilogTBCodeTemplateGenerator (included in the generation code with tag "$dumpfile("test.vcd");")
            args << "test.vcd";
            cmd.push_back({"gtkwave", args});
            args.clear();

            TerminalDialog terminal(this, cmd);
            terminal.start();
            terminal.exec();

            remove_file("test.vcd");
            remove_directory(dir);
        }
    }

    void GateEditBehaviourTab::compile_save()
    {
        auto index = languages[language_selector.currentText()];

        if (index == GateTemplate::VHDL || index == GateTemplate::VHDL_TESTBENCH)
        {
            show_vhdl_warning();

            return;
        }

        if (text_area.toPlainText().length() == 0)
        {
            show_missing_code_warning();

            return;
        }

        if (index == GateTemplate::VERILOG)
        {
            int result = std::system("iverilog -V");
            if (result != 0)
            {
                show_iverilog_warning();

                return;
            }

            QString dir = QFileDialog::getExistingDirectory(this, tr("Select folder to save in"));

            if (dir.isNull())
                return;

            std::string in_file = join_pathes(dir.toStdString(), "cell.v");
            std::string out_file = join_pathes(dir.toStdString(), "cell");
            std::ofstream file;

            degate::write_string_to_file(in_file, text_area.toPlainText().toStdString());

            TerminalCommands cmd;
            QStringList args;

            args << "-v" << "-o" << QString::fromStdString(out_file) << QString::fromStdString(in_file);
            cmd.push_back({"iverilog", args});
            args.clear();

            TerminalDialog terminal(this, cmd);
            terminal.start();
            terminal.exec();
        }
        else if (index == GateTemplate::VERILOG_TESTBENCH)
        {
            if (code_text[GateTemplate::VERILOG].size() == 0)
            {
                show_missing_verilog_warning();

                return;
            }

            int iverilog_result = std::system("iverilog -V");
            int gtkwave_result = std::system("gtkwave -V");
            if (iverilog_result != 0 || gtkwave_result != 0)
            {
                show_iverilog_gtkwave_warning();

                return;
            }

            QString dir = QFileDialog::getExistingDirectory(this, tr("Select folder to save in"));

            if (dir.isNull())
                return;

            std::string in_file = join_pathes(dir.toStdString(), "cell.v");
            std::string tb_file = join_pathes(dir.toStdString(), "test_cell.v");

            std::string file_list_file = join_pathes(dir.toStdString(), "combinded.v");
            std::string out_file = join_pathes(dir.toStdString(), "cell");

            write_string_to_file(in_file, code_text[GateTemplate::VERILOG]);
            write_string_to_file(tb_file, text_area.toPlainText().toStdString());
            write_string_to_file(file_list_file, in_file + "\n");

            TerminalCommands cmd;
            QStringList args;

            args << "-v" << "-o" << QString::fromStdString(out_file) << "-f" << QString::fromStdString(file_list_file) << QString::fromStdString(tb_file);
            cmd.push_back({"iverilog", args});
            args.clear();

            args << QString::fromStdString(out_file);
            cmd.push_back({"vvp", args});
            args.clear();

            //WARNING: the path "test.vcd" to the simulation file (that gtkwave will use) is hardcoded in the VerilogTBCodeTemplateGenerator (included in the generation code with tag "$dumpfile("test.vcd");")
            args << "test.vcd";
            cmd.push_back({"gtkwave", args});
            args.clear();

            TerminalDialog terminal(this, cmd);
            terminal.start();
            terminal.exec();

            move_file("test.vcd", join_pathes(dir.toStdString(), "test.vcd"));
        }
    }

    void GateEditBehaviourTab::on_language_selector_changed(int index)
    {
        code_text[old_index] = text_area.toPlainText().toStdString();

        text_area.setText(QString::fromStdString(code_text[languages[language_selector.currentText()]]));
        old_index = languages[language_selector.currentText()];

        if (languages[language_selector.currentText()] == GateTemplate::TEXT)
        {
            compile_button.setDisabled(true);
            compile_save_button.setDisabled(true);
        }
        else
        {
            compile_button.setDisabled(false);
            compile_save_button.setDisabled(false);
        }

        if (languages[language_selector.currentText()] == GateTemplate::VERILOG_TESTBENCH)
        {
            compile_button.setText(tr("Compile and Run"));
        }
        else
        {
            compile_button.setText(tr("Compile"));
        }
    }

    void GateEditBehaviourTab::show_iverilog_warning()
    {
        const QString message = "<html><center>" +
                                tr("You must install %1 to compile.") + "<br>"
                                "Linux: <a href='http://iverilog.icarus.com/'>icarus</a> <br>"
                                "Windows: <a href='https://bleyer.org/icarus/'>icarus</a> <br>"
                                "</center></html>";
        QMessageBox::warning(this, tr("Warning"), message.arg("<strong> iverilog </strong>"));
    }

    void GateEditBehaviourTab::show_iverilog_gtkwave_warning()
    {
        const QString message = "<html><center>" +
                                tr("You must install %1 and %2 to compile and run.") + "<br>"
                                "Linux: <a href='http://iverilog.icarus.com/'>icarus</a> " + tr("and") + " <a href='https://sourceforge.net/projects/gtkwave/files/'>gtkwave</a> <br>"
                                "Windows: <a href='https://bleyer.org/icarus/'>icarus " + tr("and") + " gtkwave</a> <br>"
                                "</center></html>";
        QMessageBox::warning(this, tr("Warning"), message.arg("<strong> iverilog </strong>").arg("<strong> gtkwave </strong>"));
    }

    void GateEditBehaviourTab::show_missing_verilog_warning()
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("You must write the Verilog code for the standard cell's behaviour first."));
    }

    void GateEditBehaviourTab::show_missing_code_warning()
    {
        QMessageBox::warning(this, tr("Warning"), tr("You need to add code before compiling."));
    }

    void GateEditBehaviourTab::show_vhdl_warning()
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("You can't compile or run VHDL code within Degate, use Verilog language instead."));
    }


    // Layout tab

    GateEditLayoutTab::GateEditLayoutTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project)
            : QWidget(parent), gate(gate), project(project)
	{
		if (gate->has_image(Layer::METAL))
		{
			metal_label.setText(tr("Metal:"));
			metal = new ImageRenderer(this, gate->get_image(Layer::METAL));
			metal_layout.addWidget(&metal_label);
			metal_layout.addWidget(metal);
			layout.addLayout(&metal_layout);
		}

		if (gate->has_image(Layer::LOGIC))
		{
			logic_label.setText(tr("Logic:"));
			logic = new ImageRenderer(this, gate->get_image(Layer::LOGIC));
			logic_layout.addWidget(&logic_label);
			logic_layout.addWidget(logic);
			layout.addLayout(&logic_layout);
		}

		if (gate->has_image(Layer::TRANSISTOR))
		{
			transistor_label.setText(tr("Transistor:"));
			transistor = new ImageRenderer(this, gate->get_image(Layer::TRANSISTOR));
			transistor_layout.addWidget(&transistor_label);
			transistor_layout.addWidget(transistor);
			layout.addLayout(&transistor_layout);
		}

		setLayout(&layout);
	}

	GateEditLayoutTab::~GateEditLayoutTab()
	{
		if (transistor != nullptr)
			delete transistor;

		if (logic != nullptr)
			delete logic;

		if (metal != nullptr)
			delete metal;
	}

	void GateEditLayoutTab::validate()
	{
	}


	// Main dialog

    GateEditDialog::GateEditDialog(QWidget* parent, GateTemplate_shptr gate, Project_shptr project)
            : QDialog(parent),
              gate(gate),
              button_box(QDialogButtonBox::Ok),
              entity_tab(parent, gate, project),
              behaviour_tab(parent, gate, project, entity_tab),
              layout_tab(parent, gate, project),
              project(project)
	{
		setWindowTitle(tr("Edit gate"));
        setWindowFlags(Qt::Window);

		tab.addTab(&entity_tab, tr("Entity"));
		tab.addTab(&behaviour_tab, tr("Behaviour"));
		tab.addTab(&layout_tab, tr("Layout"));

		layout.addWidget(&tab);
		layout.addWidget(&button_box);

		setLayout(&layout);

		QObject::connect(&button_box, SIGNAL(accepted()), this, SLOT(validate()));
	}

	void GateEditDialog::validate()
	{
		entity_tab.validate();
		behaviour_tab.validate();
		layout_tab.validate();

		accept();
	}

    GateInstanceEditDialog::GateInstanceEditDialog(QWidget* parent, Gate_shptr gate, Project_shptr project)
            : GateEditDialog(parent, gate->get_gate_template(), project), gate(gate)
	{
		setWindowTitle(tr("Edit gate instance"));
        setWindowFlags(Qt::Window);

        instance_label.setText(tr("Gate instance edition:"));

        // Orientation
        orientations[Gate::ORIENTATION_UNDEFINED] = tr("Undefined");
        orientations[Gate::ORIENTATION_NORMAL] = tr("Normal");
        orientations[Gate::ORIENTATION_FLIPPED_UP_DOWN] = tr("Flipped up and down");
        orientations[Gate::ORIENTATION_FLIPPED_LEFT_RIGHT] = tr("Flipped left and right");
        orientations[Gate::ORIENTATION_FLIPPED_BOTH] = tr("Flipped both");

		orientation_label.setText(tr("Orientation:"));

		for (auto& e : orientations)
            orientation_edit.addItem(e.second);

        orientation_edit.setCurrentText(orientations[gate->get_orientation()]);

		// Name
        name_label.setText(tr("Name:"));
        name_edit.setText(QString::fromStdString(gate->get_name()));

        // Description
        description_label.setText(tr("Description:"));
        description_edit.setText(QString::fromStdString(gate->get_description()));

        instance_layout.addWidget(&orientation_label, 0, 0);
        instance_layout.addWidget(&orientation_edit, 0, 1);
        instance_layout.addWidget(&name_label, 1, 0);
        instance_layout.addWidget(&name_edit, 1, 1);
        instance_layout.addWidget(&description_label, 2, 0);
        instance_layout.addWidget(&description_edit, 2, 1);

        gate_template_label.setText(tr("Gate template edition:"));

        layout.insertWidget(0, &instance_label);
        layout.insertSpacing(1, 10);
        layout.insertLayout(2, &instance_layout);
		layout.insertSpacing(3, 20);
        layout.insertWidget(4, &gate_template_label);
	}

	void GateInstanceEditDialog::validate()
	{
	    for (auto& e : orientations)
        {
	        if (e.second == orientation_edit.currentText())
                gate->set_orientation(e.first);
        }

		gate->set_name(name_edit.text().toStdString());
        gate->set_description(description_edit.text().toStdString());

		GateEditDialog::validate();
	}
}
