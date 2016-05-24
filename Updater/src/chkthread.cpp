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

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
// wxEVT_UPDATER_CHECK_COMPLETE
//////////////////////////////////////////////////////////////////////////

wxDEFINE_EVENT(wxEVT_UPDATER_CHECK_COMPLETE, wxThreadEvent);


//////////////////////////////////////////////////////////////////////////
// wxUpdCheckThread
//////////////////////////////////////////////////////////////////////////

wxUpdCheckThread::wxUpdCheckThread(const wxString &langId, wxEvtHandler *parent) :
    m_parent(parent),
    m_langId(langId),
    m_config(wxT(UPDATER_CFG_APPLICATION) wxT("\\Updater"), wxT(UPDATER_CFG_VENDOR)),
    m_cs(NULL),
    m_ck(NULL),
    m_version(0),
    wxThread(wxTHREAD_JOINABLE)
{
    m_path = wxFileName::GetTempDir();
    if (!wxEndsWithPathSeparator(m_path))
        m_path += wxFILE_SEP_PATH;

    if (!wxDirExists(m_path))
        wxMkdir(m_path);

    m_cs = new wxCryptoSessionRSAAES();
    wxCHECK2_MSG(m_cs, { m_ok = false; return; }, wxT("wxCryptoSessionRSAAES creation failed"));

    // Import public key.
    HRSRC res = ::FindResource(g_hInstance, MAKEINTRESOURCE(UPDATER_IDR_KEY_PUBLIC), RT_RCDATA);
    wxCHECK2_MSG(res, { m_ok = false; return; }, wxT("public key not found"));
    HGLOBAL res_handle = ::LoadResource(g_hInstance, res);
    wxCHECK2_MSG(res_handle, { m_ok = false; return; }, wxT("loading resource failed"));

    m_ck = new wxCryptoKey();
    wxCHECK2_MSG(m_ck, { m_ok = false; return; }, wxT("wxCryptoKey creation failed"));
    wxCHECK2(m_ck->ImportPublic(*m_cs, ::LockResource(res_handle), ::SizeofResource(g_hInstance, res)), { m_ok = false; return; });
}


wxUpdCheckThread::~wxUpdCheckThread()
{
    if (m_ck) delete m_ck;
    if (m_cs) delete m_cs;
}


wxThread::ExitCode wxUpdCheckThread::Entry()
{
    wxResult result = DoCheckForUpdate();

    if (m_parent) {
        // Signal the event handler that this thread is going to be destroyed.
        // NOTE: here we assume that using the m_parent pointer is safe,
        //       (in this case this is assured by the wxZRColaCharSelect destructor)
        wxThreadEvent *e = new wxThreadEvent(wxEVT_UPDATER_CHECK_COMPLETE);
        e->SetInt(result);
        wxQueueEvent(m_parent, e);
    }

    return (wxThread::ExitCode)(int)result;
}


wxUpdCheckThread::wxResult wxUpdCheckThread::DoCheckForUpdate()
{
    if (!m_ok)
        return wxUpdUninitialized;

    // Download update catalogue.
    if (TestDestroy()) return wxUpdAborted;
    wxScopedPtr<wxXmlDocument> doc(GetCatalogue());
    if (doc) {
        // Parse the update catalogue to check for an update package availability.
        if (TestDestroy()) return wxUpdAborted;
        if (ParseCatalogue(*doc)) {
            // Save update package meta-information for the next time.
            if (TestDestroy()) return wxUpdAborted;
            WriteUpdatePackageMeta();
        } else {
            wxLogStatus(_("Update check complete. Your product is up to date."));
            return wxUpdUpToDate;
        }
    } else {
        // Update download catalogue failed, or repository database didn't change. Read a cached version of update package meta-information?
        if (TestDestroy()) return wxUpdAborted;
        if (!ReadUpdatePackageMeta()) {
            // Reset CatalogueLastModified to force update catalogue download next time.
            m_config.DeleteEntry(wxT("CatalogueLastModified"));
            return wxUpdRepoUnavailable;
        }

        if (m_version <= UPDATER_PRODUCT_VERSION) {
            wxLogStatus(_("Update check complete. Your product is up to date."));
            return wxUpdUpToDate;
        }
    }

    m_fileName  = m_path;
    m_fileName += wxT("Updater-") wxT(UPDATER_CFG_APPLICATION) wxT("-");
    m_fileName += m_versionStr;
    m_fileName += wxT(".msi");

    if (TestDestroy()) return wxUpdAborted;
    if (!DownloadUpdatePackage())
        return wxUpdPkgUnavailable;

    return wxUpdUpdateAvailable;
}


