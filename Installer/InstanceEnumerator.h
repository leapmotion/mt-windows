#pragma once
#include <memory>
#include "RestartRequiredTracker.h"
#include "SystemInfoClass.h"

class InstanceEnumerator:
  public RestartRequiredTracker
{
public:
  InstanceEnumerator(std::shared_ptr<SystemInfoClassBase> hInfo = std::shared_ptr<SystemInfoClassBase>(new SystemInfoClass<SystemSetupClass>(L"root\\system")));
  ~InstanceEnumerator(void);

private:
  // Infoclass, used during enumeration
  std::shared_ptr<SystemInfoClassBase> m_hInfo;

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

  /// <summary>
  /// Destroyes the currently enumerated device
  /// </summary>
  void DestroyCurrent(void);

  operator std::shared_ptr<SystemInfoClassBase>(void) const {return m_hInfo;}
  operator HDEVINFO(void) const {return *m_hInfo;}
};

