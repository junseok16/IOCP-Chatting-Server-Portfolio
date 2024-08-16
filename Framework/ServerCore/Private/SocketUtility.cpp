#include "pch.h"
#include "SocketUtility.h"

LPFN_CONNECTEX ServerCore::CSocketUtility::pConnectEx = nullptr;
LPFN_DISCONNECTEX ServerCore::CSocketUtility::pDisconnectEx = nullptr;
LPFN_ACCEPTEX ServerCore::CSocketUtility::pAcceptEx = nullptr;

void ServerCore::CSocketUtility::Initialize()
{
	WSADATA tWSAData;
	assert(::WSAStartup(MAKEWORD(2, 2), &tWSAData) == 0);

	SOCKET iDummySocket = OpenSocket();
	assert(BindWindowsFunction(iDummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&pConnectEx)));
	assert(BindWindowsFunction(iDummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&pDisconnectEx)));
	assert(BindWindowsFunction(iDummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&pAcceptEx)));

	CloseSocket(iDummySocket);
}

void ServerCore::CSocketUtility::Release()
{
	::WSACleanup();
}

bool ServerCore::CSocketUtility::BindWindowsFunction(SOCKET _iSocket, GUID _tGUID, LPVOID* _pFn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(_iSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &_tGUID, sizeof(_tGUID), _pFn, sizeof(*_pFn), &bytes, NULL, NULL);
}

SOCKET ServerCore::CSocketUtility::OpenSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool ServerCore::CSocketUtility::SetLinger(SOCKET _iSocket, uint16 _iOnOff, uint16 _iLinger)
{
	LINGER tOptionValue;
	tOptionValue.l_onoff = _iOnOff;
	tOptionValue.l_linger = _iLinger;
	return SetSocketOption(_iSocket, SOL_SOCKET, SO_LINGER, tOptionValue);
}

bool ServerCore::CSocketUtility::SetReuseAddress(SOCKET _iSocket, bool _bFlag)
{
	return SetSocketOption(_iSocket, SOL_SOCKET, SO_REUSEADDR, _bFlag);
}

bool ServerCore::CSocketUtility::SetRecvBufferSize(SOCKET _iSocket, int32 _iSize)
{
	return SetSocketOption(_iSocket, SOL_SOCKET, SO_RCVBUF, _iSize);
}

bool ServerCore::CSocketUtility::SetSendBufferSize(SOCKET _iSocket, int32 _iSize)
{
	return SetSocketOption(_iSocket, SOL_SOCKET, SO_SNDBUF, _iSize);
}

bool ServerCore::CSocketUtility::SetTCPNoDelay(SOCKET _iSocket, bool _bFlag)
{
	return SetSocketOption(_iSocket, SOL_SOCKET, TCP_NODELAY, _bFlag);
}

bool ServerCore::CSocketUtility::SetUpdateAcceptSocket(SOCKET _iSocket, SOCKET _iListenSocket)
{
	return SetSocketOption(_iSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, _iListenSocket);
}

bool ServerCore::CSocketUtility::Bind(SOCKET _iSocket, CSockAddrEx _oSockAddr)
{
	return SOCKET_ERROR != ::bind(_iSocket, reinterpret_cast<const SOCKADDR*>(&_oSockAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool ServerCore::CSocketUtility::Bind(SOCKET _iSocket, SOCKADDR_IN _tSockAddr)
{
	return SOCKET_ERROR != ::bind(_iSocket, (SOCKADDR*)&_tSockAddr, sizeof(SOCKADDR_IN));
}

bool ServerCore::CSocketUtility::BindAnyAddress(SOCKET _iSocket, uint16 _iPort)
{
	SOCKADDR_IN tSockAddr;
	tSockAddr.sin_family = AF_INET;
	tSockAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	tSockAddr.sin_port = ::htons(_iPort);

	return SOCKET_ERROR != ::bind(_iSocket, reinterpret_cast<const SOCKADDR*>(&tSockAddr), sizeof(tSockAddr));
}

bool ServerCore::CSocketUtility::Listen(SOCKET _iSocket, int32 _iBackLog)
{
	return SOCKET_ERROR != ::listen(_iSocket, _iBackLog);
}

void ServerCore::CSocketUtility::CloseSocket(SOCKET& _iSocket)
{
	if (_iSocket != INVALID_SOCKET)
	{
		::closesocket(_iSocket);
	}
		
	_iSocket = INVALID_SOCKET;
}