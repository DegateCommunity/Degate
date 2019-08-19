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

#include "GateEditDialog.h"

namespace degate
{
	// Entity tab

	GateEditEntityTab::GateEditEntityTab(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project) : QWidget(parent), 
																													gate(gate_to_edit), 
																													fill_color(parent), 
																													frame_color(parent), 
																													project(project)
	{
		// Name
		name_label.setText("Name :");
		name.setText(QString::fromStdString(gate_to_edit->get_name()));

		// Description
		description_label.setText("Description :");
		description.setText(QString::fromStdString(gate_to_edit->get_description()));

		// Logic class
		logic_class_label.setText("Logic Class :");
		logic_class.addItem("undefined");
		logic_class.addItem("inverter");
		logic_class.addItem("tristate-inverter");
		logic_class.addItem("tristate-inverter-lo-active");
		logic_class.addItem("tristate-inverter-hi-active");
		logic_class.addItem("nand");
		logic_class.addItem("nor");
		logic_class.addItem("and");
		logic_class.addItem("or");
		logic_class.addItem("xor");
		logic_class.addItem("xnor");
		logic_class.addItem("buffer");
		logic_class.addItem("buffer-tristate-lo-active", "Tristate buffer (low active)");
		logic_class.addItem("buffer-tristate-hi-active", "Tristate buffer (high active)");
		logic_class.addItem("latch-generic", "latch (generic, transparent)");
		logic_class.addItem("latch-sync-enable", "latch (generic, with synchronous enable)");
		logic_class.addItem("latch-async-enable", "latch (generic, with asynchronous enable)");
		logic_class.addItem("flipflop", "flipflop (generic)");
		logic_class.addItem("flipflop-sync-rst", "flipflop (synchronous reset, edge-triggert)");
		logic_class.addItem("flipflop-async-rst", "flipflop (asynchronous reset, edge-triggert)");
		logic_class.addItem("generic-combinational-logic", "generic combinational logic");
		logic_class.addItem("ao", "and-or");
		logic_class.addItem("aoi", "and-or-inverter");
		logic_class.addItem("oa", "or-and");
		logic_class.addItem("oai", "or-and-inverter");
		logic_class.addItem("isolation");
		logic_class.addItem("half-adder");
		logic_class.addItem("full-adder");
		logic_class.addItem("mux");
		logic_class.addItem("demux");
		logic_class.setCurrentText(QString::fromStdString(gate_to_edit->get_logic_class()));

		// Ports list
		ports_label.setText("Ports :");
		ports.setColumnCount(6);
		QStringList list;
		list.append("ID");
		list.append("Name");
		list.append("Description");
		list.append("Color");
		list.append("In");
		list.append("Out");
		ports.setHorizontalHeaderLabels(list);
		ports.resizeColumnsToContents();
		ports.resizeRowsToContents();

		update_ports_list();

		// Ports buttons
		ports_buttons_layout.addWidget(&ports_place);
		ports_buttons_layout.addWidget(&ports_add_button);
		ports_buttons_layout.addWidget(&ports_remove_button);
		ports_place.setText("Place");
		ports_add_button.setText("Add");
		ports_remove_button.setText("Remove");

		// Fill color
		fill_color_label.setText("Fill color :");
		fill_color_layout.addWidget(&fill_color);
		fill_color_layout.addWidget(&fill_color_reset_button);
		fill_color.set_color(gate->get_fill_color());
		fill_color_reset_button.setText("Reset Color");

		// Frame color
		frame_color_label.setText("Frame color :");
		frame_color_layout.addWidget(&frame_color);
		frame_color_layout.addWidget(&frame_color_reset_button);
		frame_color.set_color(gate->get_frame_color());
		frame_color_reset_button.setText("Reset Color");

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

	GateEditEntityTab::~GateEditEntityTab()
	{
	}

	void GateEditEntityTab::validate()
	{
		if(gate == NULL)
			return;

		gate->set_name(name.text().toStdString());
		gate->set_description(description.text().toStdString());

		unsigned index = 0;
		for(GateTemplate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter)
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
			if(in && !out)
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_IN);
			else if(!in && out)
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_OUT);
			else if(in && out)
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_INOUT);
			else
				tmpl_port->set_port_type(GateTemplatePort::PORT_TYPE_UNDEFINED);

			index++;
		}

		gate->set_logic_class(logic_class.currentText().toStdString());
		gate->set_fill_color(fill_color.get_color());
		gate->set_frame_color(frame_color.get_color());
	}

	void GateEditEntityTab::add_port() // Todo: even with cancel modifications will persist
	{
		validate();

		GateTemplatePort_shptr new_port(new GateTemplatePort());
		new_port->set_object_id(project->get_logic_model()->get_new_object_id());
		gate->add_template_port(new_port);

		project->get_logic_model()->update_ports(gate);
		
		update_ports_list();
	}

	void GateEditEntityTab::remove_port() // Todo: even with cancel modifications will persist
	{
		validate();

		QItemSelectionModel* select = ports.selectionModel();

		if(!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for(auto sel = index.begin(); sel != index.end(); ++sel)
		{
			if(sel->isValid())
				gate->remove_template_port(ports.item(sel->row(), 0)->text().toInt());
		}

		project->get_logic_model()->update_ports(gate);

		update_ports_list();
	}

	void GateEditEntityTab::update_ports_list()
	{
		ports.setRowCount(0);

		for(GateTemplate::port_iterator iter = gate->ports_begin(); iter != gate->ports_end(); ++iter)
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

		if(!select->hasSelection())
			return;

		QModelIndexList index = select->selectedRows();

		for(auto sel = index.begin(); sel != index.end(); ++sel)
		{
			if(sel->isValid())
			{
				PortPlacementDialog dialog(this, project, gate, gate->get_template_port(ports.item(sel->row(), sel->column())->text().toInt()));
				dialog.exec();
			}
		}

		update_ports_list();
	}


	// Behaviour tab

	GateEditBehaviourTab::GateEditBehaviourTab(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project) : QWidget(parent), gate(gate_to_edit), project(project)
	{
	}

	GateEditBehaviourTab::~GateEditBehaviourTab()
	{
	}

	void GateEditBehaviourTab::validate()
	{
	}


	// Layout tab

	GateEditLayoutTab::GateEditLayoutTab(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project) : QWidget(parent), gate(gate_to_edit), project(project)
	{
	}

	GateEditLayoutTab::~GateEditLayoutTab()
	{
	}

	void GateEditLayoutTab::validate()
	{
	}


	// Main dialog

	GateEditDialog::GateEditDialog(QWidget* parent, GateTemplate_shptr gate_to_edit, Project_shptr project) : QDialog(parent), 
																											  gate(gate_to_edit), 
																											  button_box(QDialogButtonBox::Ok), 
																											  entity_tab(parent, gate_to_edit, project), 
																											  behaviour_tab(parent, gate_to_edit, project), 
																											  layout_tab(parent, gate_to_edit, project),
																											  project(project)
	{
		setWindowTitle("Edit gate");

		tab.addTab(&entity_tab, "Entity");
		tab.addTab(&behaviour_tab, "Behaviour");
		tab.addTab(&layout_tab, "Layout");

		layout.addWidget(&tab);
		layout.addWidget(&button_box);

		setLayout(&layout);

		QObject::connect(&button_box, SIGNAL(accepted()), this, SLOT(validate()));
	}

	GateEditDialog::~GateEditDialog()
	{
	}

	void GateEditDialog::validate()
	{
		entity_tab.validate();
		behaviour_tab.validate();
		layout_tab.validate();

		close();
	}

	GateInstanceEditDialog::GateInstanceEditDialog(QWidget* parent, Gate_shptr gate, Project_shptr project) : GateEditDialog(parent, gate->get_gate_template(), project), gate(gate)
	{
		orientation_label.setText("Gate instance orientation :");
		orientation.addItem("undefined");
		orientation.addItem("normal");
		orientation.addItem("flipped-up-down");
		orientation.addItem("flipped-left-right");
		orientation.addItem("flipped-both");
		orientation.setCurrentText(QString::fromStdString(gate->get_orienation_type_as_string()));

		layout.insertWidget(0, &orientation);
		layout.insertSpacing(1, 10);
	}

	GateInstanceEditDialog::~GateInstanceEditDialog()
	{
	}

	void GateInstanceEditDialog::validate()
	{
		if(orientation.currentText() == "undefined")
			gate->set_orientation(Gate::ORIENTATION_UNDEFINED);
		else if(orientation.currentText() == "normal")
			gate->set_orientation(Gate::ORIENTATION_NORMAL);
		else if(orientation.currentText() == "flipped-up-down")
			gate->set_orientation(Gate::ORIENTATION_FLIPPED_UP_DOWN);
		else if(orientation.currentText() == "flipped-left-right")
			gate->set_orientation(Gate::ORIENTATION_FLIPPED_LEFT_RIGHT);
		else if(orientation.currentText() == "flipped-both")
			gate->set_orientation(Gate::ORIENTATION_FLIPPED_BOTH);
		else
			gate->set_orientation(Gate::ORIENTATION_UNDEFINED);

		GateEditDialog::validate();
	}
}
