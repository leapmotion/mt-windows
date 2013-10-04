#pragma once
#include <memory>

class SystemInfoClassBase;

class NonPnpDevnode:
	public SP_DEVINFO_DATA
{
public:
	NonPnpDevnode(void);
	NonPnpDevnode(std::shared_ptr<SystemInfoClassBase> hInfo);
  NonPnpDevnode(std::shared_ptr<SystemInfoClassBase> hInfo, const SP_DEVINFO_DATA& data);
	~NonPnpDevnode(void);

private:
	std::shared_ptr<SystemInfoClassBase> m_hInfo;
	bool released;

public:
	/// <summary>
	/// Attempts to associate this non-PNP DevNode with the HidEmulator device driver
	/// </summary>
	void Associate(void);

  /// <summary>
  /// Prevents the destructor from attempting to delete this devnode
  /// </summary>
  void Release(void) {
    released = true;
  }

  void operator=(NonPnpDevnode&& rhs) {
    *(PSP_DEVINFO_DATA)this = rhs;
    m_hInfo = rhs.m_hInfo;
    released = rhs.released;
    rhs.released = true;
  }
};

