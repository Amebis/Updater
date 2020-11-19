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

#include "../Updater/include/Updater/common.h"

#include "UpdPublish.h"

#pragma warning(push)
#pragma warning(disable: WXWIDGETS_CODE_ANALYSIS_WARNINGS)
#include <wx/app.h>
#include <wx/base64.h>
#include <wx/cmdline.h>
#include <wx/xml/xml.h>
#pragma warning(pop)

#include <wxex/crypto.h>
#include <wxex/hex.h>
#include <wxex/xml.h>
