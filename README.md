# About

This project is a fork of Degate.

Degate is a software for semi-automatic VLSI reverse engineering of digital logic in chips. Please visit the project website to learn more about Degate.

  http://degate.org (original Degate project website)

# Dependencies

Only 2 dependencies : Boost and Qt5.

For Boost, you can specify to CMake a custom path with : -DBOOST_ROOT="custom_path_to_boost". Prebuilt versions for windows are available here : https://sourceforge.net/projects/boost/files/boost-binaries/.

For Qt5, you can specify to CMake a custom path with : -DCMAKE_PREFIX_PATH="custom_path_to_qt". For example on windows : Qt/VERSION/COMPILER/lib/cmake/Qt5. You can download Qt5 here : https://www.qt.io/download.

Keep your CMake installation up to date.

# Build

After installing Boost and Qt5, build Degate with cmake : cmake [path_to_source] [optional:] -DBOOST_ROOT="custom_path_to_boost" [optional:] -DCMAKE_PREFIX_PATH="custom_path_to_qt"

# License

Degate is released under the GNU General Public License Version 3. See LICENSE.TXT for details.

Degate is originaly developed by Martin Schobert <martin@mailbeschleuniger.de>.
