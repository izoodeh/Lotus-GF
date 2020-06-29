#pragma once

struct ItemDecayData
{
	CItem *pItem;
	time_t decayTimestmap;
};

class CItemDecay : public CFeature
{
	BOOL enabled;
	UINT defaultDecay;
	map<INT32, UINT> mData;
	list<INT32> lNotAffected;
	list<ItemDecayData> lItems;
	void LoadINI();
public:
	CItemDecay();
	void Init();
	~CItemDecay();
	UINT GetItemDecay(INT32 classId);
	BOOL IsAffected(INT32 classId);
	void HandleDroppedItem(CItem *pItem);
	void TimerExpired();
	void OnPickUp(CItem *pItem);
};

extern CItemDecay g_ItemDecay;