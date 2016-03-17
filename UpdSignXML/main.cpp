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
int _tmain(int argc, _TCHAR *argv[])
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    // Inizialize wxWidgets.
    wxInitializer initializer;
    if (!initializer) {
        _ftprintf(stderr, wxT("Failed to initialize the wxWidgets library, aborting.\n"));
        return -1;
    }

    // Parse command line.
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h" , "help", _("Show this help message"), wxCMD_LINE_VAL_NONE  , wxCMD_LINE_OPTION_HELP      },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("input file"            ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("output file"           ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },

        { wxCMD_LINE_NONE }
    };
    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    switch (parser.Parse()) {
    case -1:
        // Help was given, terminating.
        return 0;

    case 0:
        // everything is ok; proceed
        break;

    default:
        wxLogMessage(wxT("Syntax error detected, aborting."));
        return -1;
    }

    const wxString& filenameIn = parser.GetParam(0);
    wxXmlDocument doc;
    if (!doc.Load(filenameIn, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES)) {
        _ftprintf(stderr, wxT("%s: error UMD0001: Error opening input file.\n"), filenameIn.fn_str());
        return 1;
    }

    bool has_errors = false;

    wxXmlNode *document = doc.GetDocumentNode();

    // Examine prologue if the document is already signed.
    for (wxXmlNode *prolog = document->GetChildren(); prolog; prolog = prolog->GetNext()) {
        if (prolog->GetType() == wxXML_COMMENT_NODE) {
            wxString content = prolog->GetContent();
            if (content.length() >= _countof(wxS(UPDATER_SIGNATURE_MARK)) - 1 &&
                memcmp((const wxStringCharType*)content, wxS(UPDATER_SIGNATURE_MARK), sizeof(wxStringCharType)*(_countof(wxS(UPDATER_SIGNATURE_MARK)) - 1)) == 0)
            {
                // Previous signature found. Remove it.
                document->RemoveChild(prolog);
            }
        }
    }

    // Create cryptographic context.
    HCRYPTPROV cp = NULL;
    wxVERIFY(::CryptAcquireContext(&cp, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT));

    // Import private key.
    {
        HRSRC res = ::FindResource(NULL, MAKEINTRESOURCE(IDR_KEY_PRIVATE), RT_RCDATA);
        wxASSERT_MSG(res, wxT("private key not found"));
        HGLOBAL res_handle = ::LoadResource(NULL, res);
        wxASSERT_MSG(res_handle, wxT("loading resource failed"));

        PUBLICKEYSTRUC *key_data = NULL;
        DWORD key_size = 0;
        wxVERIFY(::CryptDecodeObjectEx(X509_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, (const BYTE*)::LockResource(res_handle), ::SizeofResource(NULL, res), CRYPT_DECODE_ALLOC_FLAG, NULL, &key_data, &key_size));
        // See: http://pumka.net/2009/12/16/rsa-encryption-cplusplus-delphi-cryptoapi-php-openssl-2/comment-page-1/#comment-429
        key_data->aiKeyAlg = CALG_RSA_SIGN;

        HCRYPTKEY ck = NULL;
        wxVERIFY(::CryptImportKey(cp, (const BYTE*)key_data, key_size, NULL, 0, &ck));
        wxVERIFY(::CryptDestroyKey(ck));
        ::LocalFree(key_data);
    }

    // Hash the content.
    HCRYPTHASH ch = NULL;
    wxVERIFY(::CryptCreateHash(cp, CALG_SHA1, 0, 0, &ch));
    ::wxXmlHashNode(ch, document);

    // Sign the hash.
    DWORD sig_size = 0;
    if (!::CryptSignHash(ch, AT_SIGNATURE, NULL, 0, NULL, &sig_size)) {
        _ftprintf(stderr, wxT("%s: error UMD0003: Signing input file failed (%i).\n"), filenameIn.fn_str(), ::GetLastError());
        has_errors = true;
    }
    std::vector<BYTE> sig(sig_size);
    if (!::CryptSignHash(ch, AT_SIGNATURE, NULL, 0, sig.data(), &sig_size)) {
        _ftprintf(stderr, wxT("%s: error UMD0003: Signing input file failed (%i).\n"), filenameIn.fn_str(), ::GetLastError());
        has_errors = true;
    }
    wxVERIFY(::CryptDestroyHash(ch));
    wxVERIFY(::CryptReleaseContext(cp, 0));

    // Reverse byte order, for consistent OpenSSL experience.
    for (std::vector<BYTE>::size_type i = 0, j  = sig.size() - 1; i < j; i++, j--)
       std::swap(sig[i], sig[j]);

    // Encode signature (Base64) and append to the document prolog.
    wxString signature;
    signature += wxS(UPDATER_SIGNATURE_MARK);
    signature += wxBase64Encode(sig.data(), sig.size());
    document->AddChild(new wxXmlNode(wxXML_COMMENT_NODE, wxS(""), signature));

    const wxString& filenameOut = parser.GetParam(1);
    if (!doc.Save(filenameOut, wxXML_NO_INDENTATION)) {
        _ftprintf(stderr, wxT("%s: error UMD0001: Error writing output file.\n"), filenameOut.fn_str());
        has_errors = true;
    }

    if (has_errors) {
        wxRemoveFile(filenameOut);
        return 1;
    } else
        return 0;
}
