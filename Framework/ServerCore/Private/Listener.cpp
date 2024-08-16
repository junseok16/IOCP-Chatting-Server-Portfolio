#include "pch.h"
#include "Listener.h"

#include "IOCPEvent.h"
#include "Session.h"
#include "Service.h"
#include "SocketUtility.h"

CListener::~CListener()
{
	CSocketUtility::CloseSocket(m_iSocket);

	for (auto& pAcceptEvent : m_vecAcceptEvents)
	{
		// TODO;
		delete pAcceptEvent;
		pAcceptEvent = nullptr;
	}
}

bool CListener::Accept(std::shared_ptr<CServerService> _spServerService)
{
	if (_spServerService == nullptr) { return false; }
		
	m_spServerService = _spServerService;
	
	// 소켓을 생성합니다.
	m_iSocket = CSocketUtility::OpenSocket();

	if (m_iSocket == INVALID_SOCKET) { return false; }

	// IOCP 큐에 등록합니다.
	if (m_spServerService->GetIOCPCore()->Register(shared_from_this()) == false) { return false; }

	if (CSocketUtility::SetReuseAddress(m_iSocket, true) == false) { return false; }

	if (CSocketUtility::SetLinger(m_iSocket, 0, 0) == false) { return false; }

	// 리스너 소켓으로 설정합니다.
	if (CSocketUtility::Bind(m_iSocket, m_spServerService->GetSockAddrEx()) == false) { return false; }

	if (CSocketUtility::Listen(m_iSocket) == false) { return false; }
	
	// 최대 세션 수만큼 수락 이벤트를 생성합니다.
	const int32 iMaxNumSessions = m_spServerService->GetMaxNumSessions();
	for (int32 iAcceptIndex = 0; iAcceptIndex < iMaxNumSessions; ++iAcceptIndex)
	{
		FIOCPEvent* pAcceptEvent = new FIOCPEvent(EIOCPEventType::ACCEPT);
		pAcceptEvent->m_spOwner = shared_from_this();

		m_vecAcceptEvents.push_back(pAcceptEvent);
		RegisterAccept(pAcceptEvent);
	}

	return true;
}

void CListener::CloseSocket()
{
	CSocketUtility::CloseSocket(m_iSocket);
}

HANDLE CListener::GetHandle()
{
	return reinterpret_cast<HANDLE>(m_iSocket);
}

void CListener::Dispatch(FIOCPEvent* _pAcceptEvent, int32 _iNumBytes)
{
	// 수락 이벤트를 처리합니다.
	assert(_pAcceptEvent->m_eIOCPEventType == EIOCPEventType::ACCEPT);
	ProcessAccept(_pAcceptEvent);
}

void CListener::RegisterAccept(FIOCPEvent* _pAcceptEvent)
{
	// 세션을 생성합니다.
	std::shared_ptr<CSession> spSession = m_spServerService->CreateSession();

	_pAcceptEvent->Initialize();
	_pAcceptEvent->m_spSession = spSession;

	DWORD iReceivedBytes = 0;
	if (false == CSocketUtility::pAcceptEx
	(
		m_iSocket,
		spSession->GetSocket(),
		spSession->m_oReceiveBuffer.GetWritePosition(),
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		OUT &iReceivedBytes,
		static_cast<LPOVERLAPPED>(_pAcceptEvent)
	))
	{
		const int32 iErrorCode = ::WSAGetLastError();
		if (iErrorCode != WSA_IO_PENDING)
		{
			RegisterAccept(_pAcceptEvent);
		}
	}
}

void CListener::ProcessAccept(FIOCPEvent* _pAcceptEvent)
{
	std::shared_ptr<CSession> spSession = _pAcceptEvent->m_spSession;

	if (false == CSocketUtility::SetUpdateAcceptSocket(spSession->GetSocket(), m_iSocket))
	{
		RegisterAccept(_pAcceptEvent);
		return;
	}

	// 클라이언트 소켓 정보를 불러옵니다.
	SOCKADDR_IN tSockAddr;
	int32 iSockAddrSize = sizeof(tSockAddr);
	if (SOCKET_ERROR == ::getpeername(spSession->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&tSockAddr), &iSockAddrSize))
	{
		RegisterAccept(_pAcceptEvent);
		return;
	}

	spSession->SetSockAddr(CSockAddrEx(tSockAddr));

	std::cout << "[리스너] 클라이언트와 연결되었습니다." << std::endl;

	// 연결 이벤트를 처리합니다.
	spSession->ProcessConnect();

	// 다시 수락 이벤트를 등록합니다.
	RegisterAccept(_pAcceptEvent);
}