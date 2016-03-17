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
#pragma comment(lib, "Crypt32.lib")


///
/// Main function
///
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    // Inizialize wxWidgets.
    wxInitializer initializer;
    if (!initializer)
        return -1;

    wxXmlDocument doc;

    // Load repository database.
    wxHTTP http;
    //wxDateTime timestamp_last_checked;
    //http.SetHeader(wxS("If-Modified-Since"), timestamp_last_checked.Format(wxS("%a, %d %b %y %T %z")));
    if (!http.Connect(wxS(UPDATER_HTTP_SERVER), UPDATER_HTTP_PORT)) {
        wxFAIL_MSG(wxT("Error resolving server name."));
        return 1;
    }
    wxInputStream *httpStream = http.GetInputStream(wxS(UPDATER_HTTP_PATH));
    if (!doc.Load(*httpStream, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES)) {
        wxFAIL_MSG(wxT("Error reading data file."));
        return 1;
    }
    wxDELETE(httpStream);
    http.Close();

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

    if (sig.empty())
        wxFAIL_MSG(wxT("Signature not found in the Updater file."));

    // Reverse byte order, for consistent OpenSSL experience.
    for (std::vector<BYTE>::size_type i = 0, j  = sig.size() - 1; i < j; i++, j--)
       std::swap(sig[i], sig[j]);

    // Create cryptographic context.
    HCRYPTPROV cp = NULL;
    wxVERIFY(::CryptAcquireContext(&cp, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT));

    // Hash the content.
    HCRYPTHASH ch = NULL;
    wxVERIFY(::CryptCreateHash(cp, CALG_SHA1, 0, 0, &ch));
    ::wxXmlHashNode(ch, document);

    // Import public key.
    HCRYPTKEY ck = NULL;
    {
        HRSRC res = ::FindResource(NULL, MAKEINTRESOURCE(IDR_KEY_PUBLIC), RT_RCDATA);
        wxASSERT_MSG(res, wxT("public key not found"));
        HGLOBAL res_handle = ::LoadResource(NULL, res);
        wxASSERT_MSG(res_handle, wxT("loading resource failed"));

        CERT_PUBLIC_KEY_INFO *keyinfo_data = NULL;
        DWORD keyinfo_size = 0;
        wxVERIFY(::CryptDecodeObjectEx(X509_ASN_ENCODING, X509_PUBLIC_KEY_INFO, (const BYTE*)::LockResource(res_handle), ::SizeofResource(NULL, res), CRYPT_DECODE_ALLOC_FLAG, NULL, &keyinfo_data, &keyinfo_size));

        wxVERIFY(::CryptImportPublicKeyInfo(cp, X509_ASN_ENCODING, keyinfo_data, &ck));
        ::LocalFree(keyinfo_data);
    }

    // We have the hash, we have the signature, we have the public key. Now verify.
    if (::CryptVerifySignature(ch, sig.data(), sig.size(), ck, NULL, 0)) {
        // The signature is correct. Now parse the file.
    } else
        wxFAIL_MSG(wxT("Updater file signature does not match file content."));

    wxVERIFY(::CryptDestroyKey(ck));
    wxVERIFY(::CryptDestroyHash(ch));
    wxVERIFY(::CryptReleaseContext(cp, 0));

    return 0;
}