wxXmlDocument* wxUpdCheckThread::GetCatalogue()
{
    wxLogStatus(_("Downloading repository catalogue..."));

    wxString lastModified;
    m_config.Read(wxT("CatalogueLastModified"), &lastModified, wxEmptyString);

    for (const wxChar *server = wxT(UPDATER_HTTP_SERVER); server[0]; server += wcslen(server) + 1) {
        if (TestDestroy()) return NULL;

        wxLogStatus(_("Contacting repository at %s..."), server);

        // Load repository database.
        wxHTTP http;
        http.SetHeader(wxS("User-Agent"), wxS("Updater/") wxS(UPDATER_VERSION_STR));
        if (!lastModified.IsEmpty())
            http.SetHeader(wxS("If-Modified-Since"), lastModified);
        if (!http.Connect(server, UPDATER_HTTP_PORT)) {
            wxLogWarning(_("Error resolving %s server name."), server);
            continue;
        }
        wxScopedPtr<wxInputStream> httpStream(http.GetInputStream(wxS(UPDATER_HTTP_PATH)));
        if (http.GetResponse() == 304) {
            wxLogStatus(_("Repository did not change since the last time."));
            return NULL;
        } else if (!httpStream) {
            wxLogWarning(_("Error response received from server %s (port %u) requesting %s."), server, UPDATER_HTTP_PORT, UPDATER_HTTP_PATH);
            continue;
        } else if (TestDestroy())
            return NULL;
        wxScopedPtr<wxXmlDocument> doc(new wxXmlDocument());
        if (!doc->Load(*httpStream, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES)) {
            wxLogWarning(_("Error loading repository catalogue."));
            http.Close();
            continue;
        }
        m_config.Write(wxT("CatalogueLastModified"), http.GetHeader(wxT("Last-Modified")));
        http.Close();

        wxLogStatus(_("Verifying repository catalogue signature..."));

        // Find the (first) signature.
        if (TestDestroy()) return NULL;
        wxXmlNode *document = doc->GetDocumentNode();
        wxMemoryBuffer sig;
        for (wxXmlNode *prolog = document->GetChildren(); prolog; prolog = prolog->GetNext()) {
            if (prolog->GetType() == wxXML_COMMENT_NODE) {
                wxString content = prolog->GetContent();
                size_t content_len = content.length();
                if (content_len >= _countof(wxS(UPDATER_SIGNATURE_MARK)) - 1 &&
                    memcmp((const wxStringCharType*)content, wxS(UPDATER_SIGNATURE_MARK), sizeof(wxStringCharType)*(_countof(wxS(UPDATER_SIGNATURE_MARK)) - 1)) == 0)
                {
                    // Read the signature.
                    size_t signature_len = content_len - (_countof(wxS(UPDATER_SIGNATURE_MARK)) - 1);
                    size_t len = wxBase64DecodedSize(signature_len);
                    size_t res = wxBase64Decode(sig.GetWriteBuf(len), len, content.Right(signature_len), wxBase64DecodeMode_SkipWS);
                    if (res != wxCONV_FAILED) {
                        sig.SetDataLen(res);

                        // Remove the signature as it is not a part of the validation check.
                        document->RemoveChild(prolog);
                        wxDELETE(prolog);
                        break;
                    } else
                        sig.Clear();
                }
            }
        }

        if (sig.IsEmpty()) {
            wxLogWarning(_("Signature not found in the repository catalogue."));
            continue;
        }

        // Hash the content.
        if (TestDestroy()) return NULL;
        wxCryptoHashSHA1 ch(*m_cs);
        if (!wxXmlHashNode(ch, document))
            continue;

        // We have the hash, we have the signature, we have the public key. Now verify.
        if (TestDestroy()) return NULL;
        if (!wxCryptoVerifySignature(ch, sig, *m_ck)) {
            wxLogWarning(_("Repository catalogue signature does not match its content, or signature verification failed."));
            continue;
        }

        // Verify document type (root element).
        const wxString &nameRoot = doc->GetRoot()->GetName();
        if (nameRoot != wxT("Packages")) {
            wxLogWarning(_("Invalid root element in repository catalogue (actual: %s, expected: %s)."), nameRoot.c_str(), wxT("Packages"));
            continue;
        }

        // The downloaded repository database passed all checks.
        return doc.release();
    }

    // No repository database downloaded successfully.
    return NULL;
}


