#pragma once

class SystemInfoClass
{
public:
  /// <summary>
  /// Creates an empty infoclass based on the SYSTEM infoclass
  /// </summary>
	SystemInfoClass(void);

  /// <summary>
  /// Creates a system device information set based on the passed enumerator
  /// </summary>
  SystemInfoClass(const wchar_t* enumerator);


	~SystemInfoClass(void);

	HDEVINFO m_hInfo;

	operator HDEVINFO(void) const {return m_hInfo;}
};

