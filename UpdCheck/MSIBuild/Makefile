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

!INCLUDE "..\..\..\MSI\include\MSIBuildCfg.mak"

MSIBUILD_IS_LOCALIZEABLE=1


######################################################################
# Component

All :: "$(LANG).$(PLAT).$(CFG).Component-1.idt"

"$(LANG).$(PLAT).$(CFG).Component-1.idt" : "Makefile" "..\..\..\MSI\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Component	ComponentId	Directory_	Attributes	Condition	KeyPath
s$(MSIBUILD_LENGTH_ID)	S38	s$(MSIBUILD_LENGTH_ID)	i2	S255	S$(MSIBUILD_LENGTH_ID)
Component	Component
!IF "$(PLAT)" == "Win32"
compUpdCheck.exe.Win32	{0E56A84C-1567-4030-B5C8-2FF998E9F7E3}	$(UPDATER_BIN_DIR)	0		fileUpdCheck.exe.Win32
!ENDIF
!IF "$(PLAT)" == "x64"
compUpdCheck.exe.x64	{9245E7EC-EDBD-41E1-858B-88810F3AFDD6}	$(UPDATER_BIN_DIR)	256		fileUpdCheck.exe.x64
!ENDIF
<<NOKEEP


######################################################################
# Feature

All :: "$(LANG).$(PLAT).$(CFG).Feature-2.idt"

"En.$(PLAT).$(CFG).Feature-2.idtx" : "Makefile" "..\..\..\MSI\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Feature	Feature_Parent	Title	Description	Display	Level	Directory_	Attributes
s$(MSIBUILD_LENGTH_ID)	S$(MSIBUILD_LENGTH_ID)	L64	L255	I2	i2	S$(MSIBUILD_LENGTH_ID)	i2
1252	Feature	Feature
featUpdCheck	$(UPDATER_FEATURE_ROOT)	Automatic Updating	Task for background product updating	1001	2	$(UPDATER_BIN_DIR)	8
<<NOKEEP

"De.$(PLAT).$(CFG).Feature-2.idt" : "En.$(PLAT).$(CFG).Feature-2.idtx" "..\locale\de_DE.po"
	rcxgettext.exe idtp $@ $**

"En.$(PLAT).$(CFG).Feature-2.idt" : "En.$(PLAT).$(CFG).Feature-2.idtx"
	copy /y $** $@ > NUL

"It.$(PLAT).$(CFG).Feature-2.idt" : "En.$(PLAT).$(CFG).Feature-2.idtx" "..\locale\it_IT.po"
	rcxgettext.exe idtp $@ $**

"Sl.$(PLAT).$(CFG).Feature-2.idt" : "En.$(PLAT).$(CFG).Feature-2.idtx" "..\locale\sl_SI.po"
	rcxgettext.exe idtp $@ $**


######################################################################
# FeatureComponents

All :: "$(LANG).$(PLAT).$(CFG).FeatureComponents-1.idt"

"$(LANG).$(PLAT).$(CFG).FeatureComponents-1.idt" : "Makefile" "..\..\..\MSI\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Feature_	Component_
s38	s$(MSIBUILD_LENGTH_ID)
FeatureComponents	Feature_	Component_
featUpdCheck	compUpdCheck.exe.$(PLAT)
featUpdCheck	compwxExtend.dll.$(PLAT)
<<NOKEEP


######################################################################
# File

All :: "$(LANG).$(PLAT).$(CFG).File-1.idt"

"$(LANG).$(PLAT).$(CFG).File-1.idt" : "Makefile" "..\..\..\MSI\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
File	Component_	FileName	FileSize	Version	Language	Attributes	Sequence
s$(MSIBUILD_LENGTH_ID)	s$(MSIBUILD_LENGTH_ID)	l255	i4	S$(MSIBUILD_LENGTH_ID)	S20	I2	i2
File	File
fileUpdCheck.exe.$(PLAT)	compUpdCheck.exe.$(PLAT)	UPDCHECK.EXE|UpdCheck.exe	0		0	1536	1
<<NOKEEP


######################################################################
# ScheduledTask

All :: "$(LANG).$(PLAT).$(CFG).ScheduledTask-2.idt"

"En.$(PLAT).$(CFG).ScheduledTask-2.idtx" : "Makefile" "..\..\..\MSI\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Task	DisplayName	Application	Parameters	Directory_	Flags	Priority	User	Password	Author	Description	IdleMin	IdleDeadline	MaxRuntime	Condition	Component_
s$(MSIBUILD_LENGTH_ID)	l255	s255	S255	s$(MSIBUILD_LENGTH_ID)	i4	i4	S255	S255	L255	L0	I2	I2	i4	S255	s$(MSIBUILD_LENGTH_ID)
1252	ScheduledTask	Task
stUpdCheck.$(PLAT)	[SimpleProductName] Update	[#fileUpdCheck.exe.$(PLAT)]		$(UPDATER_BIN_DIR)	0	32			[Manufacturer]	Regularly checks for program updates, downloads, and installs them			-1		compUpdCheck.exe.$(PLAT)
<<NOKEEP

"De.$(PLAT).$(CFG).ScheduledTask-2.idt" : "En.$(PLAT).$(CFG).ScheduledTask-2.idtx" "..\locale\de_DE.po"
	rcxgettext.exe idtp $@ $**

"En.$(PLAT).$(CFG).ScheduledTask-2.idt" : "En.$(PLAT).$(CFG).ScheduledTask-2.idtx"
	copy /y $** $@ > NUL

"It.$(PLAT).$(CFG).ScheduledTask-2.idt" : "En.$(PLAT).$(CFG).ScheduledTask-2.idtx" "..\locale\it_IT.po"
	rcxgettext.exe idtp $@ $**

"Sl.$(PLAT).$(CFG).ScheduledTask-2.idt" : "En.$(PLAT).$(CFG).ScheduledTask-2.idtx" "..\locale\sl_SI.po"
	rcxgettext.exe idtp $@ $**


######################################################################
# TaskTrigger

All :: "$(LANG).$(PLAT).$(CFG).TaskTrigger-1.idt"

"$(LANG).$(PLAT).$(CFG).TaskTrigger-1.idt" : "Makefile" "..\..\..\MSI\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Trigger	BeginDate	EndDate	StartTime	StartTimeRand	MinutesDuration	MinutesInterval	Flags	Type	DaysInterval	WeeksInterval	DaysOfTheWeek	DaysOfMonth	WeekOfMonth	MonthsOfYear	Task_
s$(MSIBUILD_LENGTH_ID)	i2	I2	i2	I2	I4	I4	i4	i2	I2	I2	I2	I4	I2	I2	s$(MSIBUILD_LENGTH_ID)
TaskTrigger	Trigger
ttUpdCheck.daily.$(PLAT)	0		0	59	1440	60	0	1	1						stUpdCheck.$(PLAT)
<<NOKEEP


######################################################################
# Build MSM module!
######################################################################

!INCLUDE "..\..\..\MSI\MSIBuild\MSM.mak"
