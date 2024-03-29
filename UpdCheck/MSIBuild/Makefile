#    SPDX-License-Identifier: GPL-3.0-or-later
#    Copyright © 1991-2022 Amebis

!INCLUDE "..\..\..\include\MSIBuildCfg.mak"

!IFNDEF MSIBUILD_PLAT_GUID
!ERROR Parameter MSIBUILD_PLAT_GUID is undefined.
!ENDIF

!IFNDEF UPDATER_BIN_DIR
!ERROR Parameter UPDATER_BIN_DIR is undefined.
!ENDIF

MSIBUILD_IS_LOCALIZEABLE=1
!IF "$(LANG)" != "en_US"
MSIBUILD_PO="..\locale\$(LANG).po"
!ENDIF


######################################################################
# Component

All :: "$(LANG).$(PLAT).$(CFG).Component-1.idt"

"$(LANG).$(PLAT).$(CFG).Component-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	move /y << $@ > NUL
Component	ComponentId	Directory_	Attributes	Condition	KeyPath
s$(MSIBUILD_LENGTH_ID)	S38	s$(MSIBUILD_LENGTH_ID)	i2	S255	S$(MSIBUILD_LENGTH_ID)
Component	Component
UpdCheck.exe.$(PLAT)	{263A3D1A-AC7E-4866-929$(MSIBUILD_PLAT_GUID)-B7C1E6FFF93B}	$(UPDATER_BIN_DIR)	$(MSIBUILD_COMPONENT_ATTRIB_FILE)		UpdCheck.exe.$(PLAT)
<<NOKEEP


######################################################################
# Feature

All :: "$(LANG).$(PLAT).$(CFG).Feature-2.idt"

"en_US.$(PLAT).$(CFG).Feature-2.idtx" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	move /y << $@ > NUL
Feature	Feature_Parent	Title	Description	Display	Level	Directory_	Attributes
s$(MSIBUILD_LENGTH_ID)	S$(MSIBUILD_LENGTH_ID)	L64	L255	I2	i2	S$(MSIBUILD_LENGTH_ID)	i2
Feature	Feature
featUpdCheck	$(UPDATER_FEATURE_ROOT)	Automatic Updating	Task for background product updating	1001	2	$(UPDATER_BIN_DIR)	8
<<NOKEEP

"$(LANG).$(PLAT).$(CFG).Feature-2.idt" : "en_US.$(PLAT).$(CFG).Feature-2.idtx" $(MSIBUILD_PO)
	cscript.exe "..\..\..\MSI\MSIBuild\MSI.wsf" //Job:IDTTranslate //Nologo $@ $** /CP:$(MSIBUILD_CODEPAGE)


######################################################################
# FeatureComponents

All :: "$(LANG).$(PLAT).$(CFG).FeatureComponents-1.idt"

"$(LANG).$(PLAT).$(CFG).FeatureComponents-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	move /y << $@ > NUL
Feature_	Component_
s38	s$(MSIBUILD_LENGTH_ID)
FeatureComponents	Feature_	Component_
featUpdCheck	UpdCheck.exe.$(PLAT)
<<NOKEEP


######################################################################
# File

All :: "$(LANG).$(PLAT).$(CFG).File-1.idt"

"$(LANG).$(PLAT).$(CFG).File-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	move /y << $@ > NUL
File	Component_	FileName	FileSize	Version	Language	Attributes	Sequence
s$(MSIBUILD_LENGTH_ID)	s$(MSIBUILD_LENGTH_ID)	l255	i4	S$(MSIBUILD_LENGTH_ID)	S20	I2	i2
File	File
UpdCheck.exe.$(PLAT)	UpdCheck.exe.$(PLAT)	UPDCHECK.EXE|UpdCheck.exe	0		0	1536	1
<<NOKEEP


######################################################################
# ScheduledTask

All :: "$(LANG).$(PLAT).$(CFG).ScheduledTask-2.idt"

"en_US.$(PLAT).$(CFG).ScheduledTask-2.idtx" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	move /y << $@ > NUL
Task	DisplayName	Application	Parameters	Directory_	Flags	Priority	User	Password	Author	Description	IdleMin	IdleDeadline	MaxRuntime	Condition	Component_
s$(MSIBUILD_LENGTH_ID)	l255	s255	S255	s$(MSIBUILD_LENGTH_ID)	i4	i4	S255	S255	L255	L0	I2	I2	i4	S255	s$(MSIBUILD_LENGTH_ID)
ScheduledTask	Task
UpdCheck.$(PLAT)	[SimpleProductName] Update	[#UpdCheck.exe.$(PLAT)]		$(UPDATER_BIN_DIR)	0	32			[Manufacturer]	Regularly checks for program updates, downloads, and installs them			-1		UpdCheck.exe.$(PLAT)
<<NOKEEP

"$(LANG).$(PLAT).$(CFG).ScheduledTask-2.idt" : "en_US.$(PLAT).$(CFG).ScheduledTask-2.idtx" $(MSIBUILD_PO)
	cscript.exe "..\..\..\MSI\MSIBuild\MSI.wsf" //Job:IDTTranslate //Nologo $@ $** /CP:$(MSIBUILD_CODEPAGE)


######################################################################
# TaskTrigger

All :: "$(LANG).$(PLAT).$(CFG).TaskTrigger-1.idt"

"$(LANG).$(PLAT).$(CFG).TaskTrigger-1.idt" : "Makefile" "..\..\..\include\MSIBuildCfg.mak"
	move /y << $@ > NUL
Trigger	BeginDate	EndDate	StartTime	StartTimeRand	MinutesDuration	MinutesInterval	Flags	Type	DaysInterval	WeeksInterval	DaysOfTheWeek	DaysOfMonth	WeekOfMonth	MonthsOfYear	Task_
s$(MSIBUILD_LENGTH_ID)	i2	I2	i2	I2	I4	I4	i4	i2	I2	I2	I2	I4	I2	I2	s$(MSIBUILD_LENGTH_ID)
TaskTrigger	Trigger
UpdCheck.daily.00.$(PLAT)	0		0	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.01.$(PLAT)	0		60	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.02.$(PLAT)	0		120	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.03.$(PLAT)	0		180	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.04.$(PLAT)	0		240	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.05.$(PLAT)	0		300	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.06.$(PLAT)	0		360	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.07.$(PLAT)	0		420	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.08.$(PLAT)	0		480	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.09.$(PLAT)	0		540	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.10.$(PLAT)	0		600	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.11.$(PLAT)	0		660	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.12.$(PLAT)	0		720	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.13.$(PLAT)	0		780	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.14.$(PLAT)	0		840	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.15.$(PLAT)	0		900	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.16.$(PLAT)	0		960	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.17.$(PLAT)	0		1020	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.18.$(PLAT)	0		1080	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.19.$(PLAT)	0		1140	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.20.$(PLAT)	0		1200	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.21.$(PLAT)	0		1260	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.22.$(PLAT)	0		1320	59			0	1	1						UpdCheck.$(PLAT)
UpdCheck.daily.23.$(PLAT)	0		1380	59			0	1	1						UpdCheck.$(PLAT)
<<NOKEEP


######################################################################
# Build MSM module!
######################################################################

!INCLUDE "..\..\..\MSI\MSIBuild\MSM.mak"
