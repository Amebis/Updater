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

#include "stdafx.h"


///
/// Main function
///
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
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
    wxUpdCheckThread::wxResult res = worker.CheckForUpdate();
    switch (res) {
    case wxUpdCheckThread::wxUpdUpdateAvailable: return worker.LaunchUpdate(NULL, true) ? 0 : 1;
    case wxUpdCheckThread::wxUpdUpToDate       : return 0;
    default                                    : return res;
    }
}
