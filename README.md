[![DegateBanner](etc/degate_banner.png)](https://github.com/DegateCommunity)

<p align="center">
    <a href="https://gitter.im/DegateCommunity/Degate" alt="Gitter">
        <img src="https://badges.gitter.im/DegateCommunity/Degate.svg" /></a>
    <a href="https://github.com/DegateCommunity/Degate/blob/master/LICENSE.TXT" alt="License">
        <img src="https://img.shields.io/github/license/DegateCommunity/Degate" /></a>
    <a href="https://github.com/DegateCommunity/Degate/issues" alt="GitHub Issues">
        <img src="https://img.shields.io/github/issues/DegateCommunity/Degate" /></a>
    <a href="https://github.com/DegateCommunity/Degate/commits/develop" alt="Last Commit">
        <img src="https://img.shields.io/github/last-commit/DegateCommunity/Degate/develop" /></a>
    <a href="https://github.com/DegateCommunity/Degate/graphs/contributors" alt="Contributors">
        <img src="https://img.shields.io/github/contributors/DegateCommunity/Degate" /></a>
</p>

<p align="center">
        <img src="https://github.com/DegateCommunity/Degate/workflows/Windows%20build%20&%20tests/badge.svg?branch=develop" />
        <img src="https://github.com/DegateCommunity/Degate/workflows/Linux%20build%20&%20tests/badge.svg?branch=develop" />
        <img src="https://github.com/DegateCommunity/Degate/workflows/Mac%20build%20&%20tests/badge.svg?branch=develop" />
</p>

&nbsp;

Degate is a multi-platform software for semi-automatic VLSI reverse engineering of digital logic in chips. This repository is a continuation of the original Degate project, with major changes and a new maintainer. For more please visit our [wiki](https://github.com/DegateCommunity/Degate/wiki) page and, if you want to chat, visit our [Gitter](https://gitter.im/DegateCommunity/Degate). The current main maintainer of Degate is [Dorian Bachelot](https://github.com/DorianBDev).

&nbsp;

- [The project](#the-project)
  - [Little history](#little-history)
  - [Current status](#current-status)
  - [Future](#future)
  - [Documentation](#documentation)
  - [Screenshots](#screenshots)
- [Build](#build)
  - [Dependencies](#dependencies)
  - [Dependencies version](#dependencies-version)
  - [Quick start](#quick-start)
    - [For Linux (debian-like)](#for-linux-debian-like)
    - [For Windows](#for-windows)
    - [For MacOS](#for-macos)
  - [Troubleshooting](#troubleshooting)
- [Demo projects](#demo-projects)
- [Contributing](#contributing)
- [Localization](#localization)
  - [Help us](#help-us)
- [License](#license)

&nbsp;

# The project

## Little history

Degate was actively developed by Martin Schobert, during his thesis, from 2008 to 2011. From 2013 to 2018, the project was mainly inactive and only got some small fixes over the years. At this time, the project was composed of a library (libGate) and a GUI interface (Degate).

Degate has definitely a great potential, mainly because it is the only free and open-source software for hardware reverse engineering of chips, but never knew how to impose itself in the community. One possible reason for that is that Degate was known to be unstable and have impacting bugs in a first place. 

You can browse the old Degate code [here](https://github.com/DegateCommunity/Degate/tree/old-degate) and the old repository here:

https://github.com/nitram2342/degate

## Current status

This project replace the old Degate, with a focus on Windows, Linux and MacOS support, getting better stability and general modernization. To achieve this the first step was to drop GTK, move to Qt and merge the lib and the GUI part of the project. Another objective was to minimize the number of dependencies, for now it uses only Boost and Qt and the mid-term objective is to only keep Qt and remove Boost.

The stability was the main problem of the old Degate and the main reason why the community hadn't already united around it. Regarding that, we want to have fewer bugs as possible, even if some functionality needs to be dropped (for the moment).

For the moment we have (as functionalities):
- OpenGL 3 support (better performances and evolutive),
- Qt (bunch of tools, open-source and easy cross-platform),
- Multi-platform support (official support for Linux, Windows and MacOS),
- Multi-language support (for now only English and French are implemented),
- UTF-8 support for the workspace (the main area of the software, where you can work on high-definition chips images),
- Backward compatibility of old Degate project format,
- Project configuration (elements color, project name, size...),
- Layers configuration (type, position, background image...),
- Gate and gate configuration (gate template, gate instance, behavior with VHDL and Verilog support and debugging),
- Via and via configuration,
- EMarker (Electric Marker) and emarker configuration,
- Interconnection (electric connection between electric objects),
- Template matching (for gates),
- Grid,
- Via matching (can be tricky),
- Wire matching (need a rework),
- Sub-projects,
- Annotation and annotation configuration,
- Light and dark theme support,
- Rule checks,
- Modules,
- Connection inspector,
- And much more...

We dropped those functionalities from old Degate:
- Collaborative tracking of wires and vias.

A lot of bugs were fixed in this newer version, and it should be a better base to improve Degate even more in the future.

## Future

There is still a lot to do in this new version, but all core functionalities (and even more) are implemented.

Future functionalities remaining to implement (not limited):
- Explicit full netlist exporter (new feature),
- More languages (new feature),
- Integrated gate analyzer (new feature).

For a more precise roadmap see the [ROADMAP.md](https://github.com/DegateCommunity/Degate/blob/develop/ROADMAP.md) file.

## Documentation

The official Degate documentation is available [here](https://degate.readthedocs.io). It is still under construction.

Also, you can find [here](https://github.com/DegateCommunity/Degate/wiki) the official Degate wiki. For example, you can find on it a list of tutorials for [Degate](https://github.com/DegateCommunity/Degate/wiki/Tutorials-:-Degate) and for [IC Reverse-Engineering](https://github.com/DegateCommunity/Degate/wiki/Tutorials-:-IC-Reverse-Engineering).

## Screenshots

![](etc/screenshots/1.png) 

![](etc/screenshots/2.png) 

![](etc/screenshots/3.png) 

![](etc/screenshots/4.png) 

# Build

## Dependencies

Degate has only 2 dependencies: Boost and Qt5.

We use [vcpkg](https://vcpkg.io) to handle installation of those, please refer to the #Quick-start section below.

## Dependencies version

Described in the `vcpkg.json` file.

## Quick start

First, clone this repository (help [here](https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/cloning-a-repository)):
```console
> git clone https://github.com/DegateCommunity/Degate
> git submodule update --init --recursive
```

### For Linux (debian-like)

Prepare the install of dependencies:
```console
> ./vcpkg/bootstrap-vcpkg.sh -disableMetrics
```
If anything is missing, you should be prompted with help on how to install the needed tools.

Build (in the 'build' folder, for example):
```console
> cmake ..
> make
```
Binaries are in the 'build/out/bin' folder.

### For Windows

Install dependencies:
- CMake: https://cmake.org/download/
- A compiler (for example MSVC): https://visualstudio.microsoft.com/
```console
> .\vcpkg\bootstrap-vcpkg.bat -disableMetrics
```

Build (in the 'build' folder, for example):
```console
> cmake ..
> cmake --build .
```

Binaries are in the 'build/out/bin' folder.

### For MacOS

Prepare the install of dependencies:
```console
> ./vcpkg/bootstrap-vcpkg.sh -disableMetrics
```

Build (in the 'build' folder, for example):
```console
> cmake ..
> cmake --build .
```

Binaries are in the 'build/out/bin' folder in the bundle ".app" format.

## Troubleshooting

- `Could NOT find Boost (missing: filesystem system thread) (found version "1.71.0")`

  This message may be caused by default options which force to use static and multithreaded version of the Boost library.
  For example, this is a problem when installing Boost with [MacPorts](https://gitter.im/DegateCommunity/Degate?at=6085523d06e2e024e8752da5) on MacOS because the installed version [does not provide a static version by default](https://gitter.im/DegateCommunity/Degate?at=6085183db6a4714a29e65fbb).
  To change these default options, you can use these two commands when calling cmake (ON/OFF):
  ```console
  > cmake .. -DBoost_USE_STATIC_LIBS=OFF -DBoost_USE_MULTITHREADED=OFF
  ```
  
  On Windows this can also be caused by an installed version of Boost that does not match your installed compiler.

- `/etc/cmake/ConfigureFiles.cmake:25 (file): file failed to open for reading (No such file or directory): .../VERSION`

  This message can appear for MacOS users. The problem comes from spaces in directory names, this is a bug from CMake.
  Simply move your Degate folder to a file tree without spaces in directory names.

- `CMake Error at vcpkg_execute_required_process.cmake:127 (message): Command failed: vcpkg/downloads/tools/ninja/1.10.2-linux/ninja -v`

  Please refer to vcpkg output, this is linked to Qt installation that need pre-installed dependencies on some platforms. For example on linux you need to execute:
  `sudo apt-get install '^libxcb.*-dev'
  libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev
  libxkbcommon-x11-dev libegl1-mesa-dev` and `sudo apt-get install libxi-dev libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev libxrandr-dev libxxf86vm-dev
`.

# Demo projects

You can find demo projects [here](https://github.com/DegateCommunity/DegateDemoProjects).

# Contributing

Read the "CONTRIBUTING.md" file.

# Localization

## Help us

For now only English and French are supported, but if you wish you can help easily. Degate use Qt Linguist for translation, you can learn more here: https://doc.qt.io/qt-5/linguist-translators.html.
You can find .ts files (to use with Qt Linguist) in 'res/languages'.

Languages:
- English (100%),
- French (100%).
- German (80%),
- Russian (0%),
- Spanish (0%),
- Italian (0%),
- Korean (0%).

To add a new language opens a new issue, or use our Crowdin project: https://crowdin.com/project/degate.

# License

Degate is released under the GNU General Public License Version 3. See LICENSE.TXT for details.

The current main maintainer of Degate is **Dorian Bachelot** <dev@dorianb.net> and the original Degate maintainer is **Martin Schobert** <martin@mailbeschleuniger.de>.
