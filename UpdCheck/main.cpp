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


///
/// Module initializer
///
class wxUpdCheckInitializer : public wxInitializer
{
protected:
    std::ofstream m_log;

public:
    wxConfig m_config;
    wxString m_path;
    wxLocale m_locale;

public:
    wxUpdCheckInitializer();
};


wxUpdCheckInitializer::wxUpdCheckInitializer() :
    m_config(wxT(UPDATER_CFG_APPLICATION) wxT("\\Updater"), wxT(UPDATER_CFG_VENDOR)),
    wxInitializer()
{
    if (wxInitializer::IsOk()) {
        // Set desired locale.
        wxLanguage language = (wxLanguage)m_config.Read(wxT("Language"), wxLANGUAGE_DEFAULT);
        if (wxLocale::IsAvailable(language))
            wxVERIFY(m_locale.Init(language));

        m_path = wxFileName::GetTempDir();
        if (!wxEndsWithPathSeparator(m_path))
            m_path += wxFILE_SEP_PATH;

        if (!wxDirExists(m_path))
            wxMkdir(m_path);

        m_log.open((LPCTSTR)(m_path + wxT("Updater-") wxT(UPDATER_CFG_APPLICATION) wxT(".log")), std::ios_base::out | std::ios_base::trunc);
        if (m_log.is_open())
            delete wxLog::SetActiveTarget(new wxLogStream(&m_log));
    }
}


