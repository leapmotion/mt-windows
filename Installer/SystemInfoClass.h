#pragma once

class __declspec(uuid("4d36e97d-e325-11ce-bfc1-08002be10318")) SystemSetupClass;
class __declspec(uuid("88bae032-5a81-49f0-bc3d-a4ff138216d6")) UsbSetupClass;

class SystemInfoClassBase {
protected:
  SystemInfoClassBase(void);

public:
  virtual ~SystemInfoClassBase(void);
  
protected:
	HDEVINFO m_hInfo;

public:
	operator HDEVINFO(void) const {return m_hInfo;}
};

template<class T>
class SystemInfoClass:
  public SystemInfoClassBase
{
public:
  /// <summary>
  /// Creates an empty infoclass based on the SYSTEM infoclass
  /// </summary>
	SystemInfoClass(void) {
	  // The SYSTEM device class is where the device will be installed
	  m_hInfo = SetupDiCreateDeviceInfoListExW(&__uuidof(T), nullptr, nullptr, nullptr);
	  if(m_hInfo == INVALID_HANDLE_VALUE)
		  throw eHidInstSysClassNotFound;
  }

  /// <summary>
  /// Creates a system device information set based on the passed enumerator
  /// </summary>
  SystemInfoClass(const wchar_t* enumerator) {
	  // Enumerate the root tree to find the one that must be updated
	  m_hInfo = SetupDiGetClassDevsW(&__uuidof(T), L"root\\system", nullptr, 0);
	  if(m_hInfo == INVALID_HANDLE_VALUE)
		  throw eHidInstSysClassNotFound;
  }
};
