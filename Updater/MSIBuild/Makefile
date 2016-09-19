#
#    Copyright 1991-2016 Amebis
#
#    This file is part of ZRCola.
#
#    ZRCola is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    ZRCola is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with ZRCola. If not, see <http://www.gnu.org/licenses/>.
#

!INCLUDE "..\..\..\include\MSIBuildCfg.mak"

!IFNDEF UPDATER_BIN_DIR
!ERROR Parameter UPDATER_BIN_DIR is undefined.
!ENDIF

!IFNDEF UPDATER_LOC_DIR
!ERROR Parameter UPDATER_LOC_DIR is undefined.
!ENDIF


######################################################################
# Component

All :: "$(LANG).$(PLAT).$(CFG).Component-1.idt"

"$(LANG).$(PLAT).$(CFG).Component-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Component	ComponentId	Directory_	Attributes	Condition	KeyPath
s$(MSIBUILD_LENGTH_ID)	S38	s$(MSIBUILD_LENGTH_ID)	i2	S255	S$(MSIBUILD_LENGTH_ID)
Component	Component
!IF "$(PLAT)" == "Win32"
compUpdater.dll.Win32	{F4962976-71C3-4200-B736-BC841FEE29C2}	$(UPDATER_BIN_DIR)	0		fileUpdater.dll.Win32
!ENDIF
!IF "$(PLAT)" == "x64"
compUpdater.dll.x64	{A968C387-EC14-4CAC-8406-F21C9779D3EF}	$(UPDATER_BIN_DIR)	256		fileUpdater.dll.x64
!ENDIF
!IF "$(LANG)" == "ru_RU"
compUpdater.mo.ru_RU	{486A9DDF-A43B-4920-BEF9-2907DD8660AD}	UPDATERLOCRURUDIR	$(MSIBUILD_COMPONENT_ATTRIB_FILE)		fileUpdater.mo.ru_RU
!ENDIF
!IF "$(LANG)" == "sl_SI"
compUpdater.mo.sl_SI	{2A56FA39-F4E6-492F-A863-AFE9E53FA988}	UPDATERLOCSLSIDIR	$(MSIBUILD_COMPONENT_ATTRIB_FILE)		fileUpdater.mo.sl_SI
!ENDIF
<<NOKEEP


######################################################################
# Directory

All :: "$(LANG).$(PLAT).$(CFG).Directory-1.idt"

"$(LANG).$(PLAT).$(CFG).Directory-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Directory	Directory_Parent	DefaultDir
s$(MSIBUILD_LENGTH_ID)	S$(MSIBUILD_LENGTH_ID)	l255
Directory	Directory
!IF "$(LANG)" == "ru_RU"
UPDATERLOCRURUDIR	$(UPDATER_LOC_DIR)	ru_RU
!ENDIF
!IF "$(LANG)" == "sl_SI"
UPDATERLOCSLSIDIR	$(UPDATER_LOC_DIR)	sl_SI
!ENDIF
<<NOKEEP


######################################################################
# File

All :: "$(LANG).$(PLAT).$(CFG).File-1.idt"

"$(LANG).$(PLAT).$(CFG).File-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
File	Component_	FileName	FileSize	Version	Language	Attributes	Sequence
s$(MSIBUILD_LENGTH_ID)	s$(MSIBUILD_LENGTH_ID)	l255	i4	S$(MSIBUILD_LENGTH_ID)	S20	I2	i2
File	File
!IF "$(PLAT)" == "Win32"
!IF "$(CFG)" == "Release"
fileUpdater.dll.Win32	compUpdater.dll.Win32	UPDATE~2.DLL|Updater10u_vc100.dll	0		0	1536	1
!ENDIF
!IF "$(CFG)" == "Debug"
fileUpdater.dll.Win32	compUpdater.dll.Win32	UPDATE~4.DLL|Updater10ud_vc100.dll	0		0	1536	1
!ENDIF
!ENDIF
!IF "$(PLAT)" == "x64"
!IF "$(CFG)" == "Release"
fileUpdater.dll.x64	compUpdater.dll.x64	UPDATE~6.DLL|Updater10u_vc100_x64.dll	0		0	1536	1
!ENDIF
!IF "$(CFG)" == "Debug"
fileUpdater.dll.x64	compUpdater.dll.x64	UPDATE~8.DLL|Updater10ud_vc100_x64.dll	0		0	1536	1
!ENDIF
!ENDIF
!IF "$(LANG)" == "ru_RU"
fileUpdater.mo.ru_RU	compUpdater.mo.ru_RU	UPDATE~1.MO|Updater10.mo	0		1060	0	1
!ENDIF
!IF "$(LANG)" == "sl_SI"
fileUpdater.mo.sl_SI	compUpdater.mo.sl_SI	UPDATE~1.MO|Updater10.mo	0		1060	0	1
!ENDIF
<<NOKEEP


######################################################################
# Build MSM module!
######################################################################

!INCLUDE "..\..\..\MSI\MSIBuild\MSM.mak"
