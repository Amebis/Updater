/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2021 Amebis
*/

#if !defined(__UPDATER_common_h__)
#define __UPDATER_common_h__

#include "../../../../include/UpdaterCfg.h"

//
// Resource IDs
//
#define UPDATER_IDR_KEY_PUBLIC          1

#if !defined(RC_INVOKED) && !defined(MIDL_PASS)

#include <codeanalysis\warnings.h>
#ifndef WXWIDGETS_CODE_ANALYSIS_WARNINGS
#define WXWIDGETS_CODE_ANALYSIS_WARNINGS ALL_CODE_ANALYSIS_WARNINGS 26812 26814
#endif

#define UPDATER_SIGNATURE_MARK "SIGNATURE:"

#endif // !defined(RC_INVOKED) && !defined(MIDL_PASS)
#endif // !defined(__UPDATER_common_h__)
