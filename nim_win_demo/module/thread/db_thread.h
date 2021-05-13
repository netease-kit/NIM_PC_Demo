#pragma once
#include "shared/threads.h"

/** @class DBThread
  * @brief 数据库操作线程
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class DBThread : public nbase::FrameworkThread
{
public:
	/**
	* 构造函数
	* @param[in] thread_id 线程id
	* @param[in] name 线程名字
	*/
	DBThread(enum  ThreadId thread_id, const char *name);
	~DBThread(void);

private:
	/**
	* 虚函数，初始化数据库线程
	* @return void	无返回值
	*/
	virtual void Init() override;

	/**
	* 虚函数，数据库线程退出时，做一些清理工作
	* @return void	无返回值
	*/
	virtual void Cleanup() override;

private:
	enum ThreadId thread_id_;
};