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
	
	// ������ �����մϴ�.
	m_iSocket = CSocketUtility::OpenSocket();

	if (m_iSocket == INVALID_SOCKET) { return false; }

	// IOCP ť�� ����մϴ�.
	if (m_spServerService->GetIOCPCore()->Register(shared_from_this()) == false) { return false; }

	if (CSocketUtility::SetReuseAddress(m_iSocket, true) == false) { return false; }

	if (CSocketUtility::SetLinger(m_iSocket, 0, 0) == false) { return false; }

	// ������ �������� �����մϴ�.
	if (CSocketUtility::Bind(m_iSocket, m_spServerService->GetSockAddrEx()) == false) { return false; }

	if (CSocketUtility::Listen(m_iSocket) == false) { return false; }
	
	// �ִ� ���� ����ŭ ���� �̺�Ʈ�� �����մϴ�.
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
	// ���� �̺�Ʈ�� ó���մϴ�.
	assert(_pAcceptEvent->m_eIOCPEventType == EIOCPEventType::ACCEPT);
	ProcessAccept(_pAcceptEvent);
}

void CListener::RegisterAccept(FIOCPEvent* _pAcceptEvent)
{
	// ������ �����մϴ�.
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

	// Ŭ���̾�Ʈ ���� ������ �ҷ��ɴϴ�.
	SOCKADDR_IN tSockAddr;
	int32 iSockAddrSize = sizeof(tSockAddr);
	if (SOCKET_ERROR == ::getpeername(spSession->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&tSockAddr), &iSockAddrSize))
	{
		RegisterAccept(_pAcceptEvent);
		return;
	}

	spSession->SetSockAddr(CSockAddrEx(tSockAddr));

	std::cout << "[������] Ŭ���̾�Ʈ�� ����Ǿ����ϴ�." << std::endl;

	// ���� �̺�Ʈ�� ó���մϴ�.
	spSession->ProcessConnect();

	// �ٽ� ���� �̺�Ʈ�� ����մϴ�.
	RegisterAccept(_pAcceptEvent);
}