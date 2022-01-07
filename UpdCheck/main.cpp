/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2022 Amebis
*/

#include "pch.h"


///
/// Main function
///
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    // Initialize wxWidgets.
    wxInitializer initializer;
    if (!initializer.IsOk())
        return -1;

    // Initialize configuration.
    wxConfigBase *cfgPrev = wxConfigBase::Set(new wxConfig(wxT(PRODUCT_CFG_APPLICATION), wxT(PRODUCT_CFG_VENDOR)));
    if (cfgPrev) wxDELETE(cfgPrev);

    // Initialize locale.
    wxLocale locale;
#pragma warning(suppress: 26812) // wxLanguage is unscoped
    wxLanguage lang_ui;
    if (wxInitializeLocale(locale, &lang_ui)) {
        // Do not add translation catalog, to keep log messages in English.
        // Log files are for help-desk and should remain globally intelligible.
        //wxVERIFY(locale.AddCatalog(wxT("Updater") wxT(wxExtendVersion)));
    }

    // Create output folder.
    wxString path(wxFileName::GetTempDir());
    if (!wxEndsWithPathSeparator(path))
        path += wxFILE_SEP_PATH;
    if (!wxDirExists(path))
        wxMkdir(path);

    // Prepare log file.
    wxFFile log_file(path + wxT("Updater-") wxT(PRODUCT_CFG_APPLICATION) wxT(".log"), wxT("wt"));
    if (log_file.IsOpened())
        delete wxLog::SetActiveTarget(new wxLogStderr(log_file.fp()));

    wxUpdCheckThread worker(lang_ui == wxLANGUAGE_DEFAULT ? wxT("en_US") : wxLocale::GetLanguageCanonicalName(lang_ui));
    const wxUpdCheckThread::wxResult res = worker.CheckForUpdate();
    switch (res) {
    case wxUpdCheckThread::wxResult::UpdateAvailable: return worker.LaunchUpdate(NULL, true) ? 0 : 1;
    case wxUpdCheckThread::wxResult::UpToDate       : return 0;
    default                                         : return static_cast<int>(res);
    }
}
