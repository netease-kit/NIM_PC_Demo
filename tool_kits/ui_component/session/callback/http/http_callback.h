#pragma once

namespace nim_comp
{
/** @class HttpCallback
  * @brief http请求事件回调类
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class HttpCallback
{
public:
	/**
	* http下载回调，需要在程序开始运行时注册好。
	* @param[in] code 返回码
	* @param[in] file_path 文件下载保存的路径
	* @param[in] sid 会话id（好友id或群id）
	* @param[in] cid 消息id
	* @return void	无返回值
	*/
	static void OnHttpDownloadCallback(nim::NIMResCode code, const std::string& file_path, const std::string& sid, const std::string& cid);
};
}