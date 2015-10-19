#ifndef SHARED_THREADS_H_
#define SHARED_THREADS_H_

// thread ids
enum ThreadId
{
	kThreadUI,			//UI线程（主线程）
	kThreadDatabase,	//写DB线程
	kThreadGlobalMisc,	//全局Misc线程（比如：打开url）
	kThreadDuilibHelper,//Duilib专用，UI辅助线程
};

#endif // SHARED_THREADS_H_
