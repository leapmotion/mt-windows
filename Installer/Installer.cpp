#include "stdafx.h"
#include "Installer.h"
#include "InstallerBase.h"
#include "LastErrorPreserver.h"

eHidStatus PerformOperation(const wchar_t* pInfPath, eHidStatus (CInstallerBase::*pMember)())
{
	// This class is declared first to ensure that its destructor is called last
	CLastErrorPreserver gle;

	// Installer base initialization:
	CInstallerBase base(pInfPath);

	// Initialize the installer base as needed:
	eHidStatus rs = base.Init();

	if(!SUCCEEDED(rs))
		// Failed to initialize, return here
		return gle.CaptureLastError(), rs;

	// Initialize our base member
	rs = (base.*pMember)();

	// Done, capture the last error so it's restored after the CInstallerBase destructor
	// call is made and then return here.
	return gle.CaptureLastError(), rs;
}

eHidStatus OcuHidInstall(const wchar_t* pInfPath)
{
	return PerformOperation(pInfPath, &CInstallerBase::Install);
}

eHidStatus OcuHidUpdate(const wchar_t* pInfPath)
{
	return PerformOperation(pInfPath, &CInstallerBase::Update);
}

eHidStatus OcuHidUninstall(const wchar_t* pInfPath)
{
	return PerformOperation(pInfPath, &CInstallerBase::Uninstall);
}

