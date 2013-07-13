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
  const SP_DEVINFO_DATA& Current(void) const {return m_info;}

  /// <summary>
  /// Finds the next matching Leap ID
  /// </summary>
  bool Next(void);
};

