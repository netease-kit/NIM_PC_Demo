#pragma once


typedef std::function<void()> UnregisterCallback;

class AutoUnregister 
{
public:
	AutoUnregister()
	{

	}

	void Add(UnregisterCallback cb)
	{
		cb_list_.push_back(cb);
	}

	~AutoUnregister()
	{
		for (auto iter = cb_list_.begin(); iter != cb_list_.end(); iter ++)
			(*iter)();
	}

private:
	std::list<UnregisterCallback> cb_list_;
};

