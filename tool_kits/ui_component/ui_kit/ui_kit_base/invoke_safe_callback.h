#pragma once
namespace nim_comp
{
	template<typename TCB, typename... TParams>
	void InvokeSafeCallback(const TCB& cb, const TParams&... params)
	{
		if (cb != nullptr)
			cb(params...);
	}
	template<unsigned int index, typename TCB, typename... TParams>
	void InvokeSafeCallback(const TCB& cb, const TParams&... params)
	{
		if (cb != nullptr)			
			std::get<index>(cb)(params...);
	}
	template<typename TCBArray, typename... TParams>
	void InvokeSafeCallbackArray(const TCBArray& cbarray, const TParams&... params)
	{
		for (auto& it : cbarray)
		{
			it(params...);
		}
	}
	template<unsigned int index,typename TCBArray, typename... TParams>
	void InvokeSafeCallbackArray(const TCBArray& cbarray, const TParams&... params)
	{
		for (auto& it : cbarray)
		{		
			std::get<index>(it)(params...); 
		}
	}

}