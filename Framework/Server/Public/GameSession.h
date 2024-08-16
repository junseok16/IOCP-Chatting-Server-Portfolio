#pragma once
#include "Session.h"

namespace Server
{
	class CGameSession : public CPacketSession
	{
	public:
		explicit CGameSession() = default;

		virtual ~CGameSession() { std::cout << "~CGameSession" << std::endl; }
		
	public:
		virtual void OnConnected() override;

		virtual void OnDisconnected() override;

		virtual void OnReceivedPacket(BYTE* _pBuffer, int32 _iLength) override;

		virtual void OnSend(int32 _iLength) override;
	};
}
