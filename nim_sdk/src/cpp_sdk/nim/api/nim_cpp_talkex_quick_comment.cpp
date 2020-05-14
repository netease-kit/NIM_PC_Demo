/** @file nim_cpp_talkex_quick_comment.cpp
  * @brief 快捷回复相关功能
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#include "src/cpp_sdk/nim/api//nim_cpp_talkex.h"
#include "src/cpp_sdk/nim/api//nim_cpp_global.h"
#include "src/cpp_sdk/nim/helper/nim_talkex_helper_quick_comment.h"
#include "public_define/defines/nim_define/nim_talkex_def_quick_comment.h"
namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_talkex_quick_comment_add_quick_comment)(const char * msg, const char* info, nim_talkex_quick_comment_add_quick_comment_cb_func cb, const void *user_data);
typedef void(*nim_talkex_quick_comment_remove_quick_comment)(const char* msg, const char* remove_param, nim_talkex_quick_comment_remove_quick_comment_cb_func cb, const void* user_data);
typedef void(*nim_talkex_quick_comment_query_quick_comment)(const char* query_param, nim_talkex_quick_comment_query_quick_comment_cb_func cb, const void* user_data);
typedef void(*nim_talkex_quick_comment_reg_add_quick_comment)(nim_talkex_quick_comment_add_quick_comment_notify_cb_func cb,const void* user_data);
typedef void(*nim_talkex_quick_comment_reg_remove_quick_comment)(nim_talkex_quick_comment_remove_quick_comment_notify_cb_func cb, const void* user_data);
#else
#include "nim_talkex.h"
#endif
    TalkEx::QuickComment::AddQuickCommentNotifyCallback g_add_qcnotify_callback = nullptr;
    void TalkEx::QuickComment::RegAddQuickCommentNotify(const AddQuickCommentNotifyCallback& cb)
    {
        g_add_qcnotify_callback = cb;
        NIM_SDK_GET_FUNC(nim_talkex_quick_comment_reg_add_quick_comment)(
            [](const char* session,int to_type,const char* msg_client_id,const char* quick_comment_info ,const void* user_data) {
                CallbackProxy::DoSafeCallback<AddQuickCommentNotifyCallback>(user_data, [=](const AddQuickCommentNotifyCallback& cb) {
                    QuickCommentInfo qc_info;
                    QuickCommentInfo::FromJsonString(quick_comment_info, qc_info);
                    CallbackProxy::Invoke(cb,session,(NIMSessionType)to_type, msg_client_id, qc_info);
                    },false);
           }, &g_add_qcnotify_callback);
    }
    TalkEx::QuickComment::RemoveQuickCommentNotifyCallback g_remove_qcnotify_callback = nullptr;
    void TalkEx::QuickComment::RegRemoveQuickCommentNotify(const RemoveQuickCommentNotifyCallback& cb)
    {
        g_remove_qcnotify_callback = cb;
        NIM_SDK_GET_FUNC(nim_talkex_quick_comment_reg_remove_quick_comment)(
            [](const char* session, int to_type, const char* msg_client_id, const char* id, const char* ext, const void* user_data) {
                CallbackProxy::DoSafeCallback<RemoveQuickCommentNotifyCallback>(user_data, [=](const RemoveQuickCommentNotifyCallback& cb) {                   
                    CallbackProxy::Invoke(cb, session, (NIMSessionType)to_type, msg_client_id, id,ext);
                    }, false);
            }, &g_remove_qcnotify_callback);
    }
    void TalkEx::QuickComment::UnregAllCb()
    {
        g_add_qcnotify_callback = nullptr;
        g_remove_qcnotify_callback = nullptr;
    }
    void TalkEx::QuickComment::AddQuickComment(const IMMessage &msg, const QuickCommentInfo& info, const AddQuickCommentCallback& cb)
    {
		AddQuickCommentCallback* cb_pointer = (cb == nullptr ? nullptr : new AddQuickCommentCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_quick_comment_add_quick_comment)(msg.ToJsonString(false).c_str(), 
            QuickCommentInfo::ToJsonString(info).c_str(),			
            [](int code, const char *quick_comment_info_json, const void *user_data) {
                CallbackProxy::DoSafeCallback<AddQuickCommentCallback>(user_data, [=](const AddQuickCommentCallback& cb){
					QuickCommentInfo quick_comment_info;
					QuickCommentInfo::FromJsonString(quick_comment_info_json, quick_comment_info);
                        CallbackProxy::Invoke(cb, code, quick_comment_info);
                    },true);
           }, cb_pointer);
	}
    void TalkEx::QuickComment::RemoveQuickComment(const IMMessage& msg, const RemoveQuickCommentParam& param, const RemoveQuickCommentCallback& cb)
    {
        RemoveQuickCommentCallback* cb_pointer = (cb == nullptr ? nullptr : new RemoveQuickCommentCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_quick_comment_remove_quick_comment)(msg.ToJsonString(false).c_str(),
            RemoveQuickCommentParam::ToJsonString(param).c_str(),
            [](int code, const char* id, const void* user_data) {
                CallbackProxy::DoSafeCallback<RemoveQuickCommentCallback>(user_data, [=](const RemoveQuickCommentCallback& cb) {
                   CallbackProxy::Invoke(cb, code, id);
                 }, true);
            }, cb_pointer);
    }
    void TalkEx::QuickComment::QueryQuickCommentList(const QueryQuickCommentsParam& query_param, const QueryQuickCommentCallback& cb)
    {
        QueryQuickCommentCallback* cb_pointer = (cb == nullptr ? nullptr : new QueryQuickCommentCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_quick_comment_query_quick_comment)(QueryQuickCommentsParam::ToJsonString(query_param).c_str(),
            [](int code, const char* response_json, const void* user_data) {
                CallbackProxy::DoSafeCallback<QueryQuickCommentCallback>(user_data, [=](const QueryQuickCommentCallback& cb) {
                    QueryQuickCommentsResponse response;
                    QueryQuickCommentsResponse::FromJsonString(response_json, response);
                    CallbackProxy::Invoke(cb, code, response);
                    }, true);
            }, cb_pointer);
    }
}
