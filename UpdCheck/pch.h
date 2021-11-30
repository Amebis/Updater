/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2021 Amebis
*/

#pragma once

#include <Updater/chkthread.h>

#pragma warning(push)
#pragma warning(disable: WXWIDGETS_CODE_ANALYSIS_WARNINGS)
#include <wx/app.h>
#include <wx/config.h>
#include <wx/dir.h>
#include <wx/ffile.h>
#include <wx/filename.h>
#include <wx/init.h>
#include <wx/scopedptr.h>
#pragma warning(pop)

#include <wxex/common.h>
