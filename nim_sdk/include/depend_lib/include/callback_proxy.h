/** @file callback_proxy.h
  * @brief C++封装层回调应用层工具类，开发者可以指定自己的异步回调方法,以防止卡死SDK线程(推荐)
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @date 2018/10/22
  */

#ifndef _NIM_CPP_PROXY_H_
#define _NIM_CPP_PROXY_H_
#include <functional>
#include <memory>
namespace nim
{
	class CallbackProxy
	{
		template <typename TR = void>
		using CallbackProxyClosure = std::function<TR(void)>;
	public:
		template <typename TCallback, typename TDoCall>
		static auto  DoSafeCallback(const void* callback, const TDoCall& closure, bool delete_callback = false)
			->decltype(closure((*(TCallback*)(callback))))
		{
			using TCallbackPtr = TCallback * ;
			using TReturn = decltype(closure((*(TCallbackPtr)(callback))));
			struct Deleter { Deleter(TCallbackPtr cb_ptr) : cb_ptr_(cb_ptr) {}~Deleter() { if (cb_ptr_ != nullptr) { delete cb_ptr_; cb_ptr_ = nullptr; } }	private: TCallbackPtr cb_ptr_; };
			if (callback != nullptr)
			{
				auto&& real_type_cb_ptr = (TCallbackPtr)(callback);
				Deleter deleter(delete_callback ? real_type_cb_ptr : nullptr);
				return (*real_type_cb_ptr == nullptr) ? (TReturn()) : (closure(std::forward<TCallback>(*real_type_cb_ptr)));
			}
			return TReturn();
		}
		template<class F, class... Args, class = typename std::enable_if<!std::is_member_function_pointer<F>::value>::type>
		static auto Invoke(F && f, Args && ... args)->decltype(f(std::forward<Args>(args)...))
		{
			using TReturn = decltype(f(std::forward<Args>(args)...));
			return Run<TReturn>(std::bind(f, std::forward<Args>(args)...));
		}
		template<class R, class C, class... DArgs, class P, class... Args>
		static R Invoke(R(C::*f)(DArgs...) const, P && p, Args && ... args)
		{
			using TReturn = R;
			return Run<TReturn>(std::bind(f, p, std::forward<Args>(args)...));
		}
		template<class R, class C, class... DArgs, class P, class... Args>
		static R Invoke(R(C::*f)(DArgs...), P && p, Args && ... args)
		{
			using TReturn = R;
			return Run<TReturn>(std::bind(f, p, std::forward<Args>(args)...));
		}
	protected:
		template<typename TR>
		static TR Run(const CallbackProxyClosure< TR>& task)
		{
			return task();
		}
		template<>
		static void Run(const CallbackProxyClosure< void>& task)
		{
			if (docallback_async_ != nullptr)
			{
				docallback_async_(task);
			}				
			else
			{
				task();
			}			
		}
	public:
		static std::function<void(const std::function< void()>&)> docallback_async_;
	};		
}

#endif