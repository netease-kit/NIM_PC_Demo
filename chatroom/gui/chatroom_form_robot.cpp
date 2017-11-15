#include "chatroom_form.h"
#include "chatroom_frontpage.h"
#include "nim_chatroom_helper.h"
#include "module/session/session_util.h"

using namespace nim_comp;

#define ROOMMSG_R_N _T("\r\n")
namespace nim_chatroom
{
	class ScoreCompare
	{
	public:
		bool operator()(const Json::Value& first, const Json::Value& last)
		{
			int first_score = first["score"].asUInt();
			int last_score = last["score"].asUInt();
			return first_score < last_score;
		}
	};

	static void SubAddSender(ui::RichEdit* edit, const std::string &robot_id, const std::string &robot_name, bool is_history, bool first_msg_each_batch = false)
	{
		long old_begin = 0, old_end = 0;
		if (edit->GetTextLength() != 0)
		{
			if (is_history)
			{
				if (first_msg_each_batch)
					edit->SetSel(0, 0);
				else
					edit->GetSel(old_begin, old_end);
				edit->ReplaceSel(ROOMMSG_R_N, false);
			}
			else
			{
				edit->SetSel(-1, -1);
				edit->ReplaceSel(ROOMMSG_R_N, false);
			}
		}

		std::wstring w_show_name = nbase::UTF8ToUTF16(robot_name);
		//
		long lSelBegin = 0, lSelEnd = 0;
		CHARFORMAT2 cf;
		edit->GetDefaultCharFormat(cf); //获取消息字体
		cf.dwMask = CFM_LINK | CFM_FACE | CFM_COLOR;
		cf.dwEffects = CFE_LINK;

		// 添加发言人，并设置他的颜色
		lSelEnd = lSelBegin = is_history ? old_end + 1 : edit->GetTextLength();
		edit->SetSel(lSelEnd, lSelEnd);
		edit->ReplaceSel(w_show_name, false);

		long lNewSelBegin = 0, lNewSelEnd = 0;
		edit->GetSel(lNewSelBegin, lNewSelEnd);
		edit->ReplaceSel(L" ", false);

		edit->SetSel(lSelBegin, lNewSelEnd);
		edit->SetSelectionCharFormat(cf);

		// 设置文本的缩进
		PARAFORMAT2 pf;
		ZeroMemory(&pf, sizeof(PARAFORMAT2));
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_STARTINDENT | PFM_LINESPACING;
		LONG lineSpace = (LONG)(1.2 * 20);//设置1.2行间距
		pf.dxStartIndent = 0;
		pf.bLineSpacingRule = 5;
		pf.dyLineSpacing = lineSpace;
		edit->SetParaFormat(pf);

		edit->SetSel(lNewSelEnd + 1, lNewSelEnd + 1);
	}

	static void AddSubText(ui::RichEdit* edit, const std::wstring &text, bool end_down)
	{
		if (text.empty()) return;

		long lSelBegin = 0, lSelEnd = 0;
		CHARFORMAT2 cf;

		// 添加正文，并设置他的颜色	
		edit->ReplaceSel(ROOMMSG_R_N, false);
		edit->GetSel(lSelBegin, lSelEnd);
		edit->ReplaceSel(text, false);
		//设置文本字体
		edit->GetDefaultCharFormat(cf); //获取消息字体
		long lSelBegin2 = 0, lSelEnd2 = 0;
		edit->GetSel(lSelBegin2, lSelEnd2);
		edit->SetSel(lSelEnd, lSelEnd2);
		edit->SetSelectionCharFormat(cf);

		// 设置文本的缩进
		PARAFORMAT2 pf;
		ZeroMemory(&pf, sizeof(PARAFORMAT2));
		pf.cbSize = sizeof(pf);
		pf.dwMask = PFM_STARTINDENT | PFM_LINESPACING;
		LONG lineSpace = (LONG)(1.2 * 20);//设置1.2行间距
		pf.dxStartIndent = 150;
		pf.bLineSpacingRule = 5;
		pf.dyLineSpacing = lineSpace;
		edit->SetSel(lSelEnd2, lSelEnd2);
		edit->SetParaFormat(pf);

		if (end_down)
			edit->EndDown();
	}

