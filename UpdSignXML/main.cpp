/*
    SPDX-License-Identifier: GPL-3.0-or-later
    Copyright © 2016-2021 Amebis
*/

#include "pch.h"


///
/// Main function
///
int _tmain(int argc, _TCHAR *argv[])
{
    wxApp::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");

    // Initialize wxWidgets.
    wxInitializer initializer;
    if (!initializer.IsOk())
        return -1;

    // Parse command line.
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h" , "help", _("Show this help message"), wxCMD_LINE_VAL_NONE  , wxCMD_LINE_OPTION_HELP      },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<input file>"          ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<output file>"         ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },

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
        wxLogWarning(wxT("Syntax error detected, aborting."));
        return -1;
    }

    // Load input XML document.
    const wxString& filenameIn = parser.GetParam(0);
    wxXmlDocument doc;
    if (!doc.Load(filenameIn, "UTF-8", wxXMLDOC_KEEP_WHITESPACE_NODES))
        return 1;

    // Examine prologue if the document is already signed and remove all signatures found.
    wxXmlNode *document = doc.GetDocumentNode();
    for (wxXmlNode *prolog = document->GetChildren(); prolog;) {
        #pragma warning(suppress: 26812) // wxXmlNodeType is unscoped
        if (prolog->GetType() == wxXML_COMMENT_NODE) {
            wxString content = prolog->GetContent();
            if (content.length() >= _countof(wxS(UPDATER_SIGNATURE_MARK)) - 1 &&
                memcmp((const wxStringCharType*)content, wxS(UPDATER_SIGNATURE_MARK), sizeof(wxStringCharType)*(_countof(wxS(UPDATER_SIGNATURE_MARK)) - 1)) == 0)
            {
                // Previous signature found. Remove it.
                wxXmlNode *signature = prolog;
                prolog = prolog->GetNext();
                document->RemoveChild(signature);
                wxDELETE(signature);
                continue;
            }
        }

        prolog = prolog->GetNext();
    }

    // Create RSA AES cryptographic session.
    wxCryptoSessionRSAAES cs;
    if (!cs.IsOk())
        return -1;

    // Import the private key into the session from resources.
    {
        HRSRC res = ::FindResource(NULL, MAKEINTRESOURCE(IDR_KEY_PRIVATE), RT_RCDATA);
        if (!res) {
            wxLogError(wxT("%s: error USX0005: Error locating private key.\n"));
            return -1;
        }
        HGLOBAL res_handle = ::LoadResource(NULL, res);
        if (!res_handle) {
            wxLogError(wxT("%s: error USX0006: Error loading private key.\n"));
            return -1;
        }

        wxCryptoKey ck;
        if (!ck.ImportPrivate(cs, ::LockResource(res_handle), ::SizeofResource(NULL, res))) {
            wxLogError(wxT("%s: error USX0007: Error importing private key.\n"));
            return -1;
        }
    }

    // Hash the XML content.
    wxUpdaterHashGen ch(cs);
    if (!wxXmlHashNode(ch, document))
        return 2;

    // Sign the hash.
    wxMemoryBuffer sig;
    if (!ch.Sign(sig))
        return 3;

    // Encode signature (Base64) and append to the document prolog.
    wxString signature;
    signature += wxS(UPDATER_SIGNATURE_MARK);
    signature += wxBase64Encode(sig);
    document->AddChild(new wxXmlNode(wxXML_COMMENT_NODE, wxS(""), signature));

    // Write output XML document.
    const wxString& filenameOut = parser.GetParam(1);
    if (!doc.Save(filenameOut, wxXML_NO_INDENTATION)) {
        wxLogError(wxT("%s: error USX0004: Error writing output file.\n"), filenameOut.fn_str());

        // Remove the output file (if exists).
        wxRemoveFile(filenameOut);

        return 4;
    }

    return 0;
}
