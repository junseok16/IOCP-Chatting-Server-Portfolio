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

	// IOCP ť�� �۾��� ��û�� ���
	if (::GetQueuedCompletionStatus
	(
		m_hIOCP,
		OUT &iNumBytes,
		// ť�� ��ϵ� ����(������ ���� Ȥ�� ���� ����)�Դϴ�.
		OUT reinterpret_cast<PULONG_PTR>(&llKey),
		// ��û���� �̺�Ʈ(ACCEPT, CONNECT, RECEIVE, SEND, ...)
		OUT reinterpret_cast<LPOVERLAPPED*>(&pIOCPEvent),
		_iTimeOut
	))
	{
		// ������ ������ ���, ���� �̺�Ʈ�� ó���մϴ�.
		// ���� ������ ���, �׿� �̺�Ʈ�� ó���մϴ�.
		std::shared_ptr<CIOCPObject> spIOCPObject = pIOCPEvent->m_spOwner;
		spIOCPObject->Dispatch(pIOCPEvent, iNumBytes);
	}
	// IOCP ť�� ������ �߻��� ���
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
