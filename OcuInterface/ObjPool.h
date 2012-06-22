#pragma once
#include <set>

using std::set;

template<class T>
class CObjPool
{
public:
	~CObjPool(void)
	{
		for each(T* cur in m_pool)
			delete cur;
	}

private:
	// This is the collection of objects that have already been allocated
	set<T*> m_allocated;

	// These are objects that are waiting to be allocated
	set<T*> m_pool;

public:
	/// <summary>
	/// Creates a new object to be used by the caller, or returns an object
	/// from the pool of known objects.
	/// </summary>
	/// <returns>An object of type T</returns>
	T* Create(void)
	{
		T* pRetVal;
		if(m_pool.size())
		{
			auto q = m_pool.begin();
			pRetVal = *q;
			m_pool.erase(q);
		}
		else
			pRetVal = new T;
		m_allocated.insert(pRetVal);
		return pRetVal;
	}

	/// <summary>
	/// Frees the passed object by returning it to the pool
	/// </summary>
	void Free(T* pObj)
	{
		m_allocated.erase(pObj);
		m_pool.insert(pObj);
	}
};

