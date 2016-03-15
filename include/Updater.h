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
#define UPDATER_BUILD_YEAR_STR  "2016"


#if !defined(RC_INVOKED) && !defined(MIDL_PASS)

#include <stdex/idrec.h>

#include <string>
#include <vector>


///
/// Data records alignment
///
#define UPDATER_RECORD_ALIGN    1


///
/// Database IDs
///
#define UPDATER_DB_ID           (*(Updater::recordid_t*)"UPD")


namespace Updater {
    typedef unsigned __int32 recordid_t;
    typedef unsigned __int32 recordsize_t;


    ///
    /// Package information
    ///
    struct package_info {
        unsigned __int32 ver;       ///< Binary Version
        std::wstring desc;          ///< Human Readable Description (i. e. "1.2")
        std::string lang;           ///< Package Language (ISO 639-1 language code followed by underscore and ISO 3166 country code)
        std::string url;            ///< Package download URL (UTF-8 encoded)
    };

    typedef stdex::idrec::record<package_info, recordid_t, recordsize_t, UPDATER_RECORD_ALIGN> package_info_rec;


    ///
    /// Signature
    ///
    typedef std::vector<unsigned char> signature;

    typedef stdex::idrec::record<signature, recordid_t, recordsize_t, UPDATER_RECORD_ALIGN> signature_rec;
};


const Updater::recordid_t stdex::idrec::record<Updater::package_info, Updater::recordid_t, Updater::recordsize_t, UPDATER_RECORD_ALIGN>::id = *(Updater::recordid_t*)"PKG";
const Updater::recordid_t stdex::idrec::record<Updater::signature   , Updater::recordid_t, Updater::recordsize_t, UPDATER_RECORD_ALIGN>::id = *(Updater::recordid_t*)"SGN";

#endif // !defined(RC_INVOKED) && !defined(MIDL_PASS)
