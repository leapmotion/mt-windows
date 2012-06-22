#pragma once
enum eHidStatus;
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//
#define OCUHID_STATUS_WARNING            0x2
#define OCUHID_STATUS_SUCCESS            0x0
#define OCUHID_STATUS_INFORMATION        0x1
#define OCUHID_STATUS_ERROR              0x3


//
// MessageId: eHidInstSuccess
//
// MessageText:
//
// Installation operation was successful
//
#define eHidInstSuccess                  ((eHidStatus)0x20010000L)

//
// MessageId: eHidInstRestartRequired
//
// MessageText:
//
// Installation operation was successful, but a reboot will be required
//
#define eHidInstRestartRequired          ((eHidStatus)0x60010000L)

//
// MessageId: eHidInstNoDevsToUpdate
//
// MessageText:
//
// The update operation did not update anything because no OcuHid devices were identified
//
#define eHidInstNoDevsToUpdate           ((eHidStatus)0x60010001L)

//
// MessageId: eHidInstServiceConfQueryFail
//
// MessageText:
//
// Failed to query the service configuration in order to determine the binaries that must be deleted.  The service was removed, but its binaries
// may still be present on the system.
//
#define eHidInstServiceConfQueryFail     ((eHidStatus)0xA0010000L)

//
// MessageId: eHidInstRequiresElevation
//
// MessageText:
//
// Installation, upgrade, and uninstallation operations all require Administrator privileges to succeed
//
#define eHidInstRequiresElevation        ((eHidStatus)0xE0010000L)

//
// MessageId: eHidInstNewdevNotFound
//
// MessageText:
//
// Could not find and/or load newdev.dll
//
#define eHidInstNewdevNotFound           ((eHidStatus)0xE0010001L)

//
// MessageId: eHidInstNewdevEntrypointNotFound
//
// MessageText:
//
// Could not find UpdateDriverForPlugAndPlayDevicesW in newdev.dll
//
#define eHidInstNewdevEntrypointNotFound ((eHidStatus)0xE0010002L)

//
// MessageId: eHidInstSysClassNotFound
//
// MessageText:
//
// The built-in SYSTEM device class could not be loaded with a call to SetupDiCreateDeviceInfoListExW
//
#define eHidInstSysClassNotFound         ((eHidStatus)0xE0010003L)

//
// MessageId: eHidInstDevCreateFail
//
// MessageText:
//
// Failed to create an empty device node
//
#define eHidInstDevCreateFail            ((eHidStatus)0xE0010004L)

//
// MessageId: eHidInstDevIDAssignFail
//
// MessageText:
//
// Failed to assign the hardware ID to the null devnode created as part of the install process
//
#define eHidInstDevIDAssignFail          ((eHidStatus)0xE0010005L)

//
// MessageId: eHidInstUserCancel
//
// MessageText:
//
// Failed to install the device due to a user cancellation
//
#define eHidInstUserCancel               ((eHidStatus)0xE0010006L)

//
// MessageId: eHidInstSCManOpenFailed
//
// MessageText:
//
// Failed to open the service control manager for administrative access
//
#define eHidInstSCManOpenFailed          ((eHidStatus)0xE0010007L)

//
// MessageId: eHidInstServiceOpenFailed
//
// MessageText:
//
// Failed to open the OcuHid service itself for deletion
//
#define eHidInstServiceOpenFailed        ((eHidStatus)0xE0010008L)

//
// MessageId: eHidInstServiceDeleteFailed
//
// MessageText:
//
// Failed to delete the service, or mark it for deletion at next reboot
//
#define eHidInstServiceDeleteFailed      ((eHidStatus)0xE0010009L)

//
// MessageId: eHidInstTempPathCreateFail
//
// MessageText:
//
// Failed to create a temporary path for installer files, you might try copying these files manually to a
// local directory and running the install operation again.
//
#define eHidInstTempPathCreateFail       ((eHidStatus)0xE001000AL)

//
// MessageId: eHidInstCompatDriverFindFail
//
// MessageText:
//
// Failed to find a compatible driver for a null device node.
//
#define eHidInstCompatDriverFindFail     ((eHidStatus)0xE001000BL)

//
// MessageId: eHidInstDriverSelectFail
//
// MessageText:
//
// Failed to set the selected driver after the install process was completed
//
#define eHidInstDriverSelectFail         ((eHidStatus)0xE001000CL)

//
// MessageId: eHidInstInfoListBuildFail
//
// MessageText:
//
// Failed to build a list of compatible drivers for the created null device node.
//
#define eHidInstInfoListBuildFail        ((eHidStatus)0xE001000DL)

//
// MessageId: eHidInstCopyOEMFail
//
// MessageText:
//
// Failed to copy the INF into the local machine's INF repository
//
#define eHidInstCopyOEMFail              ((eHidStatus)0xE001000EL)

//
// MessageId: eHidInstInstallSelectionFailed
//
// MessageText:
//
// Failed to install a driver, even after successfully indicating the device where the
// driver was to be placed and the driver to be placed on that device.  This may indicate
// that the driver is not actually compatible with the hardware it's being installed on.
//
#define eHidInstInstallSelectionFailed   ((eHidStatus)0xE001000FL)

//
// MessageId: eHidInstUpdateFailFromNetwork
//
// MessageText:
//
// Cannot install the specified driver; it is on a network share.  Move the driver to
// a local directory and try again.
//
#define eHidInstUpdateFailFromNetwork    ((eHidStatus)0xE0010010L)

