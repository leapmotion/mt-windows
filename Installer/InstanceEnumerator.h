#pragma once
#include <memory>
#include "SystemInfoClass.h"

class InstanceEnumerator
{
public:
  InstanceEnumerator(std::shared_ptr<SystemInfoClass> hInfo = std::shared_ptr<SystemInfoClass>(new SystemInfoClass(L"root\\system")));
  ~InstanceEnumerator(void);

private:
  // Infoclass, used during enumeration
  std::shared_ptr<SystemInfoClass> m_hInfo;

  // The index, and the last-recovered SP_DEVINFO_DATA
  DWORD m_i;
	SP_DEVINFO_DATA m_info;

public:
  // Accessor methods:
  SP_DEVINFO_DATA& Current(void) {return m_info;}

  /// <summary>
  /// Finds the next matching Leap ID
  /// </summary>
  bool Next(void);

  operator std::shared_ptr<SystemInfoClass>(void) const {return m_hInfo;}
  operator HDEVINFO(void) const {return *m_hInfo;}
};