///
/// Main function
///
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    // Initialize wxWidgets.
    wxUpdCheckInitializer initializer;
    if (!initializer.IsOk())
        return -1;

    // Create RSA AES cryptographic context.
    wxCryptoSessionRSAAES cs;
    if (!cs.IsOk())
        return -1;

    // Import public key.
    wxCryptoKey ck;
    {
        HRSRC res = ::FindResource(NULL, MAKEINTRESOURCE(IDR_KEY_PUBLIC), RT_RCDATA);
        wxASSERT_MSG(res, wxT("public key not found"));
        HGLOBAL res_handle = ::LoadResource(NULL, res);
        wxASSERT_MSG(res_handle, wxT("loading resource failed"));

        if (!ck.ImportPublic(cs, ::LockResource(res_handle), ::SizeofResource(NULL, res)))
            return -1;
    }

    long val;
    wxDateTime last_checked = initializer.m_config.Read(wxT("LastChecked"), &val) ? wxDateTime((time_t)val) : wxInvalidDateTime;

    for (const wxChar *server = wxT(UPDATER_HTTP_SERVER); server[0]; server += wcslen(server) + 1) {
        wxXmlDocument doc;

        wxLogStatus(wxT("Contacting repository at %s..."), server);

        // Load repository database.
        wxHTTP http;
        http.SetHeader(wxS("User-Agent"), wxS("Updater/") wxS(UPDATER_VERSION_STR));
        if (last_checked.IsValid())
            http.SetHeader(wxS("If-Modified-Since"), last_checked.Format(wxS("%a, %d %b %Y %H:%M:%S %z")));
        if (!http.Connect(server, UPDATER_HTTP_PORT)) {
            wxLogWarning(wxT("Error resolving %s server name."), server);
            continue;
        }
        wxInputStream *httpStream = http.GetInputStream(wxS(UPDATER_HTTP_PATH));
        if (!httpStream) {
            if (http.GetResponse() == 304) {
                wxLogStatus(wxT("Repository did not change since the last time..."));
                return 0;
            }

            wxLogWarning(wxT("Error response received from server %s (port %u) requesting %s."), server, UPDATER_HTTP_PORT, UPDATER_HTTP_PATH);
            continue;
        }
        wxLogStatus(wxT("Loading repository catalogue..."));
        if (!doc.Load(*httpStream, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES)) {
            wxLogWarning(wxT("Error loading repository catalogue."));
            wxDELETE(httpStream);
            http.Close();
            continue;
        }
        wxDELETE(httpStream);
        http.Close();

        wxLogStatus(wxT("Verifying repository catalogue signature..."));

        // Find the (first) signature.
        wxXmlNode *document = doc.GetDocumentNode();
        std::vector<BYTE> sig;
        for (wxXmlNode *prolog = document->GetChildren(); prolog; prolog = prolog->GetNext()) {
            if (prolog->GetType() == wxXML_COMMENT_NODE) {
                wxString content = prolog->GetContent();
                size_t content_len = content.length();
                if (content_len >= _countof(wxS(UPDATER_SIGNATURE_MARK)) - 1 &&
                    memcmp((const wxStringCharType*)content, wxS(UPDATER_SIGNATURE_MARK), sizeof(wxStringCharType)*(_countof(wxS(UPDATER_SIGNATURE_MARK)) - 1)) == 0)
                {
                    // Read the signature.
                    size_t signature_len = content_len - (_countof(wxS(UPDATER_SIGNATURE_MARK)) - 1);
                    sig.resize(wxBase64DecodedSize(signature_len));
                    size_t res = wxBase64Decode(sig.data(), sig.capacity(), content.Right(signature_len), wxBase64DecodeMode_SkipWS);
                    if (res != wxCONV_FAILED) {
                        sig.resize(res);

                        // Remove the signature as it is not a part of the validation check.
                        document->RemoveChild(prolog);
                        wxDELETE(prolog);
                        break;
                    } else
                        sig.clear();
                }
            }
        }

        if (sig.empty()) {
            wxLogWarning(wxT("Signature not found in the repository catalogue."));
            continue;
        }

        // Hash the content.
        wxCryptoHashSHA1 ch(cs);
        if (!wxXmlHashNode(ch, document))
            continue;

        // We have the hash, we have the signature, we have the public key. Now verify.
        if (!wxCryptoVerifySignature(ch, sig.data(), sig.size(), ck)) {
            wxLogWarning(wxT("Repository catalogue signature does not match its content, or signature verification failed."));
            continue;
        }

        // The signature is correct. Now parse the file.
        wxLogStatus(wxT("Parsing repository catalogue..."));

        // Start processing the XML file.
        wxXmlNode *elRoot = doc.GetRoot();
        const wxString &nameRoot = elRoot->GetName();
        if (nameRoot != wxT("Packages")) {
            wxLogWarning(wxT("Invalid root element in repository catalogue (actual: %s, expected %s)."), nameRoot.c_str(), wxT("Packages"));
            continue;
        }

        // Iterate over packages.
        wxUint32 versionMax = 0;
        wxString versionStrMax;
        std::vector<wxString> urlsMax;
        std::vector<BYTE> hashMax;
        for (wxXmlNode *elPackage = elRoot->GetChildren(); elPackage; elPackage = elPackage->GetNext()) {
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
                if (version <= UPDATER_PRODUCT_VERSION || version < versionMax) {
                    // This package is older than currently installed product or the superseeded package found.
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
                wxString languageId(initializer.m_locale.GetCanonicalName());
                std::vector<wxString> urls;
                std::vector<BYTE> hash;
                for (wxXmlNode *elDownloads = elPackage->GetChildren(); elDownloads; elDownloads = elDownloads->GetNext()) {
                    if (elDownloads->GetType() == wxXML_ELEMENT_NODE && elDownloads->GetName() == wxT("Downloads")) {
                        for (wxXmlNode *elPlatform = elDownloads->GetChildren(); elPlatform; elPlatform = elPlatform->GetNext()) {
                            if (elPlatform->GetType() == wxXML_ELEMENT_NODE) {
                                if (elPlatform->GetName() == wxT("Platform") && elPlatform->GetAttribute(wxT("id")) == platformId) {
                                    // Get language.
                                    for (wxXmlNode *elLocale = elPlatform->GetChildren(); elLocale; elLocale = elLocale->GetNext()) {
                                        if (elLocale->GetType() == wxXML_ELEMENT_NODE && elLocale->GetName() == wxT("Localization") && elLocale->GetAttribute(wxT("lang")) == languageId) {
                                            for (wxXmlNode *elLocaleNote = elLocale->GetChildren(); elLocaleNote; elLocaleNote = elLocaleNote->GetNext()) {
                                                if (elLocaleNote->GetType() == wxXML_ELEMENT_NODE) {
                                                    const wxString &name = elLocaleNote->GetName();
                                                    if (name == wxT("url"))
                                                        urls.push_back(elLocaleNote->GetNodeContent());
                                                    else if (name == wxT("hash")) {
                                                        // Read the hash.
                                                        wxString content(elLocaleNote->GetNodeContent());
                                                        hash.resize(wxHexDecodedSize(content.length()));
                                                        size_t res = wxHexDecode(hash.data(), hash.capacity(), content, wxHexDecodeMode_SkipWS);
                                                        if (res != wxCONV_FAILED)
                                                            hash.resize(res);
                                                        else
                                                            hash.clear();
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
                if (urls.empty() || hash.empty()) {
                    // This package is for different architecture and/or language.
                    // ... or missing URL and/or hash.
                    continue;
                }

                versionMax = version;
                versionStrMax = versionStr;
                urlsMax = urls;
                hashMax = hash;
            }
        }

        if (versionMax) {
            wxLogMessage(wxT("Update package found (version: %s)."), versionStrMax.c_str());

            wxString fileName(initializer.m_path);
            fileName += wxT("Updater-") wxT(UPDATER_CFG_APPLICATION) wxT("-");
            fileName += versionStrMax;
            fileName += wxT(".msi");

            bool isLocal = false;

            if (wxFileExists(fileName)) {
                // The update package file already exists. Do the integrity check.
                std::ifstream file((LPCTSTR)fileName, std::ios_base::in | std::ios_base::binary);
                if (file.is_open()) {
                    // Calculate file hash.
                    wxCryptoHashSHA1 ch(cs);
                    wxMemoryBuffer buf(4*1024);
                    char *data = (char*)buf.GetData();
                    size_t nBlock = buf.GetBufSize();
                    do {
                        file.read(data, nBlock);
                        ch.Hash(data, file.gcount());
                    } while (file.good());
                    ch.GetValue(buf);

                    if (buf.GetDataLen() == hashMax.size() &&
                        memcmp(buf.GetData(), hashMax.data(), buf.GetDataLen()) == 0)
                    {
                        // Update package file exists and its hash is correct.
                        wxLogStatus(wxT("Update package file already downloaded."));
                        isLocal = true;
                    }
                }
            }

            if (!isLocal) {
                std::ofstream file((LPCTSTR)fileName, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
                if (!file.is_open()) {
                    wxLogError(wxT("Can not open/create %s file for writing."), fileName.c_str());
                    goto quit; // This condition has no chance to succeed with the next download server. Quit.
                }

                // Download update package file.
                for (std::vector<wxString>::const_iterator i = urlsMax.cbegin(), i_end = urlsMax.end(); i != i_end; ++i) {
                    wxURL url(*i);
                    if (!url.IsOk())
                        continue;

                    wxLogStatus(wxT("Downloading update package from %s..."), i->c_str());
                    wxInputStream *stream = url.GetInputStream();
                    if (!stream) {
                        wxLogWarning(wxT("Error response received."));
                        continue;
                    }

                    // Save update package to file, and calculate hash.
                    wxCryptoHashSHA1 ch(cs);
                    wxMemoryBuffer buf(4*1024);
                    char *data = (char*)buf.GetData();
                    size_t nBlock = buf.GetBufSize();
                    do {
                        stream->Read(data, nBlock);
                        size_t nRead = stream->LastRead();
                        if (file.fail()) {
                            wxLogError(wxT("Can not write to %s file."), fileName.c_str());
                            goto quit; // This condition has no chance to succeed with the next download server. Quit.
                        }
                        file.write(data, nRead);
                        ch.Hash(data, nRead);
                    } while (stream->IsOk());
                    ch.GetValue(buf);

                    if (buf.GetDataLen() == hashMax.size() &&
                        memcmp(buf.GetData(), hashMax.data(), buf.GetDataLen()) == 0)
                    {
                        // Update package file exists and its hash is correct.
                        isLocal = true;
                        break;
                    } else
                        wxLogWarning(wxT("Update package file corrupt."));
                }
            }

            if (isLocal) {
                wxLogStatus(wxT("Launching update..."));

                // Headless Install
                wxString param("/qn");

                // Package
                param += wxT(" /i \"");
                param += fileName;
                param += wxT("\"");

                // Logging
                wxString fileNameLog(initializer.m_path + wxT("Updater-") wxT(UPDATER_CFG_APPLICATION) wxT("-msiexec.log"));

                param += wxT(" /l* \"");
                param += fileNameLog;
                param += wxT("\"");

                if ((int)::ShellExecute(NULL, NULL, wxT("msiexec.exe"), param, NULL, SW_SHOWNORMAL) > 32) {
                    wxLogStatus(wxT("For further information, see %s file."), fileNameLog.c_str());
                    break;
                } else {
                    wxLogError(wxT("msiexec.exe launch failed."));
                    goto quit; // This condition has no chance to succeed with the next download server. Quit.
                }
            } else
                wxLogWarning(wxT("Update package file download failed."));
        } else
            wxLogStatus(wxT("Update check complete. Your package is up to date."));

        // Save the last check date.
        initializer.m_config.Write(wxT("LastChecked"), (long)wxDateTime::GetTimeNow());

        return 0;
    }

quit:
    // No success.
    return 1;
}
