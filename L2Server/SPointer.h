#pragma once
#include "MemoryObject.h"

#include <algorithm>

template <typename T>
class CSPointer
{
	MemoryObject* pObject;
	int	nWorkType;
public:

	explicit CSPointer(T* pObject = 0, int nWorkType = 0)
	{
		initialize(pObject, nWorkType);
	}

	CSPointer(const CSPointer<T>& other)
	{
		initialize(other.get(), other.getWorkType());
	}

	template <typename U>
	CSPointer(const CSPointer<U>& other)
	{
		initialize(other.get(), other.getWorkType());
	}

	~CSPointer()
	{
		if (this->pObject)
		{
			this->pObject->Release(__FILE__, __LINE__, nWorkType, false);
			this->pObject = 0;
		}
	}

	T* get() const
	{
		return static_cast<T*>(pObject);
	}

	int getWorkType() const
	{
		return nWorkType;
	}

	void swap(CSPointer<T>& other)
	{
		std::swap(this->pObject, other.pObject);
		std::swap(this->nWorkType, other.nWorkType);
	}

	CSPointer<T>& operator= (const CSPointer<T>& other)
	{
		swap(CSPointer<T>(other));
		return *this;
	}

	template <typename U>
	CSPointer<T>& operator= (const CSPointer<U>& other)
	{
		swap(CSPointer<T>(other));
		return *this;
	}

	T& operator* () const
	{
		if (get() == 0)
		{
			g_Log.Add(CLog::Error, "[%s][%d] referencing NULL object", __FILE__, __LINE__);
			g_Log.CallStack();
		}
		return *get();
	}

	T* operator-> () const
	{
		if (get() == 0)
		{
			g_Log.Add(CLog::Error, "[%s][%d] referencing NULL object", __FILE__, __LINE__);
			g_Log.CallStack();
		}
		return get();
	}

	operator T* () const
	{
		return get();
	}

private:

	void initialize(T* pObject, int nWorkType)
	{
		if (pObject == 0)
		{
			this->pObject = 0;
			this->nWorkType = nWorkType;
		}
		else
		{
			this->pObject = pObject->AddRef(__FILE__, __LINE__, nWorkType);
			this->nWorkType = nWorkType;
		}
	}
};

typedef CSPointer<CCreature> CCreatureSP;
typedef CSPointer<User> CUserSP;
typedef CSPointer<CPledge> CPledgeSP;
typedef CSPointer<CEventMatch> CEventMatchSP;
