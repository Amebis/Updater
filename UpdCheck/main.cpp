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
/// Reads package information from a stream
///
/// \param[in]  stream  Input stream
/// \param[out] pkg     Package information
///
/// \returns The stream \p stream
///
inline std::istream& operator >>(_In_ std::istream& stream, _Out_ Updater::package_info &pkg)
{
    // Read binary version.
    stream.read((char*)&pkg.ver, sizeof(pkg.ver));
    if (!stream.good()) return stream;

    // Read human readable description (length prefixed).
    unsigned __int32 count;
    stream.read((char*)&count, sizeof(count));
    if (!stream.good()) return stream;
    pkg.desc.resize(count);
    stream.read((char*)pkg.desc.data(), sizeof(wchar_t)*count);

    // Read package language (length prefixed).
    stream.read((char*)&count, sizeof(count));
    if (!stream.good()) return stream;
    pkg.lang.resize(count);
    stream.read((char*)pkg.lang.data(), sizeof(char)*count);

    // Read package download URL (length prefixed).
    stream.read((char*)&count, sizeof(count));
    if (!stream.good()) return stream;
    pkg.url.resize(count);
    stream.read((char*)pkg.url.data(), sizeof(char)*count);

    return stream;
}


///
/// Reads signature from a stream
///
/// \param[in]  stream  Input stream
/// \param[out] sig     Signature
///
/// \returns The stream \p stream
///
inline std::istream& operator >>(_In_ std::istream& stream, _Out_ Updater::signature &sig)
{
    // Read signature (length prefixed).
    unsigned __int32 count;
    stream.read((char*)&count, sizeof(count));
    if (!stream.good()) return stream;

    // Read, and reverse signature byte order (to be OpenSSL compatible).
    std::vector<unsigned char> sig_swap(count);
    stream.read((char*)sig_swap.data(), sizeof(unsigned char)*count);
    if (!stream.good()) return stream;
    sig.resize(count);
    for (unsigned __int32 i = 0, j = count - 1; i < count; i++, j--)
        sig[i] = sig_swap[j];

    return stream;
}




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

    // Open data file.
    std::ifstream dat(_T("..\\..\\output\\test.bin"), std::ios_base::in | std::ios_base::binary);
    if (!dat.good()) {
        wxFAIL_MSG(wxT("Error reading data file."));
        return 1;
    }

    // Find the signature first.
    if (stdex::idrec::find<Updater::recordid_t, Updater::recordsize_t, UPDATER_RECORD_ALIGN>(dat, Updater::signature_rec::id)) {
        // Signature found. Remember file position.
        Updater::recordsize_t
            sig_pos   = (Updater::recordsize_t)dat.tellg(),     // Position to return to to read the signature.
            sig_start = sig_pos - sizeof(Updater::recordid_t);  // Beginning of the signature block

        // Create cryptographic context.
        HCRYPTPROV cp = NULL;
        wxVERIFY(::CryptAcquireContext(&cp, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT));

        // Hash the file up to signature block start.
        HCRYPTHASH ch = NULL;
        wxVERIFY(::CryptCreateHash(cp, CALG_SHA1, 0, 0, &ch));
        {
            dat.seekg(0);
            std::vector<BYTE> buf(8192);
            for (Updater::recordsize_t data_left = sig_start; dat.good() && data_left;) {
                dat.read((char*)buf.data(), std::min<Updater::recordsize_t>(buf.size(), data_left));
                Updater::recordsize_t count = dat.gcount();
                wxVERIFY(::CryptHashData(ch, buf.data(), count, 0));
                data_left -= count;
            }
        }

        // Read the signature.
        dat.seekg(sig_pos);
        Updater::signature sig;
        dat >> Updater::signature_rec(sig);
        if (dat.good()) {
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

                BYTE *key_data = NULL;
                DWORD key_size = 0;
                wxVERIFY(::CryptDecodeObjectEx(X509_ASN_ENCODING, RSA_CSP_PUBLICKEYBLOB, (const BYTE*)keyinfo_data->PublicKey.pbData, keyinfo_data->PublicKey.cbData, CRYPT_DECODE_ALLOC_FLAG, NULL, &key_data, &key_size));

                wxVERIFY(::CryptImportKey(cp, key_data, key_size, NULL, 0, &ck));
                ::LocalFree(key_data);
                ::LocalFree(keyinfo_data);
            }

            // We have the hash, we have the signature, we have the public key. Now verify.
            if (::CryptVerifySignature(ch, (BYTE*)sig.data(), sig.size(), ck, NULL, 0)) {
                // The signature is correct. Now parse the file.
                dat.seekg(0);
                if (stdex::idrec::find<Updater::recordid_t, Updater::recordsize_t, UPDATER_RECORD_ALIGN>(dat, UPDATER_DB_ID, sizeof(Updater::recordid_t))) {
                    Updater::recordsize_t size;
                    dat.read((char*)&size, sizeof(Updater::recordsize_t));
                    if (dat.good()) {
                        if (size > sig_start) {
                            // Limit the size up to the file signature. Should have not get here.
                            wxFAIL_MSG(wxT("Updater file wrong record size."));
                            size = sig_start;
                        }
                        if (stdex::idrec::find<Updater::recordid_t, Updater::recordsize_t, UPDATER_RECORD_ALIGN>(dat, Updater::package_info_rec::id, size)) {
                            // Read package information.
                            Updater::package_info pi;
                            dat >> Updater::package_info_rec(pi);
                            if (dat.good()) {

                            } else
                                wxFAIL_MSG(wxT("Error reading package information from Updater file."));
                        } else
                            wxFAIL_MSG(wxT("Updater file has no package information."));
                    } else
                        wxFAIL_MSG(wxT("Updater file read error."));
                }
            } else
                wxFAIL_MSG(wxT("Updater file signature does not match file content."));

            wxVERIFY(::CryptDestroyKey(ck));
        } else
            wxFAIL_MSG(wxT("Error reading signature from the Updater file."));

        wxVERIFY(::CryptDestroyHash(ch));
        wxVERIFY(::CryptReleaseContext(cp, 0));
    } else
        wxFAIL_MSG(wxT("Signature not found in the Updater file."));

    return 0;
}
