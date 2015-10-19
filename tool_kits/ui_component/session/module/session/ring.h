#pragma once
#include <Vfw.h>

enum RingType
{
	RING_NEW_MESSAGE = 0,
	RING_VOIP_CONNTETING,
	RING_VOIP_NORESPONSE,
	RING_VOIP_PEERBUSY,
	RING_VOIP_PEERREJECT,
	RING_VOIP_RING,
};

namespace nim_comp
{
	class Ring
	{
	public:
		Ring();
		virtual ~Ring();
		bool Init(HWND hwnd);
		void Play(RingType type, bool replay = false);
		void Replay();
		void Stop();
		LRESULT Notify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		void Destroy();
	private:
		HWND hwnd_;
		bool replay_;
	};
}