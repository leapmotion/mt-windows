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

	/// <summary>
	/// Construction routine, used to set up internal pointers
	/// </summary>
	eHidStatus Init(void);

private:
  // Set to true if restart is required
  bool m_restartRequired;

	// Set if the INF path is in a directory that PNP cannot install from, such as a network drive.
	bool m_bMustCopy;

	// INF path:
	wstring m_infPath;

private:
	/// <summary>
	/// Utility routine for deleting services
	/// </summary>
	void DeleteOcuHidService(const wchar_t* lpwcsName);

public:
  // Accessor methods:
  bool IsRestartRequired(void) const {return m_restartRequired;}

  // Mutator methods:
	void SetInfPath(const wchar_t* pwcsInfPath);

	/// <summary>
	/// Obtains the version number of the currently installed HID emulator, or zero if one could not be ofund
	/// </summary>
	DWORD GetDriverVersion(void);

	/// <summary>
	/// Installs the HidEmulator nondevice driver
	/// </summary>
	void Install(void);
	
	/// <summary>
	/// Updates the HidEmulator nondevice driver
	/// </summary>
	void Update(void);
	
	/// <summary>
	/// Uninstalls the HidEmulator nondevice driver
	/// </summary>
	void Uninstall(void);
};

