#pragma once

typedef std::function<void(const std::string &, bool)> SetStateCallback;
namespace nim_comp
{
	
class MuteBlackService : public nbase::SupportWeakCallback
{
public:
SINGLETON_DEFINE(MuteBlackService);

private:
	MuteBlackService();
	~MuteBlackService() {}

public:
	const std::set<std::string> &GetMuteList();					//获取静音列表
	const std::set<std::string> &GetBlackList();				//获取黑名单列表
	bool IsInMuteList(const std::string& id);					//检查用户是否在静音列表中
	bool IsInBlackList(const std::string& id);					//检查用户是否在黑名单中
	void InvokeSetMute(const std::string& id, bool mute);		//改变某个用户的静音状态
	void InvokeSetBlack(const std::string& id, bool black);		//改变某个用户的拉黑状态
	UnregisterCallback RegSyncSetMuteCallback(const SetStateCallback& cb);	//注册静音事件的响应函数
	UnregisterCallback RegSyncSetBlackCallback(const SetStateCallback& cb);	//注册黑名单事件的响应函数

private:
	void OnGetMuteListCallback(nim::NIMResCode res_code, const std::list<nim::MuteListInfo>& lists);		//静态对象创建时，获取并填充两个列表
	void OnGetBlackListCallback(nim::NIMResCode res_code, const std::list<nim::BlackListInfo>& lists);		//静态对象创建时，获取并填充两个列表
	void OnMuteBlackEventCallback(const nim::SpecialRelationshipChangeEvent& change_event);	//收到静音或黑名单事件时的响应
	void ModifyMuteList(const std::string& id, bool mute);		//修改m_mute_list
	void ModifyBlackList(const std::string& id, bool black);	//修改m_black_list

private:
	std::set<std::string> m_mute_list;
	std::set<std::string> m_black_list;
	std::map<int, std::unique_ptr<SetStateCallback>> sync_set_mute_cb_list;
	std::map<int, std::unique_ptr<SetStateCallback>> sync_set_black_cb_list;
};
}