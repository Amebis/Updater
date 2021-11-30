/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2021 Amebis
*/

#pragma once

#include "../include/Updater/chkthread.h"

#pragma warning(push)
#pragma warning(disable: WXWIDGETS_CODE_ANALYSIS_WARNINGS)
#include <wx/base64.h>
#include <wx/buffer.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/protocol/http.h>
#include <wx/url.h>
#pragma warning(pop)

#include <wxex/hex.h>
#include <wxex/xml.h>

//
// Private data declaration
//
extern HINSTANCE g_hInstance;
