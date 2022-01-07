/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2022 Amebis
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
