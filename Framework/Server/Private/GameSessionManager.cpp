#include "pch.h"
#include "GameSessionManager.h"

#include "GameSession.h"
#include "SendBuffer.h"

Server::CGameSessionManager g_oGameSessionManager;

void Server::CGameSessionManager::Add(std::shared_ptr<CGameSession> _spGameSession)
{
	WRITE_LOCK;
	m_setGameSessions.insert(_spGameSession);
}

void Server::CGameSessionManager::Remove(std::shared_ptr<CGameSession> _spGameSession)
{
	WRITE_LOCK;
	m_setGameSessions.erase(_spGameSession);
}

void Server::CGameSessionManager::Broadcast(std::shared_ptr<ServerCore::CSendBuffer> _spSendBuffer)
{
	WRITE_LOCK;
	for (std::shared_ptr<CGameSession> _spGameSession : m_setGameSessions)
	{
		_spGameSession->Send(_spSendBuffer);
	}
}