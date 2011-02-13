#pragma once

template <class Type>
class Singleton
{
    static Type* instance;

public:
	Singleton()
	{
		assert(!instance);
		INT_PTR offset = (INT_PTR)(Type*)1 - (INT_PTR)(Singleton< Type >*)(Type*)1;
		instance = (Type*)((INT_PTR)this + offset);
	}

	~Singleton()
	{
	   assert(instance);
	   instance = 0;
	}

	static Type& Instance()
	{
	   assert( instance );
	   return *instance;
	}

	static Type* InstancePtr()
    {
		return instance;
	}
};

template <class Type> Type* Singleton< Type >::instance = NULL;