#include "local_helper.h"
#include "shared/base.h"
#include "shared/xml_util.h"

namespace nim_comp
{

bool LocalHelper::GetAppLocalVersion( int &version, std::wstring &version_flag )
{
	std::wstring app_ver_path = QPath::GetAppPath();
	app_ver_path.append(L"app_ver.dll");

	TiXmlDocument document;
	if (shared::LoadXmlFromFile(document, app_ver_path))
	{
		TiXmlElement* root = document.RootElement();
		if(root) 
		{
			TiXmlElement* child = root->FirstChildElement();
			if(child)
			{
				std::string ver = child->Attribute("version");
				std::string vf = child->Attribute("version_flag");

				nbase::StringToInt(ver, &version);
				version_flag = nbase::UTF8ToUTF16(vf);

				return true;
			}
		}
	}
	return false;
}
}