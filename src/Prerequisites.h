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

#ifndef __PREREQUISITES_H__
#define __PREREQUISITES_H__

/* System */
#if ((!defined(_WIN32) && !defined(_WIN64)) && (defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__) || defined(__linux__)))
#define SYS_UNIX
#elif (defined(_WIN32) || defined(_WIN64))
#define SYS_WINDOWS
#else
#define SYS_UNDEFINED
#endif

#if (defined(__APPLE__) || defined(__MACH__))
#define SYS_APPLE
#elif defined(__linux__)
#define SYS_LINUX
#endif


/* Compiler */
#if defined(__GNUC__)
#define COMP_GCC
#elif defined(__MINGW32__)
#define COMP_MINGW32
#elif defined(__clang__)
#define COMP_CLANG
#elif defined(_MSC_VER)
#define COMP_MSC
#else
#define COMP_UNDEFINED
#endif

#define XML_ENCODING "version='1.0' encoding='UTF-8'" // old encoding : ISO-8859-1

#endif
