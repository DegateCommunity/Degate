``fensterbrief`` is a python script for working with letters based on LaTeX and
markdown.

.. contents:: 
   :local:

About
==================

Degate is a software for semi-automatic VLSI reverse engineering of digital logic
in chips. Please visit the project website to learn more about Degate.

  http://degate.org


Installation
=============

You need cmake ≥2.6 in order to generate the makefiles, else you will get an error message like Unknown CMake command "FIND_PACKAGE_HANDLE_STANDARD_ARGS" and the cmake version check may fail, too.

Install dependencies on Ubuntu
------------------------------

On a freshly installed Ubuntu it should be sufficient to install these packages:

::

   > sudo apt-get install cmake libboost-filesystem-dev \
        libboost-system-dev libboost-thread-dev \
        libboost-program-options-dev \
	libcppunit-dev \
	doxygen g++ gdb libgtkmm-2.4-dev libglademm-2.4-dev libxml++2.6-dev \
        libfreetype6-dev libzip-dev libgtkglextmm-x11-1.2-dev \
        libxmlrpc-c++8-dev libtiff-dev libpng-dev libcurl4-gnutls-dev


Install dependencies on Mac OS X
----------------------------------

You need the Mac Ports (or whatever the recent port system is) and Xcode (≥ 3.1) in order to build gtkmm and related libraries. If you have installed it, you can start your terminal and run:

::
   
   > sudo port install gtkmm cmake boost cppunit doxygen freetype \
     libglademm libxmlxx2 libzip xmlrpc-c gtkglextmm gnome-icon-theme


Compile degate
----------------

Get sources for Degate. Go into the main directory and type:

::

   > cmake . && make


	
Copyright and Licence
=====================

Degate is released under the GNU General Public License Version 3. See LICENSE.TXT for details.

Degate is developed by Martin Schobert <martin@mailbeschleuniger.de>.
