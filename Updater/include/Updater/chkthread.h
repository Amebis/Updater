/*
    Copyright 2016 Amebis

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

#include "common.h"

#include <wx/arrstr.h>
#include <wx/config.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/xml/xml.h>

#include <wxex/crypto.h>


///
/// wxEVT_UPDATER_CHECK_COMPLETE event
///
wxDECLARE_EXPORTED_EVENT(UPDATER_API, wxEVT_UPDATER_CHECK_COMPLETE, wxThreadEvent);


///
/// Update check thread
///
class UPDATER_API wxUpdCheckThread : public wxThread
{
public:
    ///
    /// Thread exit codes
    ///
    enum wxResult {
        wxUpdUninitialized   = -1,  ///< Initialization failed
        wxUpdAborted         = -2,  ///< Thread aborted
        wxUpdRepoUnavailable = -3,  ///< No update package information available
        wxUpdPkgUnavailable  = -4,  ///< Update package not available

        wxUpdUpdateAvailable =  0,  ///< Update downloaded, verified and prepared to install
        wxUpdUpToDate,              ///< Product is up-to-date
    };

    wxUpdCheckThread(const wxString &langId, wxEvtHandler *parent = NULL);
    virtual ~wxUpdCheckThread();

    ///
    /// Checks for updates and safely downloads update package when available.
    ///
    inline wxResult CheckForUpdate()
    {
        return DoCheckForUpdate();
    }

protected:
    ///
    /// Thread's body
    ///
    /// \returns Exit code
    ///
    virtual ExitCode Entry();

    ///
    /// Checks for updates and safely downloads update package when available.
    ///
    wxResult DoCheckForUpdate();

    ///
    /// Downloads repository database (with integrity check).
    ///
    /// \returns Repository database. Use wxDELETE after use.
    ///
    wxXmlDocument* GetCatalogue();

    ///
    /// Parses repository database.
    ///
    /// \returns
    /// - true if parsing succeeded
    /// - false otherwise
    ///
    bool ParseCatalogue(const wxXmlDocument &doc);

    ///
    /// Stores update package meta-information to cache.
    ///
    /// \returns
    /// - true if writing succeeded
    /// - false otherwise
    ///
    bool WriteUpdatePackageMeta();

    ///
    /// Retrieves update package meta-information from cache.
    ///
    /// \returns
    /// - true if reading succeeded
    /// - false otherwise
    ///
    bool ReadUpdatePackageMeta();

    ///
    /// Downloads update package.
    ///
    /// \returns
    /// - true if downloading succeeded
    /// - false otherwise
    ///
    bool DownloadUpdatePackage();

public:
    ///
    /// Launches update.
    ///
    /// \param[in] hParent   Handle of parent window
    /// \param[in] headless  Launch silent install
    ///
    /// \returns
    /// - true if launch succeeded
    /// - false otherwise
    ///
    bool LaunchUpdate(WXHWND hParent = NULL, bool headless = false);


protected:
    wxEvtHandler *m_parent; ///< Parent to notify
    bool m_ok;              ///< Is class initialized correctly?

    wxString m_langId;      ///< Language identifier
    wxConfig m_config;      ///< Application configuration
    wxString m_path;        ///< Working directory (downloaded files, etc.)
    wxCryptoSession *m_cs;  ///< Cryptographics session
    wxCryptoKey *m_ck;      ///< Public key for update database integrity validation

    wxUint32 m_version;     ///< Latest product version available (numerical)
    wxString m_versionStr;  ///< Latest product version available (string)
    wxArrayString m_urls;   ///< List of update package file downloads
    wxMemoryBuffer m_hash;  ///< Update package SHA-1 hash

    wxString m_fileName;    ///< Downloaded package file name
};