	void ChatroomForm::AddImage(const std::string &url, bool end_down, const std::wstring &file_tag/* = L""*/)
	{
		long old_begin = 0, old_end = 0;
		msg_list_->GetSel(old_begin, old_end);
		msg_list_->ReplaceSel(ROOMMSG_R_N, false);
		old_end++;
		std::wstring photo_path = PhotoService::GetInstance()->GetPhotoDir(kOther) + nbase::UTF8ToUTF16(QString::GetMd5(url));
		ITextServices *text_service = msg_list_->GetTextServices();
		if (text_service)
		{
			if (PhotoService::GetInstance()->CheckPhotoOK(photo_path))
			{
				Re_InsertImage(text_service, InsertCustomItemErrorCallback(), false, photo_path, file_tag, false);
			}
			else
			{
				Re_InsertImage(text_service, InsertCustomItemErrorCallback(), false, photo_path, file_tag, true);
				DownloadImage(url, photo_path, false, !end_down);
			}
		}
		text_service->Release();

		msg_list_->SetSel(old_end + 1, old_end + 1);

		if (end_down)
			msg_list_->EndDown();
	}

	void ChatroomForm::OnDownloadCallback(bool success, const std::string& file_path, bool is_complex_element, bool is_history)
	{
		if (is_complex_element && complex_json_.size() > 0)
		{
			Json::Value new_complex_json;
			int len = complex_json_.size();
			for (int i = 0; i < len; i++)
			{
				bool all_ready = true;
				int link_len = complex_json_[i].size();
				Json::Value &link_elements = complex_json_[i]["element"]["content"];
				for (int j = 0; j < link_len; j++)
				{
					all_ready &= PhotoService::GetInstance()->CheckPhotoOK(nbase::UTF8ToUTF16(file_path));
				}
				if (all_ready)
				{
					ITextServices *service = msg_list_->GetTextServices();
					if (service)
					{
						std::string id = complex_json_[i]["id"].asString();
						long old_end = (long)Re_RemoveCustomItem(service, nbase::UTF8ToUTF16(id));
						if (old_end >= 0)
						{
							msg_list_->SetSel(old_end, old_end);
							Re_InsertDescriptionItem(service
								, InsertCustomItemErrorCallback()
								, false
								, nbase::UTF8ToUTF16(complex_json_[i]["element"].toStyledString())
								, nbase::UTF8ToUTF16(id)
								, RE_OLE_TYPE_DESCRIPTION
								, RGB(255, 255, 255)
								, RGB(51, 51, 51)
								, 12);
							service->Release();
							descripts_info_[id] = complex_json_[i]["link_info"];
						}
					}
				}
				else
				{
					new_complex_json.append(complex_json_[i]);
				}
			}
 			complex_json_ = new_complex_json;
		}
		else
		{
			ITextServices *service = msg_list_->GetTextServices();
			if (service)
			{
				Re_ImageLoadingFinish(service, nbase::UTF8ToUTF16(file_path), success, false);
				service->Release();
			}
		}
		if (!is_history)
			msg_list_->EndDown();
	}

	void ChatroomForm::DownloadImage(const std::string &url, const std::wstring &photo_path, bool is_complex_element, bool is_history)
	{
		nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, photo_path, is_complex_element, is_history](int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
			if (res_code == nim::kNIMResSuccess)
			{
				std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
				if (nbase::FilePathIsExist(ws_file_path, false))
				{
					nbase::CopyFileW(ws_file_path, photo_path);
					nbase::DeleteFile(ws_file_path);
				}
				std::string s_photo_path = nbase::UTF16ToUTF8(photo_path);
				OnDownloadCallback(true, s_photo_path, is_complex_element, is_history);
			}
		});
		nim::NOS::DownloadResource(url, cb);
	}

	void ChatroomForm::AddRobotMsg(const ChatRoomMessage& result, bool is_history, bool first_msg_each_batch/* = false*/)
	{
		nim::IMBotRobot robot_attach;
		std::wstring str;
		Json::Value values;
		Json::Reader reader;
		if (reader.parse(result.msg_attach_, values) && values.isObject())
		{
			robot_attach.robot_accid_ = values[nim::kNIMBotRobotMsgKeyRobotID].asString();
			robot_attach.related_msg_id_ = values[nim::kNIMBotRobotReceivedMsgKeyClientMsgID].asString();
			robot_attach.out_msg_ = values[nim::kNIMBotRobotReceivedMsgKeyMsgOut].asBool();
			robot_attach.robot_msg_flag_ = values[nim::kNIMBotRobotReceivedMsgKeyRobotMsg][nim::kNIMBotRobotReceivedMsgKeyRobotMsgFlag].asString();
			robot_attach.robot_msg_content_ = values[nim::kNIMBotRobotReceivedMsgKeyRobotMsg][nim::kNIMBotRobotReceivedMsgKeyRobotMsgMessage];
		}

		if (values.isObject())
		{
			QLOG_APP(L"body:{0}\r\nattach:{1}") << nbase::UTF8ToUTF16(result.msg_body_) << nbase::UTF8ToUTF16(values.toStyledString());
		}

		std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
		if (!robot_attach.out_msg_)
		{//客户端发给机器人
			AddText(nbase::UTF8ToUTF16(result.msg_body_), result.from_nick_.empty() ? my_name : nbase::UTF8ToUTF16(result.from_nick_), result.from_id_, kMember, is_history, first_msg_each_batch);
			return;
		}

		nim::RobotInfo info;
		UserService::GetInstance()->GetRobotInfo(robot_attach.robot_accid_, info);
		std::wstring robot_name = nbase::UTF8ToUTF16(info.GetName());

		if (robot_attach.robot_msg_flag_ == "faq")
		{
			//answer_type定义
			//1能匹配到问答对库中一个相似度高的问题;
			//2能匹配到问答对库中若干个相似度高的问题;
			//3能匹配到问答对库中若干个相似度较低的问题;
			//4不能匹配问题;
			//5能匹配到寒暄库中一个相似度高的问题
			std::string answer_type = robot_attach.robot_msg_content_["answer_type"].asString();
			std::string query = robot_attach.robot_msg_content_["query"].asString();
			Json::Value match_answers = robot_attach.robot_msg_content_["match"];
			int len = match_answers.size();
			std::list<Json::Value> lst;
			for (int i = 0; i < len; i++)
			{
				lst.push_back(match_answers[i]);
			}
			lst.sort(ScoreCompare());
			if (lst.size() > 0)
			{
				AddText(nbase::UTF8ToUTF16((*lst.begin())["answer"].asString()), robot_name, robot_attach.robot_accid_, kRobot, is_history, first_msg_each_batch);
			}
		}
		else if (robot_attach.robot_msg_flag_ == "bot")
		{
			std::wstring w_show_name = nbase::UTF8ToUTF16(info.GetName());
			Json::Value values;
			values["type"] = kRobot;
			values["id"] = robot_attach.robot_accid_;
			values["name"] = info.GetName();
			msg_list_sender_name_link_[w_show_name] = values;
			SubAddSender(msg_list_, robot_attach.robot_accid_, info.GetName(), is_history, first_msg_each_batch);
			Json::Value msgs = robot_attach.robot_msg_content_;
			int len = (int)msgs.size();
			for (int i = 0; i < len; i++)
			{
				std::string msg_type = msgs[i]["type"].asString();
				if (msg_type == "01" || msg_type == "03")//01文字 03快速回复
				{
					AddSubText(msg_list_, nbase::UTF8ToUTF16(msgs[i]["content"].asString()), !is_history);
				}
				else if (msg_type == "02")//图片
				{
					std::string url = msgs[i]["content"].asString();
					AddImage(url, !is_history);
				}
				else if (msg_type == "11") //复杂类型
				{
					TiXmlDocument document;
					document.Parse(msgs[i]["content"].asString().c_str(), 0, TIXML_ENCODING_UTF8);
					TiXmlElement* root = document.RootElement();
					if (root)
					{
						if (auto pchar = root->Attribute("version")) {
							QLOG_APP(L"\r\nBot 11 template version:{0}") << nbase::UTF8ToUTF16(pchar);
						}
						auto linear_layout = root->FirstChild("LinearLayout");
						bool first_layout = true;
						while (linear_layout)
						{
							auto node = linear_layout->FirstChild();
							bool first_node = true;
							while (node)
							{
								if (node->ValueStr() == "text")
								{
									auto val = node->ToElement()->GetText();
									if (val)
									{
										AddSubText(msg_list_, nbase::UTF8ToUTF16((std::string)val), !is_history);
									}
								}
								else if (node->ValueStr() == "link")
								{
									Json::Value values;
									auto val = node->ToElement()->Attribute("type");
									if (val)
										values["type"] = (std::string)val;
									val = node->ToElement()->Attribute("target");
									if (val)
										values["target"] = (std::string)val;

									val = node->ToElement()->Attribute("params");
									if (val)
										values["params"] = (std::string)val;
									values[nim::kNIMBotRobotMsgKeyRobotID] = robot_attach.robot_accid_;
									std::string show_text;
									bool need_download = false;
									std::list<std::string> need_download_urls;
									Json::Value link_json;
									link_json["id"] = values["target"];
									link_json["element"]["config"]["bubble_offset_x"] = 10;
									link_json["element"]["config"]["bubble_offset_y"] = 10;
									link_json["element"]["config"]["line_space"] = 1;
									link_json["element"]["config"]["text_left"] = 15;
									link_json["element"]["config"]["text_top"] = 3;
									link_json["element"]["config"]["img_top"] = 3;
									link_json["element"]["config"]["img_left"] = 10;
									link_json["element"]["config"]["img_right"] = 10;
									link_json["element"]["config"]["total_width"] = 200;
									auto sub_node = node->FirstChild();
									bool first_sub_node = true;
									while (sub_node)
									{
										if (sub_node->ValueStr() == "text")
										{
											val = sub_node->ToElement()->GetText();
											if (val)
											{
												show_text = (std::string)val;
												values["link_text"] = show_text;
												Json::Value txt;
												txt["type"] = "text";
												txt["content"] = show_text;
												link_json["element"]["content"].append(txt);
											}
										}
										else if (sub_node->ValueStr() == "image")
										{
											val = sub_node->ToElement()->Attribute("url");
											if (val)
											{
												std::string url = (std::string)val;
												std::string img_id = QString::GetMd5(url);
												Json::Value img;
												img["type"] = "img";
												img["id"] = img_id;
												std::wstring photo_path = PhotoService::GetInstance()->GetPhotoDir(kOther) + nbase::UTF8ToUTF16(img_id);
												if (!PhotoService::GetInstance()->CheckPhotoOK(photo_path))
												{
													need_download_urls.push_back(url);
													need_download = true;
												}
												img["file"] = nbase::UTF16ToUTF8(photo_path);
												img["width"] = 100;
												link_json["element"]["content"].append(img);
											}
										}

										sub_node = sub_node->NextSibling();
										if (first_sub_node)
											first_sub_node = false;
									}
									link_json["link_info"] = values;
									if (need_download)
									{
										complex_json_.append(link_json);
										ITextServices *text_service = msg_list_->GetTextServices();
										if (text_service)
										{
											msg_list_->ReplaceSel(ROOMMSG_R_N, false);
											Re_InsertImage(text_service, InsertCustomItemErrorCallback(), false, L"", nbase::UTF8ToUTF16(link_json["id"].asString()), true);
											text_service->Release();
										}
										for (auto &url : need_download_urls)
										{
											std::wstring photo_path = PhotoService::GetInstance()->GetPhotoDir(kOther) + nbase::UTF8ToUTF16(QString::GetMd5(url));
											DownloadImage(url, photo_path, true, is_history);
										}
									}
									else
									{
										ITextServices *service = msg_list_->GetTextServices();
										if (service)
										{
											msg_list_->ReplaceSel(ROOMMSG_R_N, false);
											Re_InsertDescriptionItem(service
												, InsertCustomItemErrorCallback()
												, false
												, nbase::UTF8ToUTF16(link_json["element"].toStyledString())
												, nbase::UTF8ToUTF16(link_json["id"].asString())
												, RE_OLE_TYPE_DESCRIPTION
												, RGB(255, 255, 255)
												, RGB(51,51,51)
												, 12);
											service->Release();
											descripts_info_[link_json["id"].asString()] = values;
											if (!is_history)
												msg_list_->EndDown();
										}
									}
								}
								else
								{
									assert(0);
									QLOG_APP(L"\r\nComplex template parse WARN, {0}") << nbase::UTF8ToUTF16(node->ValueStr());
								}
								node = node->NextSibling();
								if (first_node)
									first_node = false;
							}
							linear_layout = linear_layout->NextSibling();
							if (first_layout)
								first_layout = false;
						}
					}
				}
			}
		}
		else
		{
			assert(0);
			std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
			AddText(nbase::UTF8ToUTF16(result.msg_body_), result.from_nick_.empty() ? my_name : nbase::UTF8ToUTF16(result.from_nick_), result.from_id_, kMember ,is_history, first_msg_each_batch);
		}
	}

	LRESULT ChatroomForm::HandleDiscuzMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!is_anonymity_)
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ui::Control *control = FindControl(pt);
			if (control != nullptr && control == msg_list_)
			{
				if (uMsg == WM_MOUSEMOVE)
				{
					std::wstring tool_tip;
					//判断是否点击在图片上
					ITextServices * text_service = msg_list_->GetTextServices();
					if (text_service)
					{
						RE_OLE_ITEM_CONTENT item = Re_CustomImageOleHitTest(text_service, pt);
						if (item.type_ == RE_OLE_TYPE_DESCRIPTION)
						{
							tool_tip = L"点击";
						}
					}
					msg_list_->SetToolTipText(tool_tip);
				}
				else if (uMsg == WM_LBUTTONUP)
				{
					//判断是否点击在图片上
					ITextServices * text_service = msg_list_->GetTextServices();
					if (text_service)
					{
						RE_OLE_ITEM_CONTENT item = Re_CustomImageOleHitTest(text_service, pt);
						if (item.type_ == RE_OLE_TYPE_DESCRIPTION)
						{
							auto iter = descripts_info_.find(nbase::UTF16ToUTF8(item.content_));
							if (iter != descripts_info_.end())
							{
								Json::Value values = iter->second;
								std::string type = values["type"].asString();
								if (type == "url")
								{
									std::string url = values["target"].asString();
									Post2GlobalMisc(nbase::Bind(&shared::tools::SafeOpenUrl, nbase::UTF8ToUTF16(url), SW_SHOW));
								}
								else if (type == "block")
								{
									std::string robot_id = values[nim::kNIMBotRobotMsgKeyRobotID].asString();
									std::string content = values["link_text"].asString();
									nim::IMBotRobot bot;
									bot.robot_accid_ = robot_id;
									bot.sent_param_["target"] = values["target"].asString();
									bot.sent_param_["type"] = "03";
									bot.sent_param_["params"] = values["params"].asString();
									std::string json_msg = ChatRoom::CreateRoomMessage(kNIMChatRoomMsgTypeRobot, QString::GetGUID(), bot.ToJsonString(), content, ChatRoomMessageSetting());
									ChatRoom::SendMsg(room_id_, json_msg);
									std::string my_id = nim_ui::LoginManager::GetInstance()->GetAccount();
									std::wstring my_name = nim_ui::UserManager::GetInstance()->GetUserName(nim_ui::LoginManager::GetInstance()->GetAccount(), false);
									AddText(nbase::UTF8ToUTF16(content), my_name, my_id, kMember, false);
								}
								else
								{
									assert(0);
									//QLOG_ERR(L"\r\nError custom click: {0}") << (std::wstring)link_info;
								}
							}
							return 1;
						}
					}
				}
			}
		}
		return 0;
	}

}