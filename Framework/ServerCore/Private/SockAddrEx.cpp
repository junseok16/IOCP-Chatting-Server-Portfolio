#include "pch.h"
#include "SockAddrEx.h"

ServerCore::CSockAddrEx::CSockAddrEx(SOCKADDR_IN _tSockAddr)
	: m_tSockAddr(_tSockAddr)
{
}

ServerCore::CSockAddrEx::CSockAddrEx(std::wstring _wstrIP, uint16 _iPort)
{
	::memset(&m_tSockAddr, 0, sizeof(m_tSockAddr));
	m_tSockAddr.sin_family = AF_INET;
	m_tSockAddr.sin_addr = IP2Address(_wstrIP.c_str());
	m_tSockAddr.sin_port = ::htons(_iPort);
}

std::wstring ServerCore::CSockAddrEx::GetIPAddress()
{
	WCHAR chBuffer[128];
	::InetNtopW(AF_INET, &m_tSockAddr.sin_addr, chBuffer, len32(chBuffer));
	return std::wstring(chBuffer);
}

IN_ADDR ServerCore::CSockAddrEx::IP2Address(const WCHAR* _pIP)
{
	IN_ADDR tAddress;
	::InetPtonW(AF_INET, _pIP, &tAddress);
	return tAddress;
}
