#pragma once

namespace ServerCore
{
	class CSendBuffer;
}

namespace Server
{
	class CGameSession;

	class CGameSessionManager final
	{
	public:
		void Add(std::shared_ptr<CGameSession> _spSession);

		void Remove(std::shared_ptr<CGameSession> _spSession);

		void Broadcast(std::shared_ptr<CSendBuffer> _spSendBuffer);

	private:
		DECLARE_LOCK;

		std::set<std::shared_ptr<CGameSession>> m_setGameSessions;
	};
}

extern Server::CGameSessionManager g_oGameSessionManager;
