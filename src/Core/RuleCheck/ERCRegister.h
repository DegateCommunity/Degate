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
#ifndef __ERCREGISTER_H__
#define __ERCREGISTER_H__

#include "Core/Primitive/SingletonBase.h"
#include "RCViolation.h"

#include "ERC/ERCNet.h"
#include "ERC/ERCOpenPorts.h"

#include <map>
#include <vector>

/**
 * Register a new ERC.
 * @param vclass : violation class (using QT_TR_NOOP("class")).
 * @param verc : the corresponding ERC class that handle this violation class.
 */
#define REGISTER_ERC(vclass, verc) {                                                     \
                                        RCBase_shptr object = nullptr;                   \
                                        for (auto& e : erc)                              \
                                        {                                                \
                                             if (std::dynamic_pointer_cast<verc>(e))     \
                                                 object = e;                             \
                                        }                                                \
                                        if (object == nullptr)                           \
                                        {                                                \
                                             erc.push_back(std::make_shared<verc>());    \
                                             object = erc.back();                        \
                                        }                                                \
                                        erc_register[vclass] = { vclass, object };       \
                                    }

namespace degate
{
    /**
     * @struct ERCRegisterEntry
     * @brief Describe an ERC register entry.
     */
    struct ERCRegisterEntry
    {
        const char* violation_class; /*!< The violation class of the entry. */
        std::weak_ptr<RCBase> erc;   /*!< The corresponding ERC instance/class. */
    };

    /**
     * @class ERCRegister
     * @brief Singleton class to register ERC.
     */
    class ERCRegister : public SingletonBase<ERCRegister>
    {
    Q_DECLARE_TR_FUNCTIONS(degate::ERCRegister)

    public:
        ERCRegister()
        {
            /////////////////////////////////
            // Register ERC here.
            /////////////////////////////////

            REGISTER_ERC(QT_TR_NOOP("open_port"), ERCOpenPorts)
            REGISTER_ERC(QT_TR_NOOP("net.undefined_port_direction"), ERCNet)
            REGISTER_ERC(QT_TR_NOOP("net.not_feeded"), ERCNet)
            REGISTER_ERC(QT_TR_NOOP("net.outputs_connected"), ERCNet)
        }

        ~ERCRegister() override = default;

        /**
         * Generate the description of a violation regarding the tuple violation class + object.
         *
         * @param violation : the violation.
         * @return Returns a translated and newly generated description for the violation.
         */
        std::string generate_description(const RCViolation& violation)
        {
            return erc_register[violation.get_rc_violation_class()].erc.lock()->generate_description(violation);
        }

        /**
         * Get the translated version of the violation class of a violation.
         *
         * @param violation : the violation
         * @return Returns the translated version of the violation class of a violation.
         */
        std::string get_translated_violation_class(const RCViolation& violation)
        {
            return tr(erc_register[violation.get_rc_violation_class()].violation_class).toStdString();
        }

        /**
         * Get a list of registered ERC.
         *
         * @return Returns a list of registered ERC.
         */
        std::vector<RCBase_shptr> get_erc_list() const
        {
            return erc;
        }

    private:
        std::map<std::string, ERCRegisterEntry> erc_register;
        std::vector<RCBase_shptr> erc;

    };

#define ERC_REGISTER ERCRegister::get_instance()
}

#endif //__ERCREGISTER_H__
