;#pragma once
;enum eHidStatus;

MessageIdTypedef=eHidStatus

SeverityNames=(
		Success			=	0x0		:		OCUHID_STATUS_SUCCESS
		Informational	=	0x1		:		OCUHID_STATUS_INFORMATION
		Warning			=	0x2		:		OCUHID_STATUS_WARNING
		Error			=	0x3		:		OCUHID_STATUS_ERROR
	)

FacilityNames=(
		HidInst			=	0x1
	)

MessageId=0
Severity=Success
Facility=HidInst
SymbolicName=eHidInstSuccess
Language=English
Installation operation was successful
.


MessageId=0
Severity=Informational
Facility=HidInst
SymbolicName=eHidInstRestartRequired
Language=English
Installation operation was successful, but a reboot will be required
.

MessageId=
SymbolicName=eHidInstNoDevsToUpdate
Language=English
The update operation did not update anything because no OcuHid devices were identified
.


MessageId=0
Severity=Warning
SymbolicName=eHidInstServiceConfQueryFail
Language=English
Failed to query the service configuration in order to determine the binaries that must be deleted.  The service was removed, but its binaries
may still be present on the system.
.


MessageId=0
Severity=Error
Facility=HidInst
SymbolicName=eHidInstRequiresElevation
Language=English
Installation, upgrade, and uninstallation operations all require Administrator privileges to succeed
.

MessageId=
SymbolicName=eHidInstNewdevNotFound
Language=English
Could not find and/or load newdev.dll
.

MessageId=
SymbolicName=eHidInstNewdevEntrypointNotFound
Language=English
Could not find UpdateDriverForPlugAndPlayDevicesW in newdev.dll
.

MessageId=
SymbolicName=eHidInstSysClassNotFound
Language=English
The built-in SYSTEM device class could not be loaded with a call to SetupDiCreateDeviceInfoListExW
.

MessageId=
SymbolicName=eHidInstDevCreateFail
Language=English
Failed to create an empty device node
.

MessageId=
SymbolicName=eHidInstDevIDAssignFail
Language=English
Failed to assign the hardware ID to the null devnode created as part of the install process
.

MessageId=
SymbolicName=eHidInstUserCancel
Language=English
Failed to install the device due to a user cancellation
.

MessageId=
SymbolicName=eHidInstSCManOpenFailed
Language=English
Failed to open the service control manager for administrative access
.

MessageId=
SymbolicName=eHidInstServiceOpenFailed
Language=English
Failed to open the OcuHid service itself for deletion
.

MessageId=
SymbolicName=eHidInstServiceDeleteFailed
Language=English
Failed to delete the service, or mark it for deletion at next reboot
.

MessageId=
SymbolicName=eHidInstTempPathCreateFail
Language=English
Failed to create a temporary path for installer files, you might try copying these files manually to a
local directory and running the install operation again.
.

MessageId=
SymbolicName=eHidInstCompatDriverFindFail
Language=English
Failed to find a compatible driver for a null device node.
.

MessageId=
SymbolicName=eHidInstDriverSelectFail
Language=English
Failed to set the selected driver after the install process was completed
.

MessageId=
SymbolicName=eHidInstInfoListBuildFail
Language=English
Failed to build a list of compatible drivers for the created null device node.
.

MessageId=
SymbolicName=eHidInstCopyOEMFail
Language=English
Failed to copy the INF into the local machine's INF repository, the INF file appears
to be missing.
.

MessageId=
SymbolicName=eHidInstINFDependencyMissing
Language=English
The INF file appears to be present, but one or more of its dependencies is missing.
Cannot copy the INF unless all of its dependencies are present.  Check the inf's CAT
file to verify that all files are present and available.
.

MessageId=
SymbolicName=eHidInstInstallSelectionFailed
Language=English
Failed to install a driver, even after successfully indicating the device where the
driver was to be placed and the driver to be placed on that device.  This may indicate
that the driver is not actually compatible with the hardware it's being installed on.
.

MessageId=
SymbolicName=eHidInstUpdateFailFromNetwork
Language=English
Cannot install the specified driver; it is on a network share.  Move the driver to
a local directory and try again.
.

MessageId=
SymbolicName=eHidInstFailedToSelectDevice
Language=English
Failed to select a located device driver to a created HidEmulator devnode
.

MessageId=
SymbolicName=eHidInstDeviceRegistrationFailed
Language=English
Failed to register a device after successfully creating it
.

MessageId=
SymbolicName=eHidInstDriverInfOpenFailed
Language=English
Failed to open the driver INF file, it could be corrupted or damaged
.

MessageId=
SymbolicName=eHidInstInfInformationUnavailable
Language=English
Failed to obtain information from the driver INF
.

MessageId=
SymbolicName=eHidInstInfCannotEnumerate
Language=English
Failed to enumerate the list of already-installed INFs
.

MessageId=
SymbolicName=eHidInstInfApparentlyNotLeap
Language=English
INF file requested for installation is apparently not a Leap INF
.
