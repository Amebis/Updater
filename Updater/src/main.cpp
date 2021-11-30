/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2021 Amebis
*/

#include "pch.h"


///
/// Module instance
///
HINSTANCE g_hInstance = NULL;


///
/// Main function
///
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) noexcept
{
    UNREFERENCED_PARAMETER(lpvReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        g_hInstance = hinstDLL;

    return TRUE;
}
