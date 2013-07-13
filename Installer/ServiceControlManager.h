#pragma once
class ServiceControlManager
{
public:
  ServiceControlManager(void);
  ~ServiceControlManager(void);

private:
	// Service manager handle:
	SC_HANDLE m_hMngr;

  // Restart flag, which may be set at any point during ordinary operations
  bool m_restartRequired;

public:
  // Accessor methods:
  bool IsRestartRequired(void) const {return m_restartRequired;}

  /// <summary>
  /// Deletes a HIDEmulator driver service
  /// </summary>
  /// <param name="lpwcsName">The driver service name</param>
  void DeleteOcuHidService(const wchar_t* lpwcsName);

  operator SC_HANDLE(void) const {return m_hMngr;}
};