bool wxUpdCheckThread::ParseCatalogue(const wxXmlDocument &doc)
{
    bool found = false;

    wxLogStatus(_("Parsing repository catalogue..."));

    // Iterate over packages.
    for (wxXmlNode *elPackage = doc.GetRoot()->GetChildren(); elPackage; elPackage = elPackage->GetNext()) {
        if (TestDestroy()) return false;

        if (elPackage->GetType() == wxXML_ELEMENT_NODE && elPackage->GetName() == wxT("Package")) {
            // Get package version.
            wxUint32 version = 0;
            wxString versionStr;
            for (wxXmlNode *elVersion = elPackage->GetChildren(); elVersion; elVersion = elVersion->GetNext()) {
                if (elVersion->GetType() == wxXML_ELEMENT_NODE && elVersion->GetName() == wxT("Version")) {
                    for (wxXmlNode *elVersionNote = elVersion->GetChildren(); elVersionNote; elVersionNote = elVersionNote->GetNext()) {
                        if (elVersionNote->GetType() == wxXML_ELEMENT_NODE) {
                            const wxString &name = elVersionNote->GetName();
                            if (name == wxT("hex"))
                                version = _tcstoul(elVersionNote->GetNodeContent(), NULL, 16);
                            else if (name == wxT("desc"))
                                versionStr = elVersionNote->GetNodeContent();
                        }
                    }
                }
            }
            if (version <= UPDATER_PRODUCT_VERSION || version < m_version) {
                // This package is older than currently installed product or the superseeded package found already.
                continue;
            }

            // Get package download URL for our platform and language.
            wxString platformId;
#if defined(__WINDOWS__)
            platformId += wxT("win");
#endif
#if defined(_WIN64)
            platformId += wxT("-amd64");
#else
            platformId += wxT("-x86");
#endif
            wxArrayString urls;
            wxMemoryBuffer hash;
            for (wxXmlNode *elDownloads = elPackage->GetChildren(); elDownloads; elDownloads = elDownloads->GetNext()) {
                if (elDownloads->GetType() == wxXML_ELEMENT_NODE && elDownloads->GetName() == wxT("Downloads")) {
                    for (wxXmlNode *elPlatform = elDownloads->GetChildren(); elPlatform; elPlatform = elPlatform->GetNext()) {
                        if (elPlatform->GetType() == wxXML_ELEMENT_NODE) {
                            if (elPlatform->GetName() == wxT("Platform") && elPlatform->GetAttribute(wxT("id")) == platformId) {
                                // Get language.
                                for (wxXmlNode *elLocale = elPlatform->GetChildren(); elLocale; elLocale = elLocale->GetNext()) {
                                    if (elLocale->GetType() == wxXML_ELEMENT_NODE && elLocale->GetName() == wxT("Localization") && elLocale->GetAttribute(wxT("lang")) == m_langId) {
                                        for (wxXmlNode *elLocaleNote = elLocale->GetChildren(); elLocaleNote; elLocaleNote = elLocaleNote->GetNext()) {
                                            if (elLocaleNote->GetType() == wxXML_ELEMENT_NODE) {
                                                const wxString &name = elLocaleNote->GetName();
                                                if (name == wxT("url"))
                                                    urls.Add(elLocaleNote->GetNodeContent());
                                                else if (name == wxT("hash")) {
                                                    // Read the hash.
                                                    wxString content(elLocaleNote->GetNodeContent());
                                                    size_t len = wxHexDecodedSize(content.length());
                                                    size_t res = wxHexDecode(hash.GetWriteBuf(len), len, content, wxHexDecodeMode_SkipWS);
                                                    if (res != wxCONV_FAILED)
                                                        hash.SetDataLen(res);
                                                    else
                                                        hash.Clear();
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (urls.empty() || hash.IsEmpty()) {
                // This package is for different architecture and/or language.
                // ... or missing URL and/or hash.
                continue;
            }

            // Save found package info.
            m_version    = version;
            m_versionStr = versionStr;
            m_urls       = urls;
            m_hash       = hash;
            found        = true;

            wxLogMessage(_("Update package found (version: %s)."), m_versionStr.c_str());
        }
    }

    return found;
}


bool wxUpdCheckThread::WriteUpdatePackageMeta()
{
    // Concatenate URLs.
    wxString urls;
    for (size_t i = 0, n = m_urls.GetCount(); i < n; i++) {
        if (i) urls += wxT('\n');
        urls += m_urls[i];
    }

    bool result = true;
    if (!m_config.Write(wxT("PackageVersion"    ), m_version          )) result = false;
    if (!m_config.Write(wxT("PackageVersionDesc"), m_versionStr       )) result = false;
    if (!m_config.Write(wxT("PackageURLs"       ), urls               )) result = false;
    if (!m_config.Write(wxT("PackageHash"       ), wxHexEncode(m_hash))) result = false;

    return result;
}


bool wxUpdCheckThread::ReadUpdatePackageMeta()
{
    long lng;
    if (m_config.Read(wxT("PackageVersion"), &lng)) {
        m_version = lng;
        if (m_config.Read(wxT("PackageVersionDesc"), &m_versionStr)) {
            wxString str;
            if (m_config.Read(wxT("PackageURLs"), &str)) {
                // Split URLs
                m_urls.Clear();
                const wxStringCharType *buf = str;
                for (size_t i = 0, j = 0, n = str.Length(); i < n;) {
                    for (; j < n && buf[j] != wxT('\n'); j++);
                    m_urls.Add(str.SubString(i, j - 1));
                    i = ++j;
                }

                if (m_config.Read(wxT("PackageHash"), &str)) {
                    // Convert to binary.
                    size_t len = wxHexDecodedSize(str.length());
                    size_t res = wxHexDecode(m_hash.GetWriteBuf(len), len, str, wxHexDecodeMode_SkipWS);
                    if (res != wxCONV_FAILED) {
                        m_hash.SetDataLen(res);
                        return true;
                    } else
                        m_hash.Clear();
                }
            }
        }
    }

    wxLogWarning(_("Reading update package meta-information from cache failed or incomplete."));
    m_version = 0;
    m_versionStr.Clear();
    m_urls.Clear();
    m_hash.Clear();

    return false;
}


bool wxUpdCheckThread::DownloadUpdatePackage()
{
    // Calculate file hash (if exists).
    wxCryptoHashSHA1 ch(*m_cs);
    if (ch.HashFile(m_fileName)) {
        wxMemoryBuffer buf;
        ch.GetValue(buf);

        if (buf.GetDataLen() == m_hash.GetDataLen() &&
            memcmp(buf.GetData(), m_hash.GetData(), buf.GetDataLen()) == 0)
        {
            // Update package file exists and its hash is correct.
            wxLogStatus(_("Update package file already downloaded."));
            return true;
        }
    }

    wxFFile file(m_fileName, wxT("wb"));
    if (!file.IsOpened()) {
        wxLogError(_("Can not open/create %s file for writing."), m_fileName.c_str());
        return false;
    }

    // Download update package file.
    for (size_t i = 0, n = m_urls.GetCount(); i < n; i++) {
        if (TestDestroy()) return false;

        wxURL url(m_urls[i]);
        if (!url.IsOk())
            continue;

        if (url.HasScheme()) {
            const wxString scheme = url.GetScheme();
            if (scheme == wxT("http") || scheme == wxT("https")) {
                wxHTTP &http = (wxHTTP&)url.GetProtocol();
                http.SetHeader(wxS("User-Agent"), wxS("Updater/") wxS(UPDATER_VERSION_STR));
            }
        }

        wxLogStatus(_("Downloading update package from %s..."), m_urls[i].c_str());
        wxScopedPtr<wxInputStream> stream(url.GetInputStream());
        if (!stream) {
            wxLogWarning(_("Error response received."));
            continue;
        }

        // Save update package to file, and calculate hash.
        wxCryptoHashSHA1 ch(*m_cs);
        wxMemoryBuffer buf(4*1024);
        char *data = (char*)buf.GetData();
        size_t nBlock = buf.GetBufSize();
        do {
            if (TestDestroy()) return false;

            stream->Read(data, nBlock);
            size_t nRead = stream->LastRead();
            file.Write(data, nRead);
            if (file.Error()) {
                wxLogError(_("Can not write to %s file."), m_fileName.c_str());
                return false;
            }
            ch.Hash(data, nRead);
        } while (stream->IsOk());
        ch.GetValue(buf);

        if (buf.GetDataLen() == m_hash.GetDataLen() &&
            memcmp(buf.GetData(), m_hash.GetData(), buf.GetDataLen()) == 0)
        {
            // Update package file downloaded and its hash is correct.
            return true;
        } else
            wxLogWarning(_("Update package file corrupt."));
    }

    return false;
}


bool wxUpdCheckThread::LaunchUpdate(WXHWND hParent, bool headless)
{
    wxLogStatus(_("Launching update..."));

    wxString param(headless ? wxT("/qn ") : wxEmptyString);

    // Package
    param += wxT("/i \"");
    param += m_fileName;
    param += wxT("\"");

    // Logging
    wxString fileNameLog(m_path);
    fileNameLog += wxT("Updater-") wxT(UPDATER_CFG_APPLICATION) wxT("-msiexec.log");

    param += wxT(" /l* \"");
    param += fileNameLog;
    param += wxT("\"");

    int result = (int)::ShellExecute(hParent, NULL, wxT("msiexec.exe"), param, NULL, SW_SHOWNORMAL);
    if (result > 32) {
        wxLogStatus(_("msiexec.exe launch succeeded. For detailed information, see %s file."), fileNameLog.c_str());
        return true;
    } else {
        wxLogError(_("msiexec.exe launch failed. ShellExecute returned %i."), result);
        return false;
    }
}
