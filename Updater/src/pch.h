/*
    Copyright 2016-2020 Amebis

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

#include "../include/Updater/chkthread.h"

#include <wx/base64.h>
#include <wx/buffer.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/protocol/http.h>
#include <wx/url.h>

#include <wxex/hex.h>
#include <wxex/xml.h>

//
// Private data declaration
//
extern HINSTANCE g_hInstance;
