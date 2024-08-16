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
	// 이벤트를 처리합니다.
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

	OnDisconnected(); // 컨텐츠 코드에서 재정의
	GetService()->ReleaseSession(GetSession());

	RegisterDisconnect();
}

bool ServerCore::CSession::RegisterConnect()
{
	if (IsConnected()) { return false; }
	
	if (GetService()->GetServiceType() != EServiceType::CLIENT) { return false; }

	if (CSocketUtility::SetReuseAddress(m_iSocket, true) == false) { return false; }
	
	if (CSocketUtility::BindAnyAddress(m_iSocket, 0) == false) { return false; }
	
	// 연결 이벤트를 초기화합니다.
	m_tConnectEvent.Initialize();
	m_tConnectEvent.m_spOwner = shared_from_this();

	DWORD iNumBytes = 0;
	SOCKADDR_IN tSockAddr = GetService()->GetSockAddrEx().GetSockAddr();

	// 연결 이벤트를 등록합니다.
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
	// 해제 이벤트를 초기화합니다.
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
	
	// 수신 이벤트를 초기화합니다.
	m_tReceiveEvent.Initialize();
	m_tReceiveEvent.m_spOwner = shared_from_this();

	// 수신 버퍼에서 데이터를 쓸 수 있는 시작 위치와 비어있는 버퍼 용량을 저장합니다.
	WSABUF tWSABuf;
	tWSABuf.buf = reinterpret_cast<char*>(m_oReceiveBuffer.GetWritePosition());
	tWSABuf.len = m_oReceiveBuffer.GetEmptySize();

	DWORD iNumBytes = 0;
	DWORD iFlags = 0;

	// 수신 이벤트를 등록합니다.
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

	// 발신 이벤트를 초기화합니다.
	m_tSendEvent.Initialize();
	m_tSendEvent.m_spOwner = shared_from_this();

	// 발신 버퍼를 발신 이벤트에 누적합니다. 
	{
		WRITE_LOCK;

		int32 iWriteSize = 0;
		while (!m_qSendBuffers.empty())
		{
			std::shared_ptr<CSendBuffer> spSendBuffer = m_qSendBuffers.front();
			iWriteSize += spSendBuffer->GetWriteSize();

			// TODO : 예외 체크

			m_qSendBuffers.pop();
			m_tSendEvent.m_vecSendBuffers.push_back(spSendBuffer);
		}
	}

	// 누적된 발신 버퍼 안에 있는 데이터를 취합합니다.
	std::vector<WSABUF> vecWSABufs;
	vecWSABufs.reserve(m_tSendEvent.m_vecSendBuffers.size());
	for (std::shared_ptr<CSendBuffer> spSendBuffer : m_tSendEvent.m_vecSendBuffers)
	{
		WSABUF tWSABuf;
		tWSABuf.buf = reinterpret_cast<char*>(spSendBuffer->GetBuffer());
		tWSABuf.len = static_cast<LONG>(spSendBuffer->GetWriteSize());
		vecWSABufs.push_back(tWSABuf);
	}

	// 발신 이벤트를 등록합니다.
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
	// 연결 이벤트를 정리합니다.
	m_tConnectEvent.m_spOwner = nullptr;

	m_bConnected.store(true);

	// 세션을 클라이언트 서비스나 서버 서비스에 등록합니다.
	GetService()->AddSession(GetSession());

	OnConnected();

	// 수신 이벤트를 등록합니다.
	RegisterReceive();
}

void ServerCore::CSession::ProcessDisconnect()
{
	// 해제 이벤트를 정리합니다.
	m_tDisconnectEvent.m_spOwner = nullptr;
}

void ServerCore::CSession::ProcessReceive(int32 _iNumBytes)
{
	// 수신 이벤트를 정리합니다.
	m_tReceiveEvent.m_spOwner = nullptr;

	// 수신받은 데이터가 0 바이트인 경우
	if (_iNumBytes == 0)
	{
		Disconnect(L"[세션] 수신한 데이터가 0 바이트입니다.");
		return;
	}

	// 수신받은 데이터만큼 수신 버퍼의 쓰기 커서를 이동합니다.
	if (m_oReceiveBuffer.OnWrite(_iNumBytes) == false)
	{
		Disconnect(L"[세션] 수신 버퍼에서 쓰기 오버플로가 발생했습니다.");
		return;
	}

	// 수신받은 버퍼에서 데이터만큼 패킷을 조립합니다.
	int32 iDataSize = m_oReceiveBuffer.GetReceivedDataSize();
	int32 iProcessLength = OnReceived(m_oReceiveBuffer.GetReadPosition(), iDataSize);

	if (iProcessLength < 0
		|| iDataSize < iProcessLength
		// 데이터만큼 수신 버퍼의 읽기 커서를 이동합니다.
		|| m_oReceiveBuffer.OnRead(iProcessLength) == false)
	{
		Disconnect(L"[세션] 수신 버퍼에서 읽기 오버플로가 발생했습니다.");
		return;
	}

	// 읽기, 쓰기 커서를 정리합니다.
	m_oReceiveBuffer.Clear();

	// 수신 이벤트를 재등록합니다.
	RegisterReceive();
}

void ServerCore::CSession::ProcessSend(int32 _iNumBytes)
{
	// 발신 이벤트를 정리합니다.
	m_tSendEvent.m_spOwner = nullptr;
	m_tSendEvent.m_vecSendBuffers.clear();

	// 발신한 데이터가 0 바이트인 경우
	if (_iNumBytes == 0)
	{
		Disconnect(L"[세션] 발신한 데이터가 0 바이트입니다.");
		return;
	}

	// 컨텐츠 코드에서 재정의
	OnSend(_iNumBytes);

	bool bRegisterSend = false;

	{
		WRITE_LOCK;
		if (m_qSendBuffers.empty()) { m_bSendRegistered.store(false); }

		else { bRegisterSend = true; }
	}

	// 발신 이벤트를 재등록합니다.
	if (bRegisterSend) { RegisterSend(); }
}

void ServerCore::CSession::HandleError(int32 _iErrorCode)
{
	switch (_iErrorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"[세션] 오류가 발생했습니다.");
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

		// 헤더 크기보다 작은 경우, 기다립니다.
		if (iDataSize < sizeof(FPacketHeader)) { break; }

		FPacketHeader tHeader = *(reinterpret_cast<FPacketHeader*>(&_pBuffer[iProcessLength]));

		// 패킷 크기보다 작은 경우, 기다립니다.
		if (iDataSize < tHeader.m_iSize) { break; }
		
		// 패킷을 조립합니다.
		OnReceivedPacket(&_pBuffer[iProcessLength], tHeader.m_iSize);

		iProcessLength += tHeader.m_iSize;
	}

	return iProcessLength;
}