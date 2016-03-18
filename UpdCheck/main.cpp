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
    FILE *m_pLogFile;

public:
    wxUpdCheckInitializer();
    virtual ~wxUpdCheckInitializer();
};


wxUpdCheckInitializer::wxUpdCheckInitializer() :
    m_pLogFile(NULL),
    wxInitializer()
{
    if (wxInitializer::IsOk()) {
        wxString str(wxFileName::GetTempDir());
        str += wxFILE_SEP_PATH;
        str += wxT(UPDATER_LOG_FILE);
        m_pLogFile = fopen(str, "w+");
        if (m_pLogFile)
            delete wxLog::SetActiveTarget(new wxLogStderr(m_pLogFile));
    } else
        m_pLogFile = NULL;
}


wxUpdCheckInitializer::~wxUpdCheckInitializer()
{
    if (m_pLogFile != NULL) {
        delete wxLog::SetActiveTarget(NULL);
        fclose(m_pLogFile);
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
    if (!cs.IsOk()) {
        wxLogError(wxT("Failed to create cryptographics session."));
        return -1;
    }

    // Import public key.
    wxCryptoKey ck;
    {
        HRSRC res = ::FindResource(NULL, MAKEINTRESOURCE(IDR_KEY_PUBLIC), RT_RCDATA);
        wxASSERT_MSG(res, wxT("public key not found"));
        HGLOBAL res_handle = ::LoadResource(NULL, res);
        wxASSERT_MSG(res_handle, wxT("loading resource failed"));

        if (!ck.ImportPublic(cs, ::LockResource(res_handle), ::SizeofResource(NULL, res))) {
            wxLogError(wxT("Failed to import public key."));
            return -1;
        }
    }

    for (const wxChar *server = wxT(UPDATER_HTTP_SERVER); server[0]; server += wcslen(server) + 1) {
        wxXmlDocument doc;

        wxLogStatus(wxT("Contacting repository at %s..."), server);

        // Load repository database.
        wxHTTP http;
        //wxDateTime timestamp_last_checked;
        //http.SetHeader(wxS("If-Modified-Since"), timestamp_last_checked.Format(wxS("%a, %d %b %y %T %z")));
        if (!http.Connect(server, UPDATER_HTTP_PORT)) {
            wxLogWarning(wxT("Error resolving %s server name."), server);
            continue;
        }
        wxInputStream *httpStream = http.GetInputStream(wxS(UPDATER_HTTP_PATH));
        if (!httpStream) {
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
        if (!wxXmlHashNode(ch, document)) {
            wxLogWarning(wxT("Failed to hash the repository catalogue."));
            continue;
        }

        // We have the hash, we have the signature, we have the public key. Now verify.
        if (!wxCryptoVerifySignature(ch, sig.data(), sig.size(), ck)) {
            wxLogWarning(wxT("Repository catalogue signature does not match its content, or signature verification failed."));
            continue;
        }

        // The signature is correct. Now parse the file.
        wxLogStatus(wxT("Parsing repository catalogue..."));

        return 0;
    }

    // No success.
    return 1;
}
