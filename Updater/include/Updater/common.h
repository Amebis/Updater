/*
    Copyright 2016-2018 Amebis

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

#if !defined(__UPDATER_common_h__)
#define __UPDATER_common_h__

#include "../../../../include/UpdaterCfg.h"


//
// Resource IDs
//
#define UPDATER_IDR_KEY_PUBLIC          1

#if !defined(RC_INVOKED) && !defined(MIDL_PASS)

///
/// Public function calling convention
///
//#ifdef UPDATER
//#define UPDATER_API    __declspec(dllexport)
//#else
//#define UPDATER_API    __declspec(dllimport)
//#endif
#define UPDATER_API

#define UPDATER_SIGNATURE_MARK  "SHA1SIGN:"

#endif // !defined(RC_INVOKED) && !defined(MIDL_PASS)
#endif // !defined(__UPDATER_common_h__)
