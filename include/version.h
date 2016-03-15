/*
    Copyright 2016 Amebis

    This file is part of Updater.

    Updater is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Updater is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Updater. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

//
// Product version as a single DWORD
// Note: Used for version comparison within C/C++ code.
//
#define UPDATER_VERSION         0x01000000

//
// Product version by components
// Note: Resource Compiler has limited preprocessing capability,
// thus we need to specify major, minor and other version components
// separately.
//
#define UPDATER_VERSION_MAJ     1
#define UPDATER_VERSION_MIN     0
#define UPDATER_VERSION_REV     0
#define UPDATER_VERSION_BUILD   0

//
// Human readable product version and build year for UI
//
#define UPDATER_VERSION_STR     "2.0-alpha"
#define UPDATER_BUILD_YEAR_STR   "2016"
