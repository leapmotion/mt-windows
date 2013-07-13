#pragma once

class NonPnpDevnode:
	public SP_DEVINFO_DATA
{
public:
	NonPnpDevnode(HDEVINFO hInfo);
	~NonPnpDevnode(void);

private:
	HDEVINFO m_hInfo;
	bool released;

public:
	/// <summary>
	/// Attempts to associate this non-PNP DevNode with the HidEmulator device driver
	/// </summary>
	void Associate(void);

  /// <summary>
  /// Attempts to update the driver associated with this devnode
  /// </remarks>
  /// <returns>True if a reboot is required</returns>
  bool InstallDriver(void);
};

