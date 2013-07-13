#pragma once

class SystemInfoClass
{
public:
	SystemInfoClass(void);
	~SystemInfoClass(void);

	HDEVINFO m_hInfo;

	operator HDEVINFO(void) const {return m_hInfo;}
};

