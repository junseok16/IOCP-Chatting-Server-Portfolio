#include "pch.h"
#include "Session.h"
#include "SocketUtility.h"
#include "Service.h"
#include "SendBuffer.h"

ServerCore::CSession::CSession()
	: m_oReceiveBuffer(BUFFER_SIZE)
{
	m_iSocket = CSocketUtility::OpenSocket();
}

ServerCore::CSession::~CSession()
{
	CSocketUtility::CloseSocket(m_iSocket);
}

void ServerCore::CSession::Dispatch(FIOCPEvent* _pIOCPEvent, int32 _iNumBytes)
{
	// �̺�Ʈ�� ó���մϴ�.
	switch (_pIOCPEvent->m_eIOCPEventType)
	{
	case EIOCPEventType::CONNECT:	 ProcessConnect();			 break;
	case EIOCPEventType::DISCONNECT: ProcessDisconnect();		 break;
	case EIOCPEventType::RECEIVE:	 ProcessReceive(_iNumBytes); break;
	case EIOCPEventType::SEND:		 ProcessSend(_iNumBytes);	 break;
	default:													 break;
	}
}

HANDLE ServerCore::CSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(m_iSocket);
}

void ServerCore::CSession::Send(std::shared_ptr<CSendBuffer> _spSendBuffer)
{
	bool bSendRegistered = false;

	{
		WRITE_LOCK;
		m_qSendBuffers.push(_spSendBuffer);
		bSendRegistered = (m_bSendRegistered.exchange(true) == false);
	}

	if (bSendRegistered) { RegisterSend(); }	
}

bool ServerCore::CSession::Connect()
{
	return RegisterConnect();
}

void ServerCore::CSession::Disconnect(const WCHAR* _pCause)
{
	if (m_bConnected.exchange(false) == false) { return; }
	
	std::wcout << "Disconnect : " << _pCause << std::endl;

	OnDisconnected(); // ������ �ڵ忡�� ������
	GetService()->ReleaseSession(GetSession());

	RegisterDisconnect();
}

bool ServerCore::CSession::RegisterConnect()
{
	if (IsConnected()) { return false; }
	
	if (GetService()->GetServiceType() != EServiceType::CLIENT) { return false; }

	if (CSocketUtility::SetReuseAddress(m_iSocket, true) == false) { return false; }
	
	if (CSocketUtility::BindAnyAddress(m_iSocket, 0) == false) { return false; }
	
	// ���� �̺�Ʈ�� �ʱ�ȭ�մϴ�.
	m_tConnectEvent.Initialize();
	m_tConnectEvent.m_spOwner = shared_from_this();

	DWORD iNumBytes = 0;
	SOCKADDR_IN tSockAddr = GetService()->GetSockAddrEx().GetSockAddr();

	// ���� �̺�Ʈ�� ����մϴ�.
	if (false == CSocketUtility::pConnectEx
	(
		m_iSocket,
		reinterpret_cast<SOCKADDR*>(&tSockAddr),
		sizeof(tSockAddr), nullptr,
		0,
		&iNumBytes,
		&m_tConnectEvent))
	{
		int32 iErrorCode = ::WSAGetLastError();
		if (iErrorCode != WSA_IO_PENDING)
		{
			m_tConnectEvent.m_spOwner = nullptr;
			return false;
		}
	}

	return true;
}

