// dllmain.h : Declaration of module class.

class Cimage_oleModule : public ATL::CAtlDllModuleT< Cimage_oleModule >
{
public :
	DECLARE_LIBID(LIBID_image_oleLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IMAGE_OLE, "{8E5865F0-EF94-4C6F-AB27-EBF5CAE15257}")
};

extern class Cimage_oleModule _AtlModule;
