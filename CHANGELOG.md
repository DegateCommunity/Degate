# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- New performance options.

### Changed
- Huge overall optimization.
- Improved new background image import.
- Improved the about dialog.

### Fixed
- Fixed layer selection after project creation.

## [2.0.0-alpha.4] - 2020-11-08
### Added
- Added a link to the Degate documentation.
- German translation from [Martin Schobert](https://github.com/nitram2342).
- Added a connection inspector dialog.
- Added automatic gate naming option.
- Added an annotation list dialog.
- Added an update checker.
- Added an auto updater.
- Added official MacOS support.

### Changed
- Reworked rule violations dialog to allow translation of violations.
- Reworked the project creation dialog.
- Improved dark theme.

### Fixed
- Fixed subwindows text reload when language change.

## [2.0.0-alpha.3] - 2020-09-14
### Added
- Added proper top menu options for many dialogs.
- Added modules support.
- Added the possibility to isolate/break connections of any connectable object.
- Added a full-screen mode.

### Changed
- Updated the roadmap.

### Fixed
- Fixed a translation problem with default Qt buttons.
- Fixed a bug with color buttons of the project settings dialog (Linux).
- Fixed a menu error in the main window (for the layer edit menu).
- Fixed a translation problem with default Qt buttons.
- Fixed project switching transition, it will now ask to save current project before all.
- Fixed default grid color.

## [2.0.0-alpha.2] - 2020-08-22
### Added
- Added rule violations dialog.
- Added "go to" function (useful for some dialogs).
- Added snap to grid.
- Added a roadmap.
- Added a contributing file.
- Added a code of conduct.
- Added a pull request template for github.
- Added automatic copy of tests files.
- Added logging for uncaught errors.

### Changed
- Better theme support.
- First half of code homogenization work.
- Moved some old files to the 'etc' folder.
- Updated "About" dialog.

### Fixed
- Fixed a bug in the theme manager that made a buggy transition between dark and native theme.
- Fixed a possible crash when trying to select multiple objects in an area.

## [2.0.0-alpha.1] - 2020-07-21
### Added
- Added multi-platform support (tested on Linux and Windows).
- Added multi-language support (for now only English and French are implemented).
- Added UTF-8 support for the workspace.
- Added light and dark theme support.
- And much more...

### Changed
- Moved to OpenGL 3.
- Moved to Qt5.
- Reworked project configuration.
- Reworked layers configuration.
- Reworked the whole workspace area.
- Reworked the text system.
- Reworked all primitive elements (gate, annotation...).
- Moved unit tests to Catch2.

### Removed
- Removed all dependencies except Boost.

### Fixed
- Many improvements and bugfixes.

## [0.1.3] - 2018-09-23
### Changed
- Test projects moved into Git repository.

### Fixed
- Several improvements and bugfixes.

## [0.1.2] - 2012-09-29
### Added
- Integrated the undo feature from Robert Nitsch into Degate.

## [0.1.1] - 2012-04-23
### Fixed
- Bugfix release.

## [0.1.0] - 2011-01-19
### Added
- Toggle fullscreen mode with F11.
- Generate verilog code stubs and testbench stubs for standard cells.
- Run a compile test for verilog code stubs directly from the Gate Edit Dialog (requires Icarus Verilog).
- Run gtkwave directly from the Gate Edit Dialog.
- Integrated via matching (cross-correlation).

### Changed
- Migrated code-base in order to support the new boost::filesystem version 3. Version 2 is still supported, too.

## [0.0.9] - 2011-01-19
### Added
- Scan for vias on adjacent layers and connect them.
- Integrated color settings for wires, vias, ... into project settings.
- Added font size as project setting.
- Added a new logic model type: EMarker, which are electrically connectable marker objects.
- Snap gates to a corridor of standard cell placements.
- Implemented highlighting of adjacent objects.
- Toggle info rendering with Ctrl-3.
- Implemented autonaming of standard cells according to their row and column position.
- Allow a persistent blacklisting of DRC violations.

### Changed
- Improved Design Rule Checks and the DRC violation dialog.

### Fixed
- Software crashed on opening projects (OS X).
- Found workaround for texture memory leak on OSX.
- Fixed several rendering problems.

## [0.0.8] - 2010-04-15
### Added
- Added a jpeg image importer.
- Added global tile cache manager.
- Implemented interface to use external tools for the wire and via matching.
- Added library and GUI code to perform Design Rule Checks.

### Changed
- Rewritten rendering engine: it uses OpenGL.
- Merging of standard cell template images. That will increase the recognition rate and reduces false-positive matches.
- Template matching improved.

### Fixed
- Several bugfixes.

## [0.0.7] - 2009-11-15
### Added
- Added basic support for distributed wire tracing.
- Implemented autosave and restore.
- Added support for behavioural description of standard cells in VHDL, Verilog and free text.
- Added support for the autogeneration of VHDL and VHDL testbench code.
- Support for enabling/disabling and re-ordering of layers added.
- Annotations (a kind of 2D-boomarks) added.
- Basic support for module hierarchy added.
- Support for subprojects added.
- Basic support for wire matching added.

### Changed
- Core completely rewritten in C++. The core is now called libdegate.
- Degate now uses cmake to build the code.
- Degate now uses a tile based approach to handle large images.

## [0.0.6] - 2009-03-30
### Added
- Dedicated toolbar icons added.
- Open projects via command line.
- Added scaling manager: handle prescaled images (bicubical scaling).
- Template matching: select multiple gates for matching, work on scaled images.
- More API documentation.
- Project attributes added: project name and project descr.
- Gates and gate ports can have colors - wires and vias too, but there is no UI.
- Handling unregular grids.
- Naming gate objects by position.
- Precompiler macro fix: improved architecture detection (thanks to mazzoo).

### Changed
- Focus removed from quit button and set to image area.
- Gate list / gate selection: sortable rows, more attributes displayed.
- Improved grid configuration.
- Improved connection inspector.

## [0.0.5] - 2009-03-25
### Added
- Select gate ports.
- Inspect connections.
- Join/ unjoin objects.
- Write logic model to temp file first and rename it afterwars. This prevents data loss, if writing fails.

### Fixed
- Several bugfixes.

## [0.0.4] - 2009-03-13
### Changed
- Template matching improved.

### Fixed
- Several bugfixes.

## [0.0.3] - 2009-01-31
### Fixed
- Problem with shift-release event fixed.

## [0.0.2] - 2008-12-19
### Fixed
- Applied patch from Eric Preston. Code now complies on Feodora10 with newer sigc++22-devel headers.

## [0.0.1] - 2008-12-19
### Fixed
- Initial package.