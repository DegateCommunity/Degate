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

#ifndef __GATEEDITDIALOG_H__
#define __GATEEDITDIALOG_H__

#include "Core/Project/Project.h"
#include "ColorPickerDialog.h"
#include "PortPlacementDialog.h"

#include <map>
#include <QDialog>
#include <QWidget>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QColorDialog>
#include <QTableWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace degate
{

    class GateEditBehaviourTab;

    /**
     * @class GateEditEntityTab
     * @brief The entity tab of the gate edit dialog.
     *
     * This tab allow the edition of all the properties of the gate template.
     *
     * @see GateEditDialog
     */
    class GateEditEntityTab : public QWidget
    {
        Q_OBJECT

        friend GateEditBehaviourTab;

    public:

        /**
         * Create the entity tab.
         *
         * @param parent : the parent of the tab.
         * @param gate : the gate template to edit.
         * @param project : the current active project.
         */
        GateEditEntityTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
        ~GateEditEntityTab() override = default;

    public slots:
        /**
         * Save all changes.
         */
        void validate();

    private slots:

        /**
         * Add a port to the gate template (can't cancel this action).
         */
        void add_port();

        /**
         * Remove a port of the gate template (can't cancel this action).
         */
        void remove_port();

        /**
         * Update the ports list.
         */
        void update_ports_list();

        /**
         * Reset the fill color.
         */
        void reset_fill_color();

        /**
         * Reset the frame color (outline).
         */
        void reset_frame_color();

        /**
         * Open a new port placement window for the gate template.
         */
        void on_port_place();

    private:
        GateTemplate_shptr gate = nullptr;
        Project_shptr project = nullptr;
        QGridLayout layout;

        // Name
        QLabel name_label;
        QLineEdit name;

        // Description
        QLabel description_label;
        QLineEdit description;

        // Logic class
        QLabel logic_class_label;
        QComboBox logic_class;

        // Ports
        QLabel ports_label;
        QTableWidget ports;
        QHBoxLayout ports_buttons_layout;
        QPushButton ports_add_button;
        QPushButton ports_remove_button;
        QPushButton ports_place;

        // Fill color
        QLabel fill_color_label;
        ColorSelectionButton fill_color;
        QHBoxLayout fill_color_layout;
        QPushButton fill_color_reset_button;

        // Frame color
        QLabel frame_color_label;
        ColorSelectionButton frame_color;
        QHBoxLayout frame_color_layout;
        QPushButton frame_color_reset_button;

        std::map<std::string, QString> logic_classes;

    };

    /**
     * @class GateEditBehaviourTab
     * @brief The behaviour tab of the gate edit dialog.
     *
     * This tab allow the definition of the behaviour of the gate (VHDL/Verilog).
     *
     * @see GateEditDialog
     */
    class GateEditBehaviourTab : public QWidget
    {
        Q_OBJECT

    public:

        /**
         * Create the behaviour tab.
         *
         * @param parent : the parent of the tab.
         * @param gate : the gate template to edit.
         * @param project : the current active project.
         */
        GateEditBehaviourTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project, GateEditEntityTab& entity_tab);
        ~GateEditBehaviourTab() override = default;

    public slots:
        /**
         * Save all changes.
         */
        void validate();

        /**
         * Generate code template for a specific language (defined with language_selector).
         */
        void generate();

        /**
         * Compile the code in the text area (for verilog only with iverilog) in a temporary directory.
         * It will open a degate terminal dialog and, if verilog testbench language is selected, run gtkwave.
         */
        void compile();

        /**
         * Compile the code in the text area (for verilog only with iverilog) and save all files in a specific directory.
         * It will open a degate terminal dialog and, if verilog testbench language is selected, run gtkwave.
         */
        void compile_save();

        /**
         * Update the text are regarding the language selector.
         *
         * @param index : new index.
         */
        void on_language_selector_changed(int index);

    protected:
        /**
         * Show warning for missing iverilog.
         */
        void show_iverilog_warning();

        /**
         * Show warning for missing iverilog and/or gtkwave.
         */
        void show_iverilog_gtkwave_warning();

        /**
         * Show warning for missing verilog code.
         */
        void show_missing_verilog_warning();

        /**
         * Show warning for missing code before compiling.
         */
        void show_missing_code_warning();

        /**
         * Show warning for trying to run/compile VHDL.
         */
        void show_vhdl_warning();

    private:
        GateTemplate_shptr gate = nullptr;
        Project_shptr project = nullptr;
        GateEditEntityTab& entity_tab;

        // Layouts
        QVBoxLayout layout;
        QHBoxLayout buttons_layout;

        // Elements
        QLabel language_label;
        QComboBox language_selector;
        QPushButton generate_button;
        QPushButton compile_button;
        QPushButton compile_save_button;
        QTextEdit text_area;

        // Code text map
        typedef std::map<int, std::string> code_text_map_type;
        code_text_map_type code_text;

        // Old index on change, actual index after change.
        int old_index;

        std::map<QString, GateTemplate::IMPLEMENTATION_TYPE> languages;
    };


    /**
     * @class GateEditLayoutTab
     * @brief The layout tab of the gate edit dialog.
     *
     * This tab show the available images of the gate template on different layers.
     *
     * @see GateEditDialog
     */
    class GateEditLayoutTab : public QWidget
    {
        Q_OBJECT

    public:

        /**
         * Create the layout tab.
         *
         * @param parent : the parent of the tab.
         * @param gate : the gate template to edit.
         * @param project : the current active project.
         */
        GateEditLayoutTab(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
        ~GateEditLayoutTab() override;

    public slots:
        /**
         * Save all changes.
         */
        void validate();

    private:
        GateTemplate_shptr gate = nullptr;
        Project_shptr project = nullptr;

        QVBoxLayout layout;

        // Metal layer
        QHBoxLayout metal_layout;
        QLabel metal_label;
        ImageRenderer* metal = nullptr;

        // Logic layer
        QHBoxLayout logic_layout;
        QLabel logic_label;
        ImageRenderer* logic = nullptr;

        // Transistor layer
        QHBoxLayout transistor_layout;
        QLabel transistor_label;
        ImageRenderer* transistor = nullptr;

    };

    /**
     * @class GateEditDialog
     * @brief Dialog to edit a gate template.
     *
     * This window is composed of three tabs (@see GateEditEntityTab, @see GateEditBehaviourTab and @see GateEditLayoutTab).
     *
     * @see QDialog
     */
    class GateEditDialog : public QDialog
    {
        Q_OBJECT

    public:

        /**
         * Create a gate template edit dialog, to show it call the exec function.
         *
         * @param parent : the parent of the dialog.
         * @param gate : the gate template to edit.
         * @param project : the current active project.
         *
         * @see QDialog
         */
        GateEditDialog(QWidget* parent, GateTemplate_shptr gate, Project_shptr project);
        ~GateEditDialog() override = default;

    public slots:
        /**
         * Save all changes.
         */
        virtual void validate();

    protected:
        QDialogButtonBox button_box;
        QVBoxLayout layout;
        Project_shptr project = nullptr;

    private:
        GateTemplate_shptr gate = nullptr;

        QTabWidget tab;

        // Tabs
        GateEditEntityTab entity_tab;
        GateEditBehaviourTab behaviour_tab;
        GateEditLayoutTab layout_tab;
    };

    /**
     * @class GateInstanceEditDialog
     * @brief Dialog to edit a gate instance (the gate orientation and his attached gate template).
     *
     * This window is composed of three tabs (@see GateEditEntityTab, @see GateEditBehaviourTab and @see GateEditLayoutTab) and an orientation selector.
     *
     * @see QDialog
     */
    class GateInstanceEditDialog : public GateEditDialog
    {
        Q_OBJECT

    public:

        /**
         * Create a gate instance edit dialog, to show it call the exec function.
         *
         * @param parent : the parent of the dialog.
         * @param gate : the gate to edit.
         * @param project : the current active project.
         *
         * @see QDialog
         */
        GateInstanceEditDialog(QWidget* parent, Gate_shptr gate, Project_shptr project);
        ~GateInstanceEditDialog() override = default;

    public slots:
        /**
         * Save all changes.
         */
        void validate() override;

    private:
        Gate_shptr gate = nullptr;
        QGridLayout instance_layout;

        QLabel instance_label;

        // Orientation
        QLabel orientation_label;
        QComboBox orientation_edit;

        // Name
        QLabel name_label;
        QLineEdit name_edit;

        // Description
        QLabel description_label;
        QLineEdit description_edit;

        // Other
        QLabel gate_template_label;

        std::map<Gate::ORIENTATION, QString> orientations;
    };
}

#endif