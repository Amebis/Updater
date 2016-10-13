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

MSIBUILD_IS_LOCALIZEABLE=1


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
compUpdCheck.exe.Win32	{0E56A84C-1567-4030-B5C8-2FF998E9F7E3}	$(UPDATER_BIN_DIR)	0		fileUpdCheck.exe.Win32
!ENDIF
!IF "$(PLAT)" == "x64"
compUpdCheck.exe.x64	{9245E7EC-EDBD-41E1-858B-88810F3AFDD6}	$(UPDATER_BIN_DIR)	256		fileUpdCheck.exe.x64
!ENDIF
<<NOKEEP


######################################################################
# Feature

All :: "$(LANG).$(PLAT).$(CFG).Feature-2.idt"

"en_US.$(PLAT).$(CFG).Feature-2.idtx" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Feature	Feature_Parent	Title	Description	Display	Level	Directory_	Attributes
s$(MSIBUILD_LENGTH_ID)	S$(MSIBUILD_LENGTH_ID)	L64	L255	I2	i2	S$(MSIBUILD_LENGTH_ID)	i2
1252	Feature	Feature
featUpdCheck	$(UPDATER_FEATURE_ROOT)	Automatic Updating	Task for background product updating	1001	2	$(UPDATER_BIN_DIR)	8
<<NOKEEP

!IF "$(LANG)" == "en_US"
"en_US.$(PLAT).$(CFG).Feature-2.idt" : "en_US.$(PLAT).$(CFG).Feature-2.idtx"
	copy /y $** $@ > NUL
!ELSE
"$(LANG).$(PLAT).$(CFG).Feature-2.idt" : "en_US.$(PLAT).$(CFG).Feature-2.idtx" "..\locale\$(LANG).po"
	cscript.exe "..\..\..\MSI\MSIBuild\MSI.wsf" //Job:IDTTranslate //Nologo $@ $** /CP:$(MSIBUILD_CODEPAGE)
!ENDIF


######################################################################
# FeatureComponents

All :: "$(LANG).$(PLAT).$(CFG).FeatureComponents-1.idt"

"$(LANG).$(PLAT).$(CFG).FeatureComponents-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Feature_	Component_
s38	s$(MSIBUILD_LENGTH_ID)
FeatureComponents	Feature_	Component_
featUpdCheck	compUpdCheck.exe.$(PLAT)
featUpdCheck	compwxExtend.dll.$(PLAT)
featUpdCheck	compUpdater.dll.$(PLAT)
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
fileUpdCheck.exe.$(PLAT)	compUpdCheck.exe.$(PLAT)	UPDCHECK.EXE|UpdCheck.exe	0		0	1536	1
<<NOKEEP


######################################################################
# ScheduledTask

All :: "$(LANG).$(PLAT).$(CFG).ScheduledTask-2.idt"

"en_US.$(PLAT).$(CFG).ScheduledTask-2.idtx" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Task	DisplayName	Application	Parameters	Directory_	Flags	Priority	User	Password	Author	Description	IdleMin	IdleDeadline	MaxRuntime	Condition	Component_
s$(MSIBUILD_LENGTH_ID)	l255	s255	S255	s$(MSIBUILD_LENGTH_ID)	i4	i4	S255	S255	L255	L0	I2	I2	i4	S255	s$(MSIBUILD_LENGTH_ID)
1252	ScheduledTask	Task
stUpdCheck.$(PLAT)	[SimpleProductName] Update	[#fileUpdCheck.exe.$(PLAT)]		$(UPDATER_BIN_DIR)	0	32			[Manufacturer]	Regularly checks for program updates, downloads, and installs them			-1		compUpdCheck.exe.$(PLAT)
<<NOKEEP

!IF "$(LANG)" == "en_US"
"en_US.$(PLAT).$(CFG).ScheduledTask-2.idt" : "en_US.$(PLAT).$(CFG).ScheduledTask-2.idtx"
	copy /y $** $@ > NUL
!ELSE
"$(LANG).$(PLAT).$(CFG).ScheduledTask-2.idt" : "en_US.$(PLAT).$(CFG).ScheduledTask-2.idtx" "..\locale\$(LANG).po"
	cscript.exe "..\..\..\MSI\MSIBuild\MSI.wsf" //Job:IDTTranslate //Nologo $@ $** /CP:$(MSIBUILD_CODEPAGE)
!ENDIF


######################################################################
# TaskTrigger

All :: "$(LANG).$(PLAT).$(CFG).TaskTrigger-1.idt"

"$(LANG).$(PLAT).$(CFG).TaskTrigger-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	-if exist $@ del /f /q $@
	move /y << $@ > NUL
Trigger	BeginDate	EndDate	StartTime	StartTimeRand	MinutesDuration	MinutesInterval	Flags	Type	DaysInterval	WeeksInterval	DaysOfTheWeek	DaysOfMonth	WeekOfMonth	MonthsOfYear	Task_
s$(MSIBUILD_LENGTH_ID)	i2	I2	i2	I2	I4	I4	i4	i2	I2	I2	I2	I4	I2	I2	s$(MSIBUILD_LENGTH_ID)
TaskTrigger	Trigger
ttUpdCheck.daily.00.$(PLAT)	0		0	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.01.$(PLAT)	0		60	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.02.$(PLAT)	0		120	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.03.$(PLAT)	0		180	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.04.$(PLAT)	0		240	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.05.$(PLAT)	0		300	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.06.$(PLAT)	0		360	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.07.$(PLAT)	0		420	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.08.$(PLAT)	0		480	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.09.$(PLAT)	0		540	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.10.$(PLAT)	0		600	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.11.$(PLAT)	0		660	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.12.$(PLAT)	0		720	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.13.$(PLAT)	0		780	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.14.$(PLAT)	0		840	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.15.$(PLAT)	0		900	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.16.$(PLAT)	0		960	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.17.$(PLAT)	0		1020	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.18.$(PLAT)	0		1080	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.19.$(PLAT)	0		1140	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.20.$(PLAT)	0		1200	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.21.$(PLAT)	0		1260	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.22.$(PLAT)	0		1320	59			0	1	1						stUpdCheck.$(PLAT)
ttUpdCheck.daily.23.$(PLAT)	0		1380	59			0	1	1						stUpdCheck.$(PLAT)
<<NOKEEP


######################################################################
# Build MSM module!
######################################################################

!INCLUDE "..\..\..\MSI\MSIBuild\MSM.mak"
