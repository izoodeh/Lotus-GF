#pragma once
#include <sql.h>

enum DBType
{
	DBT_DEFAULT = 0
};

//size 420
class CDBConn
{
	/* 000 */ SQLHSTMT handle;
	/* 008 */ DBType dbType;
	/* 00C */ INT32 _unknC;
	/* 010 */ LPVOID _unkn10;
	/* 018 */ WORD _unkn18;
	/* 01A */ WORD paramCount;
	/* 01C */ BYTE statementBuff[0x400];
	/* 41C */ BOOL _unkn41C;
	/* 420 */ BOOL m_Binded;
public:
	CDBConn();
	~CDBConn();
	void UnBind();
	void Login();
	void Init();
	bool Fetch();
	void Bind(WCHAR* wParam, INT32 len);
	void Bind(CHAR* sParam, INT32 len);
	void Bind(LPUINT lpParam);
	void Bind(LPINT lpParam);
	void Bind(PINT64 pParam);
	void Bind(PSHORT pParam);
	void Bind(DOUBLE* pParam);
	void ResetParam();
	void BindBinary(CHAR* sParam, INT32 len);
	void BindBinaryNullable(LPBYTE lpMem, INT32 len, SQLLEN* pSqlLen);
	void BindNullable(WCHAR* wParam, INT32 len, SQLLEN* pSqlLen);
	void BindNullable(LPUINT pParam, SQLLEN* pSqlLen);
	void BindNullable(LPINT pParam, SQLLEN* pSqlLen);
	void BindNullable(PINT64 pParam, SQLLEN* pSqlLen);
	void BindNullable(DOUBLE* pParam, SQLLEN* pSqlLen);
	bool ExecutePrepared(const WCHAR* wQuery);
	void PrepareStatement(const WCHAR* statement);
	void BindParameter(INT32 param);
};