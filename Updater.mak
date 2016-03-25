#
#    Copyright 2016 Amebis
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

!IFNDEF UPDATER_CFG
UPDATER_CFG=HKLM\Software\Amebis\ZRCola\Updater
!ENDIF

!IFNDEF UPDATER_OUTPUT_DIR
UPDATER_OUTPUT_DIR=output\update
!ENDIF

!IF "$(PROCESSOR_ARCHITECTURE)" == "AMD64"
UPDATER_REG_FLAGS=/f /reg:64
UPDATER_REG_FLAGS32=/f /reg:32
!ELSE
UPDATER_REG_FLAGS=/f
!ENDIF


######################################################################
# Main targets
######################################################################

UpdaterRegisterSettings ::
	reg.exe add "$(UPDATER_CFG)" /v "CachePath" /t REG_SZ /d "$(MAKEDIR)\$(UPDATER_OUTPUT_DIR)" $(UPDATER_REG_FLAGS) > NUL
!IF "$(PROCESSOR_ARCHITECTURE)" == "AMD64"
	reg.exe add "$(UPDATER_CFG)" /v "CachePath" /t REG_SZ /d "$(MAKEDIR)\$(UPDATER_OUTPUT_DIR)" $(UPDATER_REG_FLAGS32) > NUL
!ENDIF

UpdaterUnregisterSettings ::
	-reg.exe delete "$(UPDATER_CFG)" /v "CachePath" $(UPDATER_REG_FLAGS) > NUL
!IF "$(PROCESSOR_ARCHITECTURE)" == "AMD64"
	-reg.exe delete "$(UPDATER_CFG)" /v "CachePath" $(UPDATER_REG_FLAGS32) > NUL
!ENDIF
