#include "pch.h"
#include "GameSession.h"

#include "SendBuffer.h"
#include "GameSessionManager.h"

void Server::CGameSession::OnConnected()
{
	g_oGameSessionManager.Add(static_pointer_cast<CGameSession>(shared_from_this()));
}

void Server::CGameSession::OnDisconnected()
{
	g_oGameSessionManager.Remove(static_pointer_cast<CGameSession>(shared_from_this()));
}

void Server::CGameSession::OnReceivedPacket(BYTE* _pBuffer, int32 _iLength)
{
	FPacketHeader tHeader = *(reinterpret_cast<FPacketHeader*>(_pBuffer));
	std::cout << "Packet ID : " << tHeader.m_iID << "Size : " << tHeader.m_iSize << std::endl;
}

void Server::CGameSession::OnSend(int32 _iLength)
{
	// std::cout << "OnSend Len = " << _iLength << std::endl;
}