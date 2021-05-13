#include "batp.h"
namespace nbase
{
	uint64_t CalcBatpSN()
	{
		static std::atomic_uint64_t _sn = (BATP_NOTIFY_SN + 1)/*0xFFFFFFFFFFFFFFFF*/;
		return ++_sn;
	}
}