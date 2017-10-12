#include "bubble_robot.h"
#include "module/emoji/emoji_info.h"
#include "module/session/session_manager.h"
#include "gui/session/msg_record.h"

using namespace ui;

namespace nim_comp
{
void MsgBubbleRobot::InitControl(bool bubble_right)
{
	__super::InitControl(bubble_right);
	
	msg_text_ = new Box;
	if(bubble_right)
		GlobalManager::FillBoxWithCache(msg_text_, L"session/robot_right.xml");
	else
		GlobalManager::FillBoxWithCache(msg_text_, L"session/robot_left.xml");
	bubble_box_->Add(msg_text_);

	text_ = (RichEdit*) msg_text_->FindSubControl(L"robot_text");

	ITextServices* text_services = text_->GetTextServices();
	IRichEditOleCallbackEx* richedit_cb = new IRichEditOleCallbackEx( text_services, std::function<void()>() );
	text_services->Release();
	text_->SetOleCallback( richedit_cb );
	text_->SetEventMask(ENM_LINK);
	text_->SetAutoURLDetect(true);
	text_->AttachMenu(nbase::Bind(&MsgBubbleRobot::OnMenu, this, std::placeholders::_1));
	text_->SetRich(true);
	text_->SetAttribute(L"align", L"hcenter");
}

void MsgBubbleRobot::InsertImage(const std::string &url, const std::wstring &file_tag/* = L""*/)
{
	std::wstring photo_path = PhotoService::GetInstance()->GetPhotoDir(kOther) + nbase::UTF8ToUTF16(QString::GetMd5(url));
	if (PhotoService::GetInstance()->CheckPhotoOK(photo_path))
	{
		ITextServices *text_service = text_->GetTextServices();
		if (text_service)
		{
			Re_InsertImage(text_service, InsertCustomItemErrorCallback(), false, photo_path, file_tag, false);
			text_service->Release();
			//InsertTextToEdit(text_, L"\r\n");
			return;
		}
	}
	ITextServices *text_service = text_->GetTextServices();
	if (text_service)
	{
		Re_InsertImage(text_service, InsertCustomItemErrorCallback(), false, photo_path, file_tag, true);
		text_service->Release();
		//InsertTextToEdit(text_, L"\r\n");
		DownloadImage(url, photo_path, false);
	}
}

void MsgBubbleRobot::DownloadImage(const std::string &url, const std::wstring &photo_path, bool is_complex_element)
{
	std::string session_id = msg_.local_talk_id_;
	std::string msg_id = msg_.client_msg_id_;
	nim::NOS::DownloadMediaCallback cb = ToWeakCallback([this, photo_path, session_id, msg_id, is_complex_element](int res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id) {
		if (res_code == nim::kNIMResSuccess)
		{
			std::wstring ws_file_path = nbase::UTF8ToUTF16(file_path);
			if (nbase::FilePathIsExist(ws_file_path, false))
			{
				nbase::CopyFileW(ws_file_path, photo_path);
				nbase::DeleteFile(ws_file_path);
			}
			std::string s_photo_path = nbase::UTF16ToUTF8(photo_path);
			OnDownloadCallbackEx(res_code == nim::kNIMResSuccess, s_photo_path, is_complex_element);
// 			SessionBox* session_box = dynamic_cast<SessionBox*>(SessionManager::GetInstance()->FindSessionBox(session_id));
// 			if (session_box)
// 			{
// 				session_box->OnDownloadCallback(msg_id, res_code == nim::kNIMResSuccess, s_photo_path);
// 			}
// 
// 			MsgRecordForm* msg_record = dynamic_cast<MsgRecordForm*>(WindowsManager::GetInstance()->GetWindow(MsgRecordForm::kClassName, MsgRecordForm::kClassName));
// 			if (msg_record)
// 			{
// 				msg_record->OnDownloadCallback((nim::NIMResCode)res_code, s_photo_path, session_id, msg_id);
// 			}
		}
	});
	nim::NOS::DownloadResource(url, cb);
}

void MsgBubbleRobot::SetShowHeader()
{
	if (robot_attach_.out_msg_)
		msg_header_button_->SetBkImage(PhotoService::GetInstance()->GetUserPhoto(robot_attach_.robot_accid_, true));
	else
		__super::SetShowHeader();
}

void MsgBubbleRobot::SetShowName(bool show, const std::string& from_nick)
{
	std::string nick = from_nick;
	if (robot_attach_.out_msg_)
	{
		nim::RobotInfo info;
		UserService::GetInstance()->GetRobotInfo(robot_attach_.robot_accid_, info);
		nick = info.GetName();
	}
	__super::SetShowName(show, nick);
}

std::string MsgBubbleRobot::GetSenderId()
{
	if (robot_attach_.out_msg_)
		return robot_attach_.robot_accid_;
	else
		return __super::GetSenderId();
}

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

void MsgBubbleRobot::InitInfo(const nim::IMMessage &msg)
{
	std::wstring str;
	nim::Talk::ParseBotRobotMessageAttach(msg, robot_attach_);

	__super::InitInfo(msg);

	if (!robot_attach_.out_msg_)
	{//客户端发给机器人
		std::wstring str = nbase::UTF8ToUTF16(msg.content_);
		SetMsgText(str);
		return;
	}

	Json::Value values;
	Json::Reader reader;
	if (reader.parse(msg.attach_, values) && values.isObject())
	{
		QLOG_APP(L"body:{0}\r\nattach:{1}") << nbase::UTF8ToUTF16(msg.content_) << nbase::UTF8ToUTF16(values.toStyledString());
	}

	if (robot_attach_.robot_msg_flag_ == "faq")
	{
		//answer_type定义
		//1能匹配到问答对库中一个相似度高的问题;
		//2能匹配到问答对库中若干个相似度高的问题;
		//3能匹配到问答对库中若干个相似度较低的问题;
		//4不能匹配问题;
		//5能匹配到寒暄库中一个相似度高的问题
		std::string answer_type = robot_attach_.robot_msg_content_["answer_type"].asString();
		std::string query = robot_attach_.robot_msg_content_["query"].asString();
		Json::Value match_answers = robot_attach_.robot_msg_content_["match"];
		int len = match_answers.size();
		std::list<Json::Value> lst;
		for (int i = 0; i < len; i++)
		{
			lst.push_back(match_answers[i]);
		}
		lst.sort(ScoreCompare());
		if (lst.size() > 0)
		{
			InsertTextToEdit(text_, nbase::UTF8ToUTF16((*lst.begin())["answer"].asString()));
		}
	}
	else if (robot_attach_.robot_msg_flag_ == "bot")
	{
		Json::Value msgs = robot_attach_.robot_msg_content_;
		int len = (int)msgs.size();
		for (int i = 0; i < len; i++)
		{
			if (i > 0)
				InsertTextToEdit(text_, L"\r\n");
			std::string msg_type = msgs[i]["type"].asString();
			if (msg_type == "01" || msg_type == "03")//01文字 03快速回复
			{
				InsertTextToEdit(text_, nbase::UTF8ToUTF16(msgs[i]["content"].asString()));
			}
			else if (msg_type == "02")//图片
			{
				std::string url = msgs[i]["content"].asString();
				InsertImage(url);
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
						if (!first_layout)
							InsertTextToEdit(text_, L"\r\n");
						auto node = linear_layout->FirstChild();
						bool first_node = true;
						while (node)
						{
							if (!first_node)
								InsertTextToEdit(text_, L"\r\n");
							if (node->ValueStr() == "text")
							{
								auto val = node->ToElement()->GetText();
								if (val)
								{
									InsertTextToEdit(text_, nbase::UTF8ToUTF16((std::string)val));
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
								values[nim::kNIMBotRobotMsgKeyRobotID] = robot_attach_.robot_accid_;
								std::string show_text;
								bool need_download = false;
								std::list<std::string> need_download_urls;
								Json::Value link_json;
								link_json["id"] = values["target"];
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
											//values["link_text"] = "[模板触发消息]";
											std::string url = (std::string)val;
											//InsertImage(url);
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
											img["width"] = 200;
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
									ITextServices *text_service = text_->GetTextServices();
									if (text_service)
									{
										Re_InsertImage(text_service, InsertCustomItemErrorCallback(), false, L"", nbase::UTF8ToUTF16(link_json["id"].asString()), true);
										text_->ReplaceSel(L" ", false);//每个button后面增加一个空格
										text_service->Release();
									}
									for (auto &url : need_download_urls)
									{
										std::wstring photo_path = PhotoService::GetInstance()->GetPhotoDir(kOther) + nbase::UTF8ToUTF16(QString::GetMd5(url));
										DownloadImage(url, photo_path, true);
									}
								}
								else
								{
									ITextServices *service = text_->GetTextServices();
									if (service)
									{
										auto old_len = text_->GetTextLength();
										Re_InsertDescriptionItem(service
											, InsertCustomItemErrorCallback()
											, false
											, nbase::UTF8ToUTF16(link_json["element"].toStyledString())
											, nbase::UTF8ToUTF16(link_json["id"].asString())
											, RE_OLE_TYPE_DESCRIPTION
											, my_msg_ ? RGB(35, 142, 250) : RGB(225,230,235));
										service->Release();
										text_->ReplaceSel(L" ", false);//每个button后面增加一个空格
										auto new_len = text_->GetTextLength();
										text_->SetSel(old_len, new_len);
										CHARRANGE cr;
										text_->GetSel(cr);
										text_->AddLinkInfo(cr, nbase::UTF8ToUTF16(values.toStyledString()));
										text_->SetSelNone();
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
		SetMsgText(nbase::UTF8ToUTF16(msg.attach_));
	}
}

void MsgBubbleRobot::SetMsgText( const std::wstring &str )
{
	if(str.empty())
	{
		text_->SetFixedWidth(10);
		text_->SetFixedHeight(20);
	}
	else
	{
		InsertTextToEdit(text_, str);
	}
}

ui::CSize MsgBubbleRobot::EstimateSize(ui::CSize szAvailable)
{
	if (!robot_attach_.out_msg_ && msg_.content_.empty())
		return Box::EstimateSize(szAvailable);

	int sub_width = 200;
	int width = szAvailable.cx - DpiManager::GetInstance()->ScaleInt(sub_width);

	ui::CSize sz = text_->GetNaturalSize(width, 0);
	text_->SetFixedWidth(sz.cx, true, false);
	text_->SetFixedHeight(sz.cy, false);

	return Box::EstimateSize(szAvailable);
}

bool MsgBubbleRobot::OnMenu( ui::EventArgs* arg )
{
	bool can_recall = (msg_.session_type_ != nim::kNIMSessionTypeP2P) || (msg_.receiver_accid_ != robot_attach_.robot_accid_);
	bool can_retweet = !robot_attach_.out_msg_;
	PopupMenu(!robot_attach_.out_msg_, can_recall, can_retweet);
	return false;
}

void MsgBubbleRobot::OnMenuCopy()
{
	long start_pos = 0, end_pos = 0;
	text_->GetSel( start_pos, end_pos );
	if( start_pos == end_pos )
	{
		text_->SetSelAll();
		text_->Copy();
		text_->SetSelNone();
	}
	else
	{
		text_->Copy();
	}
}

void MsgBubbleRobot::OnDownloadCallbackEx(bool success, const std::string& file_path, bool is_complex_element)
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
				ITextServices *service = text_->GetTextServices();
				if (service)
				{
					std::string id = complex_json_[i]["id"].asString();
					long old_end = (long)Re_RemoveCustomItem(service, nbase::UTF8ToUTF16(id));
					if (old_end >= 0)
					{
						text_->SetSel(old_end, old_end);
						Re_InsertDescriptionItem(service
							, InsertCustomItemErrorCallback()
							, false
							, nbase::UTF8ToUTF16(complex_json_[i]["element"].toStyledString())
							, nbase::UTF8ToUTF16(id)
							, RE_OLE_TYPE_DESCRIPTION
							, my_msg_ ? RGB(35, 142, 250) : RGB(225, 230, 235));
						service->Release();
						long new_begin = 0, new_end = 0;
						text_->GetSel(new_begin, new_end);
						text_->SetSel(old_end, new_end + 1);//每个button后面有一个空格
						CHARRANGE cr;
						text_->GetSel(cr);
						text_->AddLinkInfo(cr, nbase::UTF8ToUTF16(complex_json_[i]["link_info"].toStyledString()));
						text_->SetSelNone();
						ui::CSize max_size(9999, 9999);
						EstimateSize(max_size);
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
		ITextServices *service = text_->GetTextServices();
		if (service)
		{
			Re_ImageLoadingFinish(service, nbase::UTF8ToUTF16(file_path), success, false);
			service->Release();
			ui::CSize max_size(9999, 9999);
			EstimateSize(max_size);
		}
	}
}

}