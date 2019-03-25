#pragma once 

template<class T>
class CSingleton
{
protected:
	static T* Instance;
public:
	virtual ~CSingleton() {};

	inline static T* GetInstance()
	{
		if(!Instance)Instance = new T();
		return Instance;
	}

	static void DeleteInstance()
	{
		if(Instance)
		{
			delete Instance;
			Instance = NULL;
		}
	}
};

template<class T>
T* CSingleton<T>::Instance = NULL;

