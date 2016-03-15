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
/// Writes package information to a stream
///
/// \param[in] stream  Output stream
/// \param[in] pkg     Package information
///
/// \returns The stream \p stream
///
inline std::ostream& operator <<(_In_ std::ostream& stream, _In_ const Updater::package_info &pkg)
{
    // Write binary version.
    if (stream.fail()) return stream;
    stream.write((const char*)&pkg.ver, sizeof(pkg.ver));

    // Write human readable description (length prefixed).
    std::wstring::size_type char_count = pkg.desc.length();
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    // 4G check
    if (char_count > 0xffffffff) {
        stream.setstate(std::ios_base::failbit);
        return stream;
    }
#endif
    if (stream.fail()) return stream;
    unsigned __int32 count = (unsigned __int32)char_count;
    stream.write((const char*)&count, sizeof(count));
    if (stream.fail()) return stream;
    stream.write((const char*)pkg.desc.c_str(), sizeof(wchar_t)*count);

    // Write package language (length prefixed)
    char_count = pkg.lang.length();
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    // 4G check
    if (char_count > 0xffffffff) {
        stream.setstate(std::ios_base::failbit);
        return stream;
    }
#endif
    if (stream.fail()) return stream;
    count = (unsigned __int32)char_count;
    stream.write((const char*)&count, sizeof(count));
    if (stream.fail()) return stream;
    stream.write(pkg.lang.c_str(), sizeof(char)*count);

    // Write package download URL (length prefixed)
    char_count = pkg.url.length();
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    // 4G check
    if (char_count > 0xffffffff) {
        stream.setstate(std::ios_base::failbit);
        return stream;
    }
#endif
    if (stream.fail()) return stream;
    count = (unsigned __int32)char_count;
    stream.write((const char*)&count, sizeof(count));
    if (stream.fail()) return stream;
    stream.write(pkg.url.c_str(), sizeof(char)*count);

    return stream;
}


///
/// Writes signature to a stream
///
/// \param[in] stream  Output stream
/// \param[in] sig     Signature
///
/// \returns The stream \p stream
///
inline std::ostream& operator <<(_In_ std::ostream& stream, _In_ const Updater::signature &sig)
{
    // Write signature (length prefixed).
    Updater::signature::size_type sig_count = sig.size();
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    // 4G check
    if (sig_count > 0xffffffff) {
        stream.setstate(std::ios_base::failbit);
        return stream;
    }
#endif
    if (stream.fail()) return stream;
    unsigned __int32 count = (unsigned __int32)sig_count;
    stream.write((const char*)&count, sizeof(count));

    // Reverse signature byte order (to make OpenSSL compatible), and write.
    if (stream.fail()) return stream;
    std::vector<unsigned char> sig_swap(count);
    for (unsigned __int32 i = 0, j = count - 1; i < count; i++, j--)
        sig_swap[j] = sig[i];
    stream.write((const char*)sig_swap.data(), sizeof(unsigned char)*count);

    return stream;
}


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
        { wxCMD_LINE_SWITCH, "h"  , "help"   , _("Show this help message"            ), wxCMD_LINE_VAL_NONE  , wxCMD_LINE_OPTION_HELP      },
        { wxCMD_LINE_OPTION, "x"  , "ver-hex", _("Hexadecimal version of the product"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_OPTION, "s"  , "ver-str", _("String version of the product"     ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_OPTION, "l"  , "lang"   , _("Package language"                  ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_OPTION, "u"  , "url"    , _("Package download URL"              ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
        { wxCMD_LINE_PARAM ,  NULL, NULL     , _("output file"                       ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },

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

    const wxString& filenameOut = parser.GetParam(0);
    std::fstream dst((LPCTSTR)filenameOut, std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary);
    if (dst.fail()) {
        _ftprintf(stderr, wxT("%s: error UMD0001: Error opening output file.\n"), filenameOut.fn_str());
        return 1;
    }

    bool has_errors = false;

    // Open file ID.
    std::streamoff dst_start = stdex::idrec::open<Updater::recordid_t, Updater::recordsize_t>(dst, UPDATER_DB_ID);

    Updater::package_info pi;
    wxString val;

    pi.ver = parser.Found(wxT("x"), &val) ? _tcstoul(val, NULL, 16) : 0;

    if (parser.Found(wxT("s"), &val))
        pi.desc = val;

    if (parser.Found(wxT("l"), &val))
        pi.lang = val.ToAscii();

    if (parser.Found(wxT("u"), &val))
        pi.url = val.ToUTF8();

    dst << Updater::package_info_rec(pi);
    if (dst.fail()) {
        _ftprintf(stderr, wxT("%s: error UMD0002: Writing to output file failed.\n"), filenameOut.fn_str());
        has_errors = true;
    }

    stdex::idrec::close<Updater::recordid_t, Updater::recordsize_t, UPDATER_RECORD_ALIGN>(dst, dst_start);

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

    // Hash the file.
    HCRYPTHASH ch = NULL;
    wxVERIFY(::CryptCreateHash(cp, CALG_SHA1, 0, 0, &ch));
    {
        std::vector<BYTE> buf(8192);
        for (dst.seekg(0); !dst.eof();) {
            dst.read((char*)buf.data(), buf.size());
            wxVERIFY(::CryptHashData(ch, buf.data(), dst.gcount(), 0));
        }
        dst.clear();
    }

    // Sign the hash.
    DWORD sig_size = 0;
    if (!::CryptSignHash(ch, AT_SIGNATURE, NULL, 0, NULL, &sig_size)) {
        _ftprintf(stderr, wxT("%s: error UMD0003: Signing output file failed (%i).\n"), filenameOut.fn_str(), ::GetLastError());
        has_errors = true;
    }
    Updater::signature sig(sig_size);
    if (!::CryptSignHash(ch, AT_SIGNATURE, NULL, 0, sig.data(), &sig_size)) {
        _ftprintf(stderr, wxT("%s: error UMD0003: Signing output file failed (%i).\n"), filenameOut.fn_str(), ::GetLastError());
        has_errors = true;
    }
    wxVERIFY(::CryptDestroyHash(ch));
    wxVERIFY(::CryptReleaseContext(cp, 0));

    // Append to the end of the file.
    dst << Updater::signature_rec(sig);

    if (has_errors) {
        dst.close();
        wxRemoveFile(filenameOut);
        return 1;
    } else
        return 0;
}
