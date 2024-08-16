#pragma once

namespace ServerCore
{
	class CSockAddrEx final
	{
	public:
		CSockAddrEx() = default;

		CSockAddrEx(SOCKADDR_IN _tSockAddr);

		CSockAddrEx(std::wstring _wstrIP, uint16 _iPort);

	public:
		SOCKADDR_IN& GetSockAddr() { return m_tSockAddr; }

		std::wstring GetIPAddress();

		uint16 GetPort() { return ::ntohs(m_tSockAddr.sin_port); }

	public:
		static IN_ADDR IP2Address(const WCHAR* _pIP);

	private:
		SOCKADDR_IN m_tSockAddr{};
	};
}
