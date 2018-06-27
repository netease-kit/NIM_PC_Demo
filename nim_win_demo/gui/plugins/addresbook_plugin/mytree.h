#pragma once
#include "gui/extern_ctrl/treectrl/treeview.h"
#include "gui/extern_ctrl/treectrl/treeitem.h"
#include "gui/extern_ctrl/treectrl/treeitem_uistyle.h"
class MyAddresbookItemInfo
{
public:
	int type_;//0 部门 1 联系人
	std::string id_;
	std::string name_;
};
class MyAddresbookDepartmentInfo : public MyAddresbookItemInfo
{

};
class MyAddresbookContactInfo : public MyAddresbookItemInfo
{
public:
	std::string moblie_;
	std::string mail_;
	std::string employeesa_;
	std::string head_image_path_;
};
class MyAddressbookDepartment : virtual public TreeItem<MyAddresbookDepartmentInfo>
{
public:
	virtual std::string OnGetIUIStyleName() const override;
};
class MyAddressbookContact : virtual public TreeItem<MyAddresbookContactInfo>
{
public:
	virtual std::string OnGetIUIStyleName() const override;
};
class MyAddresbookDepartmentUI : public ITreeItemUIStyle
{
public:
	MyAddresbookDepartmentUI() = default;
	virtual ~MyAddresbookDepartmentUI() = default;
public:
	virtual int GetHeight();
	virtual void OnFill() override;
};
class MyAddressbookContactUI : public ITreeItemUIStyle
{
public:
	MyAddressbookContactUI() = default;
	virtual ~MyAddressbookContactUI() = default;
public:
	virtual int GetHeight();
	virtual void OnFill() override;
};