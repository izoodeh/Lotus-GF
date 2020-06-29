#pragma once

//size 14h    vTable: 613D38h
class MemoryObject
{
public:
/* 08 */	int refCount;
/* 0C */	int inTimer;
/* 10 */	int identity;
/* 14 */	int deleteCount; //?

	virtual ~MemoryObject() = 0;
	virtual MemoryObject* AddRef() = 0;
	virtual void Delete(const char* file, int line) = 0;
	virtual void _NullSub18() = 0;
};

//vTable: 60A1D8h
class CBaseObject : public MemoryObject
{
public:

};