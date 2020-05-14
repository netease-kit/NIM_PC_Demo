/** @file nim_cpp_talkex_collect.cpp
  * @brief 收藏相关功能
  * @copyright (c) 2015-2020, NetEase Inc. All rights reserved
  * @date 2020/04/20
  */

#include "src/cpp_sdk/nim/api//nim_cpp_talkex.h"
#include "src/cpp_sdk/nim/helper/nim_talkex_helper_collect.h"
#include "src/cpp_sdk/nim/api//nim_cpp_global.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_talkex_collect_add_collect)(const char * collect_info, nim_talkex_collect_add_collect_cb_func cb, const void *user_data);
typedef void(*nim_talkex_collect_remove_collects)(const char* collect_list, nim_talkex_collect_remove_collects_cb_func cb, const void* user_data);
typedef void(*nim_talkex_collect_update_collect_ext)(const char* collect_match_param, const char* ext, nim_talkex_collect_update_collect_ext_cb_func cb, const void* user_data);
typedef void(*nim_talkex_collect_query_collects)(const char* query_collect_list_param, nim_talkex_collect_query_collects_cb_func cb, const void* user_data);


#else
#include "nim_talkex.h"
#endif
    void TalkEx::Collect::AddCollect(const CollectInfo& collect_info, const AddCollectCallback& cb)
    {
        AddCollectCallback* cb_pointer = (cb == nullptr ? nullptr : new AddCollectCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_collect_add_collect)(CollectInfo::ToJsonString(collect_info).c_str(),
            [](int code, const char* collect_info_json, const void* user_data) {
                CallbackProxy::DoSafeCallback<AddCollectCallback>(user_data, [=](const AddCollectCallback& cb){
                        CollectInfo collect_info;
                        CollectInfo::FromJsonString(collect_info_json,collect_info);
                        CallbackProxy::Invoke(cb, code, collect_info);
                    },true);
           }, cb_pointer);
    }
    void TalkEx::Collect::RemoveCollects(const RemoveCollectsParm& collect_list, const RemoveCollectsCallback& cb)
    {
        if (collect_list.Size() <= 0)
            return;
        RemoveCollectsCallback* cb_pointer = (cb == nullptr ? nullptr : new RemoveCollectsCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_collect_remove_collects)(RemoveCollectsParm::ToJsonString(collect_list).c_str(),
            [](int code, int count, const void* user_data) {
                CallbackProxy::DoSafeCallback<RemoveCollectsCallback>(user_data, [=](const RemoveCollectsCallback& cb) {
                    CallbackProxy::Invoke(cb, code, count);
                 }, true);
            }, cb_pointer);
    }
    void TalkEx::Collect::UpdateCollectExt(const MatchCollectParm& collect_match_param, const std::string& ext, const UpdateCollectCallback& cb)
    {
        UpdateCollectCallback* cb_pointer = (cb == nullptr ? nullptr : new UpdateCollectCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_collect_update_collect_ext)(MatchCollectParm::ToJsonString(collect_match_param).c_str(), ext.c_str(),
            [](int code, const char* collect_info_json, const void* user_data) {
                CallbackProxy::DoSafeCallback<UpdateCollectCallback>(user_data, [=](const UpdateCollectCallback& cb) {
                    CollectInfo collect_info;
                    CollectInfo::FromJsonString(collect_info_json, collect_info);
                    CallbackProxy::Invoke(cb, code, collect_info);
                    }, true);
            }, cb_pointer);
    }
    void TalkEx::Collect::QueryCollectList(const QueryCollectsParm& query_collect_list_param, const QueryCollectsCallback& cb)
    {
        QueryCollectsCallback* cb_pointer = (cb == nullptr ? nullptr : new QueryCollectsCallback(cb));
        NIM_SDK_GET_FUNC(nim_talkex_collect_query_collects)(QueryCollectsParm::ToJsonString(query_collect_list_param).c_str(),
            [](int code, int total_count,const char* collect_info_list_json, const void* user_data) {
                CallbackProxy::DoSafeCallback<QueryCollectsCallback>(user_data, [=](const QueryCollectsCallback& cb) {
                    CollectInfoList collect_info_list;
                    CollectInfoList::FromJsonString(collect_info_list_json, collect_info_list);
                    CallbackProxy::Invoke(cb, code, total_count,collect_info_list);
                    }, true);
            }, cb_pointer);
    }
}
