#pragma once
#include <functional>
#include <string>

using namespace std;
using namespace std::tr1;

enum eHidStatus;

class CInstallerBase
{
public:
	/// <summary>
	/// Initialization for the initializer base
	/// </summary>
	/// <param name="pInfPath">The path to the HidEmulator INF file.</param>
	CInstallerBase(const wchar_t* pInfPath);
	~CInstallerBase(void);

	// In a production driver, this would actually contain a real PNPID.  In our
	// case, however, this contains a root-enumerated PNP device.
	static const wchar_t sc_pnpID[];
	
	/// <summary>
	/// Construction routine, used to set up internal pointers
	/// </summary>
	eHidStatus Init(void);

private:
	// Device info, for enumeration:
	HDEVINFO hInfo;

	// Service manager handles:
	SC_HANDLE hMngr;

	// Set if the INF path is in a directory that PNP cannot install from, such as a network drive.
	bool m_bMustCopy;

	// INF path:
	wstring m_infPath;

private:
	/// <summary>
	/// Utility routine for deleting services
	/// </summary>
	eHidStatus DeleteOcuHidService(const wchar_t* lpwcsName);

	/// <summary>
	/// Calls the specified function on the first identified HID devnode
	/// </summary>
	eHidStatus ForEach(function<int (SP_DEVINFO_DATA&)> op);

public:
	// Mutator methods:
	void SetInfPath(const wchar_t* pwcsInfPath);

	/// <summary>
	/// Installs the HidEmulator nondevice driver
	/// </summary>
	eHidStatus Install(void);
	
	/// <summary>
	/// Updates the HidEmulator nondevice driver
	/// </summary>
	eHidStatus Update(void);
	
	/// <summary>
	/// Uninstalls the HidEmulator nondevice driver
	/// </summary>
	eHidStatus Uninstall(void);
};

