#
#    Copyright 2016-2020 Amebis
#
#    This file is part of Updater.
#
#    Updater is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Updater is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Updater. If not, see <http://www.gnu.org/licenses/>.
#

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

"$(KEY_DIR)" :
	if not exist $@ md $@


######################################################################
# Building
######################################################################

GenRSAKeypair :: \
	"$(KEY_DIR)" \
	"$(KEY_DIR)\UpdaterKeyPrivate.bin" \
	"$(KEY_DIR)\UpdaterKeyPublic.bin"

"$(KEY_DIR)\UpdaterKeypair.txt" :
	openssl.exe genrsa -out $@ 4096

"$(KEY_DIR)\UpdaterKeyPrivate.bin" : "$(KEY_DIR)\UpdaterKeypair.txt"
	if exist $@ del /f /q $@
	if exist "$(@:"=).tmp" del /f /q "$(@:"=).tmp"
	openssl.exe rsa -in $** -inform PEM -outform DER -out "$(@:"=).tmp"
	move /y "$(@:"=).tmp" $@ > NUL

"$(KEY_DIR)\UpdaterKeyPublic.bin" : "$(KEY_DIR)\UpdaterKeypair.txt"
	if exist $@ del /f /q $@
	if exist "$(@:"=).tmp" del /f /q "$(@:"=).tmp"
	openssl.exe rsa -in $** -inform PEM -outform DER -out "$(@:"=).tmp" -pubout
	move /y "$(@:"=).tmp" $@ > NUL

