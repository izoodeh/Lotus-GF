#pragma once

class CPrivateStoreAction	//: public CNewAction
{
public:
	virtual void Destructor() = 0;
	virtual UINT GetActionType() = 0;
	virtual bool FalseSub1() = 0;
	virtual bool OnStart() = 0;
	virtual bool FalseSub2() = 0;
	virtual bool TrueSub1() = 0;
	virtual bool TrueSub2() = 0;
	virtual bool TrueSub3() = 0;
	virtual bool TrueSub4() = 0;
	virtual void OnEnd() = 0;
	virtual void TrueSub5() = 0;
	virtual void AddTimer(int id) = 0;

	UINT m_ownerId;
};