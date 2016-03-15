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

