#pragma once
#include "SockAddrEx.h"

namespace ServerCore
{
	class CIOCPCore;
	class CSession;
	class CListener;

	enum class EServiceType : uint8
	{
		SERVER,
		CLIENT,
		ENUM_END
	};

	class CService : public std::enable_shared_from_this<CService>
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CService
		(
			EServiceType _eServiceType,
			CSockAddrEx _oSockAddrEx,
			std::shared_ptr<CIOCPCore> _spIOCPCore,
			std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory,
			int32 _iMaxNumSessions = 1
		);
		
		virtual ~CService();
		
	/********************
		Methods
	********************/
	public:
		/**
		* @brief	서비스를 시작합니다.
		* @details	서비스 클래스를 상속한 자식 클래스에서 이 순수 가상 함수를 구현해야 합니다.
		*/
		virtual bool Open() abstract;

		/**
		* @brief	세션 객체를 생성하는 함수가 있는지 반환합니다.
		* @return	bool
		*/
		bool IsAbleToOpen() const { return m_fnSessionFactory != nullptr; }

		/**
		* @brief	서비스를 종료합니다.
		*/
		virtual void Close();

		std::shared_ptr<CSession> CreateSession();

		void AddSession(std::shared_ptr<CSession> _spSession);

		void ReleaseSession(std::shared_ptr<CSession> _spSession);
		
	/********************
		Getter/Setter
	********************/
	public:
		void SetSessionFactory(std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory) { m_fnSessionFactory = _fnSessionFactory; }

		int32 GetNumSessions() const { return m_iNumSessions; }

		int32 GetMaxNumSessions() const { return m_iMaxNumSessions; }

		EServiceType GetServiceType() const { return m_eServiceType; }

		CSockAddrEx GetSockAddrEx() const { return m_oSockAddrEx; }

		std::shared_ptr<CIOCPCore>& GetIOCPCore() { return m_spIOCPCore; }
		
	/********************
		Data Members
	********************/
	protected:
		DECLARE_LOCK;

		EServiceType m_eServiceType = EServiceType::ENUM_END;

		CSockAddrEx m_oSockAddrEx{};

		std::shared_ptr<CIOCPCore> m_spIOCPCore = nullptr;

		std::set<std::shared_ptr<CSession>> m_setSessions;
		
		int32 m_iNumSessions = 0;

		int32 m_iMaxNumSessions = 0;

		std::function<std::shared_ptr<CSession>(void)> m_fnSessionFactory = nullptr;
	};

	class CClientService : public CService
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CClientService
		(
			CSockAddrEx _oSockAddrEx,
			std::shared_ptr<CIOCPCore> _spIOCPCore,
			std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory,
			int32 _iMaxNumSessions = 1
		);
		
		virtual ~CClientService() = default;
		
	/********************
		Methods
	********************/
	public:
		virtual bool Open() override;
	};

	class CServerService : public CService
	{
	public:
		CServerService
		(
			CSockAddrEx _oSockAddrEx,
			std::shared_ptr<CIOCPCore> _spIOCPCore,
			std::function<std::shared_ptr<CSession>(void)> _fnSessionFactory,
			int32 _iMaxNumSessions = 1
		);
		
		virtual ~CServerService() = default;

	public:
		virtual bool Open() override;

		virtual void Close() override;

	private:
		std::shared_ptr<CListener> m_spListener = nullptr;
	};
}
