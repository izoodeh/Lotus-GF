#include "StdAfx.h"
#include "DBConn.h"
#include <sqlext.h>

CDBConn::CDBConn() : m_Binded(FALSE)
{
	typedef CDBConn* (*f)(CDBConn*, DBType);
	f(0x454660L)(this, DBT_DEFAULT);
}

CDBConn::~CDBConn()
{
	typedef void(*f)(CDBConn*);
	f(0x452A74L)(this);
}

void CDBConn::UnBind()
{
	typedef void (*f)(CDBConn*);
	f(0x45347CL)(this);
	m_Binded = FALSE;
}

void CDBConn::Login()
{
	typedef void (*f)(CDBConn*);
	f(0x454984L)(this);
}

void CDBConn::Init()
{
	typedef void (*f)(CDBConn*);
	f(0x454B3CL)(this);
}

bool CDBConn::Fetch()
{
	typedef bool (*f)(CDBConn*);
	return f(0x452B4CL)(this);
}

void CDBConn::Bind(WCHAR *wParam, INT32 len)
{
	typedef void(*f)(CDBConn*, WCHAR*, int);
	f(0x452F94L)(this, wParam, len);
}

void CDBConn::Bind(CHAR *sParam, INT32 len)
{
	typedef void(*f)(CDBConn*, CHAR*, int);
	f(0x453030L)(this, sParam, len);
}

void CDBConn::Bind(LPUINT lpParam)
{
	typedef void(*f)(CDBConn*, LPUINT);
	f(0x4530D0L)(this, lpParam);
}

void CDBConn::Bind(LPINT lpParam)
{
	typedef void(*f)(CDBConn*, LPINT);
	f(0x453170L)(this, lpParam);
}

void CDBConn::Bind(PINT64 pParam)
{
	typedef void(*f)(CDBConn*, PINT64);
	f(0x453210L)(this, pParam);
}

void CDBConn::Bind(PSHORT pParam)
{
	typedef void(*f)(CDBConn*, PSHORT);
	f(0x4532B0L)(this, pParam);
}

void CDBConn::Bind(DOUBLE* pParam)
{
	typedef void(*f)(CDBConn*, DOUBLE*);
	f(0x453350L)(this, pParam);
}

void CDBConn::ResetParam()
{
	if(m_Binded)
	{
		UnBind();
	}
	typedef void(*f)(CDBConn*);
	f(0x4533F0L)(this);
}

void CDBConn::BindBinary(CHAR *sParam, INT32 len)
{
	typedef void(*f)(CDBConn*, CHAR*, INT32);
	f(0x453518L)(this, sParam, len);
}

void CDBConn::BindBinaryNullable(LPBYTE lpMem, INT32 len, SQLLEN *pSqlLen)
{
	typedef void(*f)(CDBConn*, LPBYTE, INT32, SQLLEN *);
	f(0x4535B4L)(this, lpMem, len, pSqlLen);
}

void CDBConn::BindNullable(WCHAR* wParam, INT32 len, SQLLEN* pSqlLen)
{
	typedef void(*f)(CDBConn*, WCHAR*, INT32, SQLLEN*);
	f(0x453650L)(this, wParam, len, pSqlLen);
}

void CDBConn::BindNullable(LPUINT pParam, SQLLEN* pSqlLen)
{
	typedef void(*f)(CDBConn*, LPUINT, SQLLEN*);
	f(0x4536ECL)(this, pParam, pSqlLen);
}

void CDBConn::BindNullable(LPINT pParam, SQLLEN* pSqlLen)
{
	typedef void(*f)(CDBConn*, LPINT, SQLLEN*);
	f(0x453788L)(this, pParam, pSqlLen);
}

void CDBConn::BindNullable(PINT64 pParam, SQLLEN* pSqlLen)
{
	typedef void(*f)(CDBConn*, PINT64, SQLLEN*);
	f(0x453824L)(this, pParam, pSqlLen);
}

void CDBConn::BindNullable(DOUBLE* pParam, SQLLEN* pSqlLen)
{
	typedef void(*f)(CDBConn*, DOUBLE*, SQLLEN*);
	f(0x4538C0L)(this, pParam, pSqlLen);
}

bool CDBConn::ExecutePrepared(const WCHAR *wQuery)
{
	typedef bool (*f)(CDBConn*, const WCHAR*);
	return f(0x4539BCL)(this, wQuery);
}

void CDBConn::PrepareStatement(const WCHAR* statement)
{
	typedef SQLRETURN (*f)(SQLHSTMT, const WCHAR*, SQLINTEGER);
	f(0x5A555EL)(handle, statement, -3);
}

void CDBConn::BindParameter(INT32 param)
{
	SQLBindParameter(handle, paramCount++, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 4, 0, &param, 0, 0);
}