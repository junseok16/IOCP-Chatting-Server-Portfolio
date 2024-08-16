#pragma once
#include "SockAddrEx.h"

namespace ServerCore
{
	class CSocketUtility final
	{
	public:
		static LPFN_CONNECTEX pConnectEx;
		
		static LPFN_DISCONNECTEX pDisconnectEx;
		
		static LPFN_ACCEPTEX pAcceptEx;

	public:
		static void Initialize();

		static void Release();

		static bool BindWindowsFunction(SOCKET _iSocket, GUID _tGUID, LPVOID* _pFn);

		/*
		* @details	家南阑 积己钦聪促.
		*/
		static SOCKET OpenSocket();

		static bool SetLinger(SOCKET _iSocket, uint16 _iOnOff, uint16 _iLinger);

		static bool SetReuseAddress(SOCKET _iSocket, bool _bFlag);

		static bool SetRecvBufferSize(SOCKET _iSocket, int32 _iSize);

		static bool SetSendBufferSize(SOCKET _iSocket, int32 _iSize);

		static bool SetTCPNoDelay(SOCKET _iSocket, bool _bFlag);

		static bool SetUpdateAcceptSocket(SOCKET _iSocket, SOCKET _iListenSocket);

		static bool Bind(SOCKET _iSocket, CSockAddrEx _oSockAddr);

		static bool Bind(SOCKET _iSocket, SOCKADDR_IN _tSockAddr);

		static bool BindAnyAddress(SOCKET _iSocket, uint16 _iPort);

		static bool Listen(SOCKET _iSocket, int32 _iBackLog = SOMAXCONN);

		static void CloseSocket(SOCKET& _iSocket);
	};

	template<typename T>
	static inline bool SetSocketOption(SOCKET _iSocket, int32 _iLevel, int32 _iOptionName, T _pOptionValue)
	{
		return SOCKET_ERROR != ::setsockopt(_iSocket, _iLevel, _iOptionName, reinterpret_cast<char*>(&_pOptionValue), sizeof(T));
	}
}
