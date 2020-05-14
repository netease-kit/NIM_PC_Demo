/** @file nim_cpp_talkex_quick_comment.cpp
  * @brief 快捷回复相关功能
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#include "src/cpp_sdk/nim/api//nim_cpp_talkex.h"
#include "src/cpp_sdk/nim/api//nim_cpp_global.h"
#include "src/cpp_sdk/nim/helper/nim_talkex_helper_pin_message.h"
#include "public_define/defines/nim_define/nim_talkex_def_quick_comment.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT

typedef void(*nim_talkex_pin_message_add_pin_message)(const char * msg, const char* info, nim_talkex_pin_message_add_pin_message_cb_func cb, const void *user_data);
typedef void(*nim_talkex_pin_message_unpin_message)(const char* unpin_param, nim_talkex_pin_message_unpin_message_cb_func cb, const void* user_data);
typedef void(*nim_talkex_pin_message_update_pin_message)(const char* modify_param, nim_talkex_pin_message_update_pin_message_cb_func cb, const void* user_data);
typedef void(*nim_talkex_pin_message_query_all_pin_message)(const char* session, int to_type, const nim_talkex_pin_message_query_all_pin_message_cb_func cb, const void* user_data);
typedef void(*nim_talkex_pin_message_reg_add_pin_message)(nim_talkex_pin_message_add_pin_message_notify_cb_func cb, const void* user_data);
typedef void(*nim_talkex_pin_message_reg_unpin_message)(nim_talkex_pin_message_unpin_message_notify_cb_func cb, const void* user_data);
typedef void(*nim_talkex_pin_message_reg_update_pin_message)(nim_talkex_pin_message_update_pin_message_notify_cb_func cb, const void* user_data);
#else
#include "nim_talkex.h"
#endif

    void TalkEx::PinMsg::AddPinMessage(const IMMessage& msg, const PinMessageInfo& pin_info, const PinMessageCallback& cb)
    {
        PinMessageCallback* cb_pointer = (cb == nullptr ? nullptr : new PinMessageCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_add_pin_message)(msg.ToJsonString(false).c_str(), 
            PinMessageInfo::ToJsonString(pin_info).c_str(),
            [](int code, const char* session, int to_type, const char* pin_info, const void* user_data) {
                CallbackProxy::DoSafeCallback<PinMessageCallback>(user_data, [=](const PinMessageCallback& cb) {
                    PinMessageInfo info;
                    PinMessageInfo::FromJsonString(pin_info, info);
                    CallbackProxy::Invoke(cb, code, session,to_type, info);
                    }, true);
            }, cb_pointer);
    } 
    void TalkEx::PinMsg::UnPinMessage(const ModifyPinMessageParam& unpin_param, const UnPinMessageCallback& cb)
    {
        UnPinMessageCallback* cb_pointer = (cb == nullptr ? nullptr : new UnPinMessageCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_unpin_message)(ModifyPinMessageParam::ToJsonString(unpin_param).c_str(),
            [](int code, const char* session, int to_type, const char* id, const void* user_data) {
                CallbackProxy::DoSafeCallback<UnPinMessageCallback>(user_data, [=](const UnPinMessageCallback& cb) {
                    CallbackProxy::Invoke(cb, code, session, to_type, id);
                    }, true);
            }, cb_pointer);
    }
    void TalkEx::PinMsg::UpdatePinMessage(const ModifyPinMessageParam& modify_param, const UpdatePinMessageCallback& cb)
    {
        UpdatePinMessageCallback* cb_pointer = (cb == nullptr ? nullptr : new UpdatePinMessageCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_update_pin_message)(ModifyPinMessageParam::ToJsonString(modify_param).c_str(),
            [](int code, const char* session, int to_type, const char* pin_info, const void* user_data) {
                CallbackProxy::DoSafeCallback<UpdatePinMessageCallback>(user_data, [=](const UpdatePinMessageCallback& cb) {
                    PinMessageInfo info;
                    PinMessageInfo::FromJsonString(pin_info, info);
                    CallbackProxy::Invoke(cb, code, session, to_type, info);
                    }, true);
            }, cb_pointer);
    }
    void TalkEx::PinMsg::QueryAllPinMessage(const std::string& session, int to_type, const QueryPinMessageCallback& cb)
    {
        QueryPinMessageCallback* cb_pointer = (cb == nullptr ? nullptr : new QueryPinMessageCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_query_all_pin_message)(session.c_str(),to_type,
            [](int code, const char* session, int to_type, const char* pin_info, const void* user_data) {
                CallbackProxy::DoSafeCallback<QueryPinMessageCallback>(user_data, [=](const QueryPinMessageCallback& cb) {
                    QueryAllPinMessageResponse response;
                    QueryAllPinMessageResponse::FromJsonString(pin_info, response);
                    CallbackProxy::Invoke(cb, code, session, to_type, response);
                    }, true);
            }, cb_pointer);
    }
    TalkEx::PinMsg::AddPinMessageNotifyCallback g_add_pin_message_notify_cb = nullptr;
    void TalkEx::PinMsg::RegAddPinMessage(const AddPinMessageNotifyCallback& cb)
    {
        g_add_pin_message_notify_cb = cb;
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_reg_add_pin_message)(
            [](const char* session, int to_type, const char* pin_info, const void* user_data) {
                CallbackProxy::DoSafeCallback<AddPinMessageNotifyCallback>(user_data, [=](const AddPinMessageNotifyCallback& cb) {
                    PinMessageInfo info;
                    PinMessageInfo::FromJsonString(pin_info, info);
                    CallbackProxy::Invoke(cb, session, to_type, info);
                    },false);
            }, & g_add_pin_message_notify_cb);
    }
    TalkEx::PinMsg::UnPinMessageNotifyCallback g_unpin_message_notify_cb = nullptr;
    void TalkEx::PinMsg::RegUnPinMessage(const UnPinMessageNotifyCallback& cb)
    {
        g_unpin_message_notify_cb = cb;
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_reg_unpin_message)(
            [](const char* session, int to_type, const char* id, const void* user_data) {
                CallbackProxy::DoSafeCallback<UnPinMessageNotifyCallback>(user_data, [=](const UnPinMessageNotifyCallback& cb) {
                    CallbackProxy::Invoke(cb, session, to_type, id);
                    }, false);
            }, &g_unpin_message_notify_cb);
    }
    TalkEx::PinMsg::UpdatePinMessageNotifyCallback g_update_pin_message_notify_cb = nullptr;
    void TalkEx::PinMsg::RegUpdatePinMessage(const UpdatePinMessageNotifyCallback& cb)
    {
        g_update_pin_message_notify_cb = cb;
        NIM_SDK_GET_FUNC(nim_talkex_pin_message_reg_update_pin_message)(
            [](const char* session, int to_type, const char* pin_info, const void* user_data) {
                CallbackProxy::DoSafeCallback<UpdatePinMessageNotifyCallback>(user_data, [=](const UpdatePinMessageNotifyCallback& cb) {
                    PinMessageInfo info;
                    PinMessageInfo::FromJsonString(pin_info, info);
                    CallbackProxy::Invoke(cb, session, to_type, info);
                    }, false);
            }, & g_update_pin_message_notify_cb);
    }
    void TalkEx::PinMsg::UnregAllCb()
    {
        g_add_pin_message_notify_cb = nullptr;
        g_unpin_message_notify_cb = nullptr;
        g_update_pin_message_notify_cb = nullptr;
    }
}
