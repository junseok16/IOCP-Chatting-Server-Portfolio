#include "pch.h"
#include "IOCPCore.h"

#include "IOCPEvent.h"

ServerCore::CIOCPCore::CIOCPCore()
{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	assert(m_hIOCP != INVALID_HANDLE_VALUE);
}

ServerCore::CIOCPCore::~CIOCPCore()
{
	::CloseHandle(m_hIOCP);
}

bool ServerCore::CIOCPCore::Register(std::shared_ptr<CIOCPObject> _spIOCPObject)
{
	return ::CreateIoCompletionPort
	(
		_spIOCPObject->GetHandle(),
		m_hIOCP,
		0,
		0
	);
}

bool ServerCore::CIOCPCore::Dispatch(uint32 _iTimeOut)
{
	DWORD iNumBytes = 0;
	ULONG_PTR llKey = 0;
	FIOCPEvent* pIOCPEvent = nullptr;

	// IOCP 큐에 작업이 요청된 경우
	if (::GetQueuedCompletionStatus
	(
		m_hIOCP,
		OUT &iNumBytes,
		// 큐에 등록된 소켓(리스너 소켓 혹은 세션 소켓)입니다.
		OUT reinterpret_cast<PULONG_PTR>(&llKey),
		// 요청받은 이벤트(ACCEPT, CONNECT, RECEIVE, SEND, ...)
		OUT reinterpret_cast<LPOVERLAPPED*>(&pIOCPEvent),
		_iTimeOut
	))
	{
		// 리스너 소켓인 경우, 수락 이벤트를 처리합니다.
		// 세션 소켓인 경우, 그외 이벤트를 처리합니다.
		std::shared_ptr<CIOCPObject> spIOCPObject = pIOCPEvent->m_spOwner;
		spIOCPObject->Dispatch(pIOCPEvent, iNumBytes);
	}
	// IOCP 큐에 오류가 발생한 경우
	else
	{
		int32 iErrCode = ::WSAGetLastError();
		switch (iErrCode)
		{
		case WAIT_TIMEOUT:
			return false;

		default:
			std::shared_ptr<CIOCPObject> spIOCPObject = pIOCPEvent->m_spOwner;
			spIOCPObject->Dispatch(pIOCPEvent, iNumBytes);
			break;
		}
	}

	return true;
}
