#include "nim_talk_cpp_wrapper.h"
#include "nim_sdk_function.h"

namespace nim
{

typedef void(*nim_talk_reg_arc_cb)(const char *json_extension, nim_talk_arc_cb_func cb, const void *user_data);
typedef void(*nim_talk_send_msg)(const char* json_msg, const char *json_extension, nim_http_up_prg_cb_func prg_cb, const void *prg_user_data);
typedef void(*nim_talk_stop_send_msg)(const char *json_msg, const char *json_extension);
typedef void(*nim_talk_reg_custom_sysmsg_arc_cb)(const char *json_extension, nim_custom_sysmsg_arc_cb_func cb, const void *user_data);
typedef void(*nim_talk_send_custom_sysmsg)(const char *json_msg, const char *json_extension);
typedef void(*nim_talk_reg_receive_cb)(const char *json_extension, nim_talk_receive_cb_func cb, const void* user_data);


static void CallbackWrapper1NoDelete(const char *result, const void *user_data)
{
	if (user_data)
	{
		Talk::SendMsgCallback* cb_pointer = (Talk::SendMsgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(result)));
		}
	}
}

static void CallbackWrapper2NoDelete(const char *content, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Talk::SendMsgCallback* cb_pointer = (Talk::SendMsgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), PCharToString(content)));
		}
	}
}

static void CallbackWrapper3NoDelete(__int64 uploaded_size, __int64 file_size, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Talk::FileUpPrgCallback* cb_pointer = (Talk::FileUpPrgCallback*)user_data;
		if (*cb_pointer)
		{
			PostTaskToUIThread(std::bind((*cb_pointer), uploaded_size, file_size));
		}
	}
}

static Talk::SendMsgCallback* g_send_msg_cb_pointer = nullptr;
void Talk::RegSendMsgCb(const SendMsgCallback& cb, const std::string& json_extension)
{
	delete g_send_msg_cb_pointer;
	if (cb)
	{
		g_send_msg_cb_pointer = new SendMsgCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_talk_reg_arc_cb)(json_extension.c_str(), &CallbackWrapper1NoDelete, g_send_msg_cb_pointer);
}

void Talk::SendMsg(const std::string& json_msg, std::string msg_id, FileUpPrgCallback* pcb)
{
	if (pcb)
	{
		return NIM_SDK_Get_Function(nim_talk_send_msg)(json_msg.c_str(), nullptr, &CallbackWrapper3NoDelete, pcb);
	} 
	else
	{
		return NIM_SDK_Get_Function(nim_talk_send_msg)(json_msg.c_str(), nullptr, nullptr, nullptr);
	}
}
void Talk::StopSendMsg(const std::string& json_msg, const std::string& json_extension)
{
	NIM_SDK_Get_Function(nim_talk_stop_send_msg)(json_msg.c_str(), json_extension.c_str());
}

static Talk::SendCustomSysmsgCallback* g_send_custom_sysmsg_cb_pointer = nullptr;
void Talk::RegSendCustomSysmsgCb(const SendCustomSysmsgCallback& cb, const std::string& json_extension)
{
	delete g_send_custom_sysmsg_cb_pointer;
	if (cb)
	{
		g_send_custom_sysmsg_cb_pointer = new SendCustomSysmsgCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_talk_reg_custom_sysmsg_arc_cb)(json_extension.c_str(), &CallbackWrapper1NoDelete, g_send_custom_sysmsg_cb_pointer);
}

void Talk::SendCustomSysmsg(const std::string& json_msg)
{
	return NIM_SDK_Get_Function(nim_talk_send_custom_sysmsg)(json_msg.c_str(), nullptr);
}

static Talk::ReveiveMsgCallback* g_cb_pointer = nullptr;
void Talk::RegReceiveCb(const ReveiveMsgCallback& cb, const std::string& json_extension)
{
	delete g_cb_pointer;
	if (cb)
	{
		g_cb_pointer = new ReveiveMsgCallback(cb);
	}
	return NIM_SDK_Get_Function(nim_talk_reg_receive_cb)(json_extension.c_str(), &CallbackWrapper2NoDelete, g_cb_pointer);
}

}