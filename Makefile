#    SPDX-License-Identifier: GPL-3.0-or-later
#    Copyright © 2016-2022 Amebis

KEY_DIR=..\include


######################################################################
# Main targets
######################################################################

All :: \
	GenRSAKeypair

Clean ::
# Ommited intentionally not to delete keys accidentaly.


######################################################################
# Folder creation
######################################################################

"$(APPDATA)\Updater" \
"$(KEY_DIR)" :
	if not exist $@ md $@


######################################################################
# Building
######################################################################

GenRSAKeypair :: \
	"$(APPDATA)\Updater" \
	"$(KEY_DIR)" \
	"$(KEY_DIR)\UpdaterKeyPrivate.bin" \
	"$(KEY_DIR)\UpdaterKeyPublic.bin"

"$(APPDATA)\Updater\Keypair.txt" :
	openssl.exe genrsa -out $@ 4096

"$(KEY_DIR)\UpdaterKeyPrivate.bin" : "$(APPDATA)\Updater\Keypair.txt"
	if exist $@ del /f /q $@
	if exist "$(@:"=).tmp" del /f /q "$(@:"=).tmp"
	openssl.exe rsa -in $** -inform PEM -outform DER -out "$(@:"=).tmp"
	move /y "$(@:"=).tmp" $@ > NUL

"$(KEY_DIR)\UpdaterKeyPublic.bin" : "$(APPDATA)\Updater\Keypair.txt"
	if exist $@ del /f /q $@
	if exist "$(@:"=).tmp" del /f /q "$(@:"=).tmp"
	openssl.exe rsa -in $** -inform PEM -outform DER -out "$(@:"=).tmp" -pubout
	move /y "$(@:"=).tmp" $@ > NUL