bool ServerCore::CSession::RegisterDisconnect()
{
	// ���� �̺�Ʈ�� �ʱ�ȭ�մϴ�.
	m_tDisconnectEvent.Initialize();
	m_tDisconnectEvent.m_spOwner = shared_from_this();

	if (false == CSocketUtility::pDisconnectEx(m_iSocket, &m_tDisconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 iErrorCode = ::WSAGetLastError();
		if (iErrorCode != WSA_IO_PENDING)
		{
			m_tDisconnectEvent.m_spOwner = nullptr;
			return false;
		}
	}

	return true;
}

void ServerCore::CSession::RegisterReceive()
{
	if (IsConnected() == false) { return; }
	
	// ���� �̺�Ʈ�� �ʱ�ȭ�մϴ�.
	m_tReceiveEvent.Initialize();
	m_tReceiveEvent.m_spOwner = shared_from_this();

	// ���� ���ۿ��� �����͸� �� �� �ִ� ���� ��ġ�� ����ִ� ���� �뷮�� �����մϴ�.
	WSABUF tWSABuf;
	tWSABuf.buf = reinterpret_cast<char*>(m_oReceiveBuffer.GetWritePosition());
	tWSABuf.len = m_oReceiveBuffer.GetEmptySize();

	DWORD iNumBytes = 0;
	DWORD iFlags = 0;

	// ���� �̺�Ʈ�� ����մϴ�.
	if (SOCKET_ERROR == ::WSARecv(m_iSocket, &tWSABuf, 1, OUT &iNumBytes, OUT &iFlags, &m_tReceiveEvent, nullptr))
	{
		int32 iErrorCode = ::WSAGetLastError();
		if (iErrorCode != WSA_IO_PENDING)
		{
			HandleError(iErrorCode);
			m_tReceiveEvent.m_spOwner = nullptr;
		}
	}
}

void ServerCore::CSession::RegisterSend()
{
	if (!IsConnected()) { return; }

	// �߽� �̺�Ʈ�� �ʱ�ȭ�մϴ�.
	m_tSendEvent.Initialize();
	m_tSendEvent.m_spOwner = shared_from_this();

	// �߽� ���۸� �߽� �̺�Ʈ�� �����մϴ�. 
	{
		WRITE_LOCK;

		int32 iWriteSize = 0;
		while (!m_qSendBuffers.empty())
		{
			std::shared_ptr<CSendBuffer> spSendBuffer = m_qSendBuffers.front();
			iWriteSize += spSendBuffer->GetWriteSize();

			// TODO : ���� üũ

			m_qSendBuffers.pop();
			m_tSendEvent.m_vecSendBuffers.push_back(spSendBuffer);
		}
	}

	// ������ �߽� ���� �ȿ� �ִ� �����͸� �����մϴ�.
	std::vector<WSABUF> vecWSABufs;
	vecWSABufs.reserve(m_tSendEvent.m_vecSendBuffers.size());
	for (std::shared_ptr<CSendBuffer> spSendBuffer : m_tSendEvent.m_vecSendBuffers)
	{
		WSABUF tWSABuf;
		tWSABuf.buf = reinterpret_cast<char*>(spSendBuffer->GetBuffer());
		tWSABuf.len = static_cast<LONG>(spSendBuffer->GetWriteSize());
		vecWSABufs.push_back(tWSABuf);
	}

	// �߽� �̺�Ʈ�� ����մϴ�.
	DWORD iNumBytes = 0;
	if (SOCKET_ERROR == ::WSASend(m_iSocket, vecWSABufs.data(), static_cast<DWORD>(vecWSABufs.size()), OUT &iNumBytes, 0, &m_tSendEvent, nullptr))
	{
		int32 iErrorCode = ::WSAGetLastError();
		if (iErrorCode != WSA_IO_PENDING)
		{
			HandleError(iErrorCode);
			m_tSendEvent.m_spOwner = nullptr;
			m_tSendEvent.m_vecSendBuffers.clear();
			m_bSendRegistered.store(false);
		}
	}
}

void ServerCore::CSession::ProcessConnect()
{
	// ���� �̺�Ʈ�� �����մϴ�.
	m_tConnectEvent.m_spOwner = nullptr;

	m_bConnected.store(true);

	// ������ Ŭ���̾�Ʈ ���񽺳� ���� ���񽺿� ����մϴ�.
	GetService()->AddSession(GetSession());

	OnConnected();

	// ���� �̺�Ʈ�� ����մϴ�.
	RegisterReceive();
}

void ServerCore::CSession::ProcessDisconnect()
{
	// ���� �̺�Ʈ�� �����մϴ�.
	m_tDisconnectEvent.m_spOwner = nullptr;
}

void ServerCore::CSession::ProcessReceive(int32 _iNumBytes)
{
	// ���� �̺�Ʈ�� �����մϴ�.
	m_tReceiveEvent.m_spOwner = nullptr;

	// ���Ź��� �����Ͱ� 0 ����Ʈ�� ���
	if (_iNumBytes == 0)
	{
		Disconnect(L"[����] ������ �����Ͱ� 0 ����Ʈ�Դϴ�.");
		return;
	}

	// ���Ź��� �����͸�ŭ ���� ������ ���� Ŀ���� �̵��մϴ�.
	if (m_oReceiveBuffer.OnWrite(_iNumBytes) == false)
	{
		Disconnect(L"[����] ���� ���ۿ��� ���� �����÷ΰ� �߻��߽��ϴ�.");
		return;
	}

	// ���Ź��� ���ۿ��� �����͸�ŭ ��Ŷ�� �����մϴ�.
	int32 iDataSize = m_oReceiveBuffer.GetReceivedDataSize();
	int32 iProcessLength = OnReceived(m_oReceiveBuffer.GetReadPosition(), iDataSize);

	if (iProcessLength < 0
		|| iDataSize < iProcessLength
		// �����͸�ŭ ���� ������ �б� Ŀ���� �̵��մϴ�.
		|| m_oReceiveBuffer.OnRead(iProcessLength) == false)
	{
		Disconnect(L"[����] ���� ���ۿ��� �б� �����÷ΰ� �߻��߽��ϴ�.");
		return;
	}

	// �б�, ���� Ŀ���� �����մϴ�.
	m_oReceiveBuffer.Clear();

	// ���� �̺�Ʈ�� �����մϴ�.
	RegisterReceive();
}

void ServerCore::CSession::ProcessSend(int32 _iNumBytes)
{
	// �߽� �̺�Ʈ�� �����մϴ�.
	m_tSendEvent.m_spOwner = nullptr;
	m_tSendEvent.m_vecSendBuffers.clear();

	// �߽��� �����Ͱ� 0 ����Ʈ�� ���
	if (_iNumBytes == 0)
	{
		Disconnect(L"[����] �߽��� �����Ͱ� 0 ����Ʈ�Դϴ�.");
		return;
	}

	// ������ �ڵ忡�� ������
	OnSend(_iNumBytes);

	bool bRegisterSend = false;

	{
		WRITE_LOCK;
		if (m_qSendBuffers.empty()) { m_bSendRegistered.store(false); }

		else { bRegisterSend = true; }
	}

	// �߽� �̺�Ʈ�� �����մϴ�.
	if (bRegisterSend) { RegisterSend(); }
}

void ServerCore::CSession::HandleError(int32 _iErrorCode)
{
	switch (_iErrorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"[����] ������ �߻��߽��ϴ�.");
		break;

	default:
		// TODO : Log
		std::cout << "Handle Error : " << _iErrorCode << std::endl;
		break;
	}
}

CPacketSession::CPacketSession()
{
}

CPacketSession::~CPacketSession()
{
}

int32 CPacketSession::OnReceived(BYTE* _pBuffer, int32 _iLength)
{
	int32 iProcessLength = 0;

	while (true)
	{
		int32 iDataSize = _iLength - iProcessLength;

		// ��� ũ�⺸�� ���� ���, ��ٸ��ϴ�.
		if (iDataSize < sizeof(FPacketHeader)) { break; }

		FPacketHeader tHeader = *(reinterpret_cast<FPacketHeader*>(&_pBuffer[iProcessLength]));

		// ��Ŷ ũ�⺸�� ���� ���, ��ٸ��ϴ�.
		if (iDataSize < tHeader.m_iSize) { break; }
		
		// ��Ŷ�� �����մϴ�.
		OnReceivedPacket(&_pBuffer[iProcessLength], tHeader.m_iSize);

		iProcessLength += tHeader.m_iSize;
	}

	return iProcessLength;
}