#ifndef  _SDK_BASE_BASE_THREADEX_H_
#define _SDK_BASE_BASE_THREADEX_H_

#include <list>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <queue>
#include <chrono>

#include "task_loop_interface.h"
#include "base/thread/thread.h"
namespace nbase
{

	class BaseThread : public nbase::Thread //virtual public ITaskLoop
	{
	public:
		class DelayTask
		{
		public:
			DelayTask(int time_block, const Task& task) :
				time_block_(time_block),
				run_time_point_(std::chrono::high_resolution_clock::now()),
				task_(task) {
			}
			inline bool NeedDoTask() {
				std::chrono::duration<double, std::milli> tt = std::chrono::high_resolution_clock::now() - run_time_point_;
				return tt.count() >= time_block_;
			}
			inline void Run()
			{
				task_();
			}
		private:
			const int time_block_;
			const std::chrono::high_resolution_clock::time_point run_time_point_;
			Task task_;
		};
	private:
		class STLThread : public nbase::Thread
		{
		public:
			STLThread(const std::function<void()>& thread_proc, const std::string& thread_name = "ThreadEx worker") 
				:func(thread_proc) 
				,thread_name_(thread_name)
			{}
			virtual void Run() override{ 
				SetThreadName(GetCurrentThreadId(), thread_name_.c_str());
				func(); 
			}
// 			void Join()
// 			{
// 				if (thread_.joinable())
// 					thread_.join();
// 			}
		private:
			std::function<void()> func;
			std::string thread_name_;
			//std::thread thread_;
		};
	public:
		BaseThread() :
			begin_internal_callback_(nullptr),
			end_internal_callback_(nullptr),
			begin_callback_(nullptr),
			end_callback_(nullptr),
			exit_(false),
			running_(false),
			stl_thread_(nullptr)
		{	};
		virtual ~BaseThread() {
			if (running_ && stl_thread_ != nullptr)
			{
				Stop();
				//stl_thread_->Join();
				stl_thread_->Close();
				stl_thread_.reset();
			}
		};
	public:
		bool IsRunning() const
		{
			return running_;
		}
		void AttachBegin(const RuntimeCallback& begin_callback)
		{
			begin_callback_ = begin_callback;
		}
		void AttachEnd(const RuntimeCallback& end_callback)
		{
			end_callback_ = end_callback;
		}
		void PostTask(const Task& task)
		{
			std::lock_guard<std::recursive_mutex> auto_lock(mut_task_list_);
			catch_task_list_.push(task);
			if (task_list_.empty())
				task_list_.swap(catch_task_list_);
			std::unique_lock<std::recursive_mutex> lk(mut_loop_);
			cv_loop.notify_all();
		}
		void PostHighPriorityTask(const Task& task)
		{
			std::lock_guard<std::recursive_mutex> auto_lock(mut_high_priority_task_list_);
			catch_high_priority_task_list_.push(task);
			if (high_priority_task_list_.empty())
				high_priority_task_list_.swap(catch_high_priority_task_list_);
			std::unique_lock<std::recursive_mutex> lk(mut_loop_);
			cv_loop.notify_all();
		}
		void PostDelayTask(int time_block, const Task& task)
		{
			std::lock_guard<std::recursive_mutex> auto_lock(mut_delay_task_list_);
			catch_delay_task_list_.emplace_back(DelayTask(time_block, task));
			if (delay_task_list_.empty())
				delay_task_list_.swap(catch_delay_task_list_);
		}
		void Start()
		{
			stl_thread_ = std::make_unique<STLThread>([this]() {
				DoMain();
			});
			stl_thread_->Create();
			if (!running_)
			{
				std::unique_lock<std::mutex> wait_lock(run_signal_lock_);
				if (!running_)
					run_signal_cond_var_.wait(wait_lock);
			}

		}
		void Stop()
		{
			DoStop();
			Join();
		}
		void Join()
		{
			DoJoin();
		}
		void AttachCurrentThread()
		{
			DoMain();
		}
	protected:
		void AttachInternalBegin(const RuntimeCallback& begin_callback)
		{
			begin_internal_callback_ = begin_callback;
		}
		void AttachInternalEnd(const RuntimeCallback& end_callback)
		{
			end_internal_callback_ = end_callback;
		}
	protected:
		virtual void DoStop()
		{
			exit_ = true;
			std::unique_lock<std::recursive_mutex> lk(mut_loop_);
			cv_loop.notify_all();
		}
		virtual void DoJoin()
		{
			if (stl_thread_ != nullptr)
			{
				//stl_thread_->Join();
				stl_thread_->Close();
			}
		}
		virtual void DoMain()
		{
			{
				std::unique_lock<std::mutex> lock(run_signal_lock_);
				if (begin_internal_callback_ != nullptr)
					begin_internal_callback_();
				if (begin_callback_ != nullptr)
					begin_callback_();
				running_ = true;
				run_signal_cond_var_.notify_all();
			}
			OnTaskLoop();
			running_ = false;
			if (end_callback_)
				end_callback_();
			if (end_internal_callback_ != nullptr)
				end_internal_callback_();
		}
	protected:
		virtual void OnTaskLoop()
		{
			constexpr int TIMER_RESOLUTION = 10;
			constexpr std::chrono::milliseconds BLOCK(TIMER_RESOLUTION);
			while (!exit_)
			{
				std::cv_status ret = std::cv_status::no_timeout;
				{
					std::unique_lock<std::recursive_mutex> lk(mut_loop_);
					ret = cv_loop.wait_for(lk, BLOCK);
				}
				if (exit_)
					break;
				if (ret == std::cv_status::no_timeout)
				{
					RunTasks();
				}
				else
				{
					RunDelayTasks();
				}
			}
		}
		void RunDelayTasks(bool check_catch = true)
		{
			RunHighPriorityTasks();
			auto it = delay_task_list_.begin();
			while (it != delay_task_list_.end())
			{
				if (it->NeedDoTask())
				{
					it->Run();
					it = delay_task_list_.erase(it);
					if (exit_)
						return;
				}
				else
				{
					it++;
				}
			}
			if (check_catch)
			{
				std::lock_guard<std::recursive_mutex> auto_lock(mut_delay_task_list_);
				if (!catch_delay_task_list_.empty())
				{
					delay_task_list_.swap(catch_delay_task_list_);
					RunDelayTasks(false);
				}
			}
		}
		void RunHighPriorityTasks()
		{
			if (exit_) return;
			while (!high_priority_task_list_.empty())
			{
				high_priority_task_list_.front()();
				high_priority_task_list_.pop();
				if (exit_) return;
			}
			std::lock_guard<std::recursive_mutex> auto_lock(mut_high_priority_task_list_);
			if (!catch_high_priority_task_list_.empty())
			{
				high_priority_task_list_.swap(catch_high_priority_task_list_);
				RunHighPriorityTasks();
			}
		}
		void RunTasks()
		{
			RunHighPriorityTasks();
			while (!task_list_.empty())
			{
				task_list_.front()();
				task_list_.pop();
				if (exit_)
					return;
				RunHighPriorityTasks();
			}
			std::lock_guard<std::recursive_mutex> auto_lock(mut_task_list_);
			if (!catch_task_list_.empty())
			{
				task_list_.swap(catch_task_list_);
				RunTasks();
			}
		}
	protected:
		RuntimeCallback begin_internal_callback_;
		RuntimeCallback end_internal_callback_;
	private:
		RuntimeCallback begin_callback_;
		RuntimeCallback end_callback_;


		std::queue< Task> task_list_;
		std::queue< Task> catch_task_list_;
		std::recursive_mutex mut_task_list_;

		std::queue< Task> high_priority_task_list_;
		std::queue< Task> catch_high_priority_task_list_;
		std::recursive_mutex mut_high_priority_task_list_;

		std::list< DelayTask> delay_task_list_;
		std::list< DelayTask> catch_delay_task_list_;
		std::recursive_mutex mut_delay_task_list_;

		std::recursive_mutex mut_loop_;
		std::condition_variable_any cv_loop;

		std::atomic_bool exit_;
		std::atomic_bool running_;
		std::unique_ptr< nbase::Thread> stl_thread_;
		std::mutex run_signal_lock_;
		std::condition_variable run_signal_cond_var_;

	};

}

#endif//_SDK_BASE_BASE_THREADEX_H_
