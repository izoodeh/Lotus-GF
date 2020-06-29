#pragma once

class CClassExt
{
	BOOL extendNpcClassCount;
	BOOL extendUserSocket;
	BOOL extendUser;
	void ExtendNpcClassCount();
	void ExtendUserSocket();
	void ExtendUser();
public:
	CClassExt(void);
	virtual ~CClassExt(void);
	void Init();
};

extern CClassExt g_ClassExt;