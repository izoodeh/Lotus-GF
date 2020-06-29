#pragma once

class CNWindow
{
	BOOL m_initialized;
public:
	CNWindow();
	~CNWindow();
	void Init();
};

extern CNWindow g_NWindow;