#pragma once
#include "IOCPCore.h"
#include "SockAddrEx.h"

namespace ServerCore
{
	class CServerService;
	struct FIOCPEvent;

	class CListener final : public CIOCPObject
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CListener() = default;

		virtual ~CListener();

	public:
		bool Accept(std::shared_ptr<CServerService> _spServerService);

		void CloseSocket();
		
	private:
		void RegisterAccept(FIOCPEvent* _pAcceptEvent);

		void ProcessAccept(FIOCPEvent* _pAcceptEvent);
		
	public:
		virtual HANDLE GetHandle() override;

		virtual void Dispatch(FIOCPEvent* _pIOCPEvent, int32 _iNumBytes = 0) override;

	/********************
		Data Members
	********************/
	protected:
		// 리스너 소켓
		SOCKET m_iSocket = INVALID_SOCKET;

		std::vector<FIOCPEvent*> m_vecAcceptEvents;

		std::shared_ptr<CServerService> m_spServerService = nullptr;
	};
}
