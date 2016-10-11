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


void UpdaterAddURL(wxXmlNode *elLocale, const wxString &url)
{
    wxXmlNode *elUrl = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("url"));
    elUrl->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, url));

    elLocale->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\t")));
    elLocale->AddChild(elUrl);
    elLocale->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t\t")));
}


void UpdaterAddHash(wxXmlNode *elLocale, const wxMemoryBuffer &hash)
{
    wxXmlNode *elHash = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("hash"));
    elHash->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxHexEncode(hash)));

    elLocale->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\t")));
    elLocale->AddChild(elHash);
    elLocale->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t\t")));
}


void UpdaterAddLocalization(wxXmlNode *elPlatform, const wxString &languageId, const wxString &url, const wxMemoryBuffer &hash)
{
    wxXmlNode *elLocale = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Localization"));
    elLocale->AddAttribute(new wxXmlAttribute(wxT("lang"), languageId));
    elLocale->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t\t")));
    UpdaterAddURL(elLocale, url);
    if (!hash.IsEmpty())
        UpdaterAddHash(elLocale, hash);

    elPlatform->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\t")));
    elPlatform->AddChild(elLocale);
    elPlatform->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t")));
}


void UpdaterAddPlatform(wxXmlNode *elDownloads, const wxString &platformId, const wxString &languageId, const wxString &url, const wxMemoryBuffer &hash)
{
    wxXmlNode *elPlatform = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Platform"));
    elPlatform->AddAttribute(new wxXmlAttribute(wxT("id"), platformId));
    elPlatform->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t")));
    UpdaterAddLocalization(elPlatform, languageId, url, hash);

    elDownloads->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\t")));
    elDownloads->AddChild(elPlatform);
    elDownloads->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t")));
}


void UpdaterAddDownloads(wxXmlNode *elPackage, const wxString &platformId, const wxString &languageId, const wxString &url, const wxMemoryBuffer &hash)
{
    wxXmlNode *elDownloads = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Downloads"));
    elDownloads->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t")));
    UpdaterAddPlatform(elDownloads, platformId, languageId, url, hash);

    elPackage->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\t")));
    elPackage->AddChild(elDownloads);
    elPackage->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t")));
}


void UpdaterAddPackage(wxXmlNode *elPackages, const wxString &platformId, const wxString &languageId, const wxString &url, const wxMemoryBuffer &hash)
{
    wxXmlNode *elHex = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("hex"));
    elHex->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format(wxT("%08x"), PRODUCT_VERSION)));

    wxXmlNode *elDesc = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("desc"));
    elDesc->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT(PRODUCT_VERSION_STR)));

    wxXmlNode *elVersion = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Version"));
    elVersion->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t")));
    elVersion->AddChild(elHex);
    elVersion->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t\t")));
    elVersion->AddChild(elDesc);
    elVersion->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t")));

    wxXmlNode *elPackage = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Package"));
    elPackage->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t\t")));
    elPackage->AddChild(elVersion);
    elPackage->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n\t")));
    UpdaterAddDownloads(elPackage, platformId, languageId, url, hash);

    elPackages->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\t")));
    elPackages->AddChild(elPackage);
    elPackages->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("\n")));
}


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
        { wxCMD_LINE_SWITCH, "h" , "help", _("Show this help message"                   ), wxCMD_LINE_VAL_NONE  , wxCMD_LINE_OPTION_HELP      },
        { wxCMD_LINE_OPTION, "f" , "file", _("Package file to calculate SHA-1 hash from"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL   },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<input repository file>"                  ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<output repository file>"                 ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<platform>"                               ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<language>"                               ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM , NULL, NULL  , _("<download URL>"                           ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },

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

    // Verify document type (root element).
    wxXmlNode *elPackages = doc.GetRoot();
    const wxString &nameRoot = elPackages->GetName();
    if (nameRoot != wxT("Packages")) {
        wxLogWarning(wxT("Invalid root element in repository catalogue (actual: %s, expected %s)."), nameRoot.c_str(), wxT("Packages"));
        return 2;
    }

    // Get package file hash.
    wxString filenamePckg;
    wxMemoryBuffer hash;
    if (parser.Found(wxT("f"), &filenamePckg)) {
        // Create RSA AES cryptographic session.
        wxCryptoSessionRSAAES cs;
        wxCHECK(cs.IsOk(), -1);

        // Calculate file SHA-1 hash.
        wxCryptoHashSHA1 ch(cs);
        wxCHECK(ch.HashFile(filenamePckg), 3);
        ch.GetValue(hash);
    }

    // Iterate over packages.
    bool url_present = false;
    const wxString& platformId = parser.GetParam(2);
    const wxString& language = parser.GetParam(3);
    const wxString& url = parser.GetParam(4);
    wxString languageId(language);
    for (wxXmlNode *elPackage = elPackages->GetChildren(); elPackage; elPackage = elPackage->GetNext()) {
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
            if (version != PRODUCT_VERSION) {
                // This package is not our version. Skip.
                continue;
            }

            // Set package download URL for given platform and language.
            for (wxXmlNode *elDownloads = elPackage->GetChildren(); elDownloads; elDownloads = elDownloads->GetNext()) {
                if (elDownloads->GetType() == wxXML_ELEMENT_NODE && elDownloads->GetName() == wxT("Downloads")) {
                    for (wxXmlNode *elPlatform = elDownloads->GetChildren(); elPlatform; elPlatform = elPlatform->GetNext()) {
                        if (elPlatform->GetType() == wxXML_ELEMENT_NODE) {
                            if (elPlatform->GetName() == wxT("Platform") && elPlatform->GetAttribute(wxT("id")) == platformId) {
                                // Get language.
                                for (wxXmlNode *elLocale = elPlatform->GetChildren(); elLocale; elLocale = elLocale->GetNext()) {
                                    if (elLocale->GetType() == wxXML_ELEMENT_NODE && elLocale->GetName() == wxT("Localization") && elLocale->GetAttribute(wxT("lang")) == languageId) {
                                        bool hash_present = false;
                                        for (wxXmlNode *elLocaleNote = elLocale->GetChildren(); elLocaleNote; elLocaleNote = elLocaleNote->GetNext()) {
                                            if (elLocaleNote->GetType() == wxXML_ELEMENT_NODE) {
                                                const wxString &name = elLocaleNote->GetName();
                                                if (name == wxT("url")) {
                                                    if (elLocaleNote->GetNodeContent() == url)
                                                        url_present = true;
                                                } else if (!hash.IsEmpty() && name == wxT("hash")) {
                                                    // Read the hash.
                                                    wxMemoryBuffer hashOrig;
                                                    wxString content(elLocaleNote->GetNodeContent());
                                                    size_t len = wxHexDecodedSize(content.length());
                                                    size_t res = wxHexDecode(hashOrig.GetWriteBuf(len), len, content, wxHexDecodeMode_SkipWS);
                                                    if (res != wxCONV_FAILED) {
                                                        hashOrig.SetDataLen(res);
                                                        if (hash.GetDataLen() == hashOrig.GetDataLen() &&
                                                            memcmp(hash.GetData(), hashOrig.GetData(), hash.GetDataLen()) == 0)
                                                        {
                                                            hash_present = true;
                                                        } else {
                                                            wxLogError(wxT("This update package is already in the database. However, its hash is different."));
                                                            return 3;
                                                        }
                                                    } else {
                                                        wxLogError(wxT("This update package is already in the database. However, its hash is corrupt."));
                                                        return 4;
                                                    }
                                                }
                                            }
                                        }

                                        if (!url_present) {
                                            // Add URL.
                                            UpdaterAddURL(elLocale, url);
                                            url_present = true;
                                        }
                                        if (!hash_present && !hash.IsEmpty()) {
                                            // Add hash.
                                            UpdaterAddHash(elLocale, hash);
                                        }
                                    }
                                }

                                if (!url_present) {
                                    // Add localization.
                                    UpdaterAddLocalization(elPlatform, languageId, url, hash);
                                    url_present = true;
                                }
                            }
                        }
                    }

                    if (!url_present) {
                        // Add platform.
                        UpdaterAddPlatform(elDownloads, platformId, languageId, url, hash);
                        url_present = true;
                    }
                }
            }

            if (!url_present) {
                // Add downloads.
                UpdaterAddDownloads(elPackage, platformId, languageId, url, hash);
                url_present = true;
            }
        }
    }

    if (!url_present) {
        // Add package.
        UpdaterAddPackage(elPackages, platformId, languageId, url, hash);
        url_present = true;
    }


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
