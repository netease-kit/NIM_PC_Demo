#include "test_gif_form.h"


using namespace ui;

const LPCTSTR TestGifForm::kClassName = L"TestGifForm";

TestGifForm::TestGifForm()
{

}

TestGifForm::~TestGifForm()
{

}

std::wstring TestGifForm::GetSkinFolder()
{
	return L"test_gif_form";
}

std::wstring TestGifForm::GetSkinFile()
{
	return L"test_gif_form.xml";
}

std::wstring TestGifForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring TestGifForm::GetWindowId() const
{
	return kClassName;
}

UINT TestGifForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void TestGifForm::InitWindow()
{
	auto set = dynamic_cast<ui::Button*>(FindControl(L"set"));
	set->AttachClick([this, set](ui::EventArgs* param){
		int count;
		auto seek = dynamic_cast<ui::RichEdit*>(FindControl(L"seek"));		
		nbase::StringToInt(seek->GetText(), &count);		
		auto gif = FindControl(L"gif");
		gif->StartGifPlayForUI(0, count);
		return true;
	});
	auto gif = FindControl(L"gif");
	gif->AttachGifPlayStop([this](ui::EventArgs* param){
		ShowMsgBox(GetHWND(), MsgboxCallback(), L"≤•∑≈ÕÍ≥…!", false);
		return true;
	});
	auto button = dynamic_cast<ui::CheckBox*>(FindControl(L"button"));
	button->AttachSelect([this](ui::EventArgs*){
		auto gif = FindControl(L"gif");
		gif->SetVisible(true);
		return true;
	});
	button->AttachUnSelect([this](ui::EventArgs*){
		auto gif = FindControl(L"gif");
		gif->SetVisible(false);
		return true;
	});
	button->Selected(true);
}