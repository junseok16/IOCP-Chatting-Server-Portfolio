#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

CService::CService
(
	EServiceType _eServiceType,
	CSockAddrEx _oSockAddrEx,
	std::shared_ptr<CIOCPCore> _spIOCPCore,
	std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory,
	int32 _iMaxNumSessions
)
	: m_eServiceType(_eServiceType)
	, m_oSockAddrEx(_oSockAddrEx)
	, m_spIOCPCore(_spIOCPCore)
	, m_fnSessionFactory(_fnSessionFactory)
	, m_iMaxNumSessions(_iMaxNumSessions)
{
}

CService::~CService()
{
}

void CService::Close()
{
}

std::shared_ptr<CSession> CService::CreateSession()
{
	std::shared_ptr<CSession> spSession = m_fnSessionFactory();
	spSession->SetService(shared_from_this());
	if (m_spIOCPCore->Register(spSession) == false) { return nullptr; }
	
	return spSession;
}

void CService::AddSession(std::shared_ptr<CSession> _spSession)
{
	WRITE_LOCK;
	m_iNumSessions++;
	m_setSessions.insert(_spSession);
}

void CService::ReleaseSession(std::shared_ptr<CSession> _spSession)
{
	WRITE_LOCK;
	assert(m_setSessions.erase(_spSession) != 0);
	m_iNumSessions--;
}

CClientService::CClientService
(
	CSockAddrEx _oSockAddrEx,
	std::shared_ptr<CIOCPCore> _spIOCPCore,
	std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory,
	int32 _iMaxNumSessions
)
	: CService(EServiceType::CLIENT, _oSockAddrEx, _spIOCPCore, _fnSessionFactory, _iMaxNumSessions)
{
}

bool CClientService::Open()
{
	if (IsAbleToOpen() == false) { return false; }
	
	// 技记 家南阑 积己钦聪促.
	const int32 iNumSessions = GetMaxNumSessions();
	for (int32 iSessionIndex = 0; iSessionIndex < iNumSessions; ++iSessionIndex)
	{
		std::shared_ptr<CSession> spSession = CreateSession();
		if (spSession->Connect() == false) { return false; }	
	}

	return true;
}

CServerService::CServerService
(
	CSockAddrEx _oSockAddrEx,
	std::shared_ptr<CIOCPCore> _spIOCPCore,
	std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory,
	int32 _iMaxNumSessions
)
	: CService(EServiceType::SERVER, _oSockAddrEx, _spIOCPCore, _fnSessionFactory, _iMaxNumSessions)
{
}

bool CServerService::Open()
{
	if (IsAbleToOpen() == false) { return false; }
	
	// 府胶呈 家南阑 积己钦聪促.
	m_spListener = std::make_shared<CListener>();
	if (m_spListener == nullptr) { return false; }
	
	std::shared_ptr<CServerService> spServerService = static_pointer_cast<CServerService>(shared_from_this());
	if (m_spListener->Accept(spServerService) == false) { return false; }
	
	return true;
}

void CServerService::Close()
{
	// TODO
	CService::Close();
}
