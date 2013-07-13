#pragma once
#include "RestartRequiredTracker.h"

class ServiceControlManager:
  public RestartRequiredTracker
{
public:
  ServiceControlManager(void);
  ~ServiceControlManager(void);

private:
	// Service manager handle:
	SC_HANDLE m_hMngr;

public:
  /// <summary>
  /// Deletes a HIDEmulator driver service
  /// </summary>
  /// <param name="lpwcsName">The driver service name</param>
  void DeleteOcuHidService(const wchar_t* lpwcsName);

  operator SC_HANDLE(void) const {return m_hMngr;}
};

