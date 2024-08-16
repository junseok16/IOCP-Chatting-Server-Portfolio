#pragma once
#include "IOCPCore.h"
#include "IOCPEvent.h"
#include "SockAddrEx.h"
#include "ReceiveBuffer.h"

namespace ServerCore
{
	class CService;
	class CSendBuffer;
	struct FIOCPEvent;

	class CSession : public CIOCPObject
	{
		friend class CListener;
		friend class CIOCPCore;
		friend class CService;

		enum { BUFFER_SIZE = 0x1'0000 };

	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CSession();

		virtual ~CSession();
		
	/********************
		Methods
	********************/
	public:
		void Send(std::shared_ptr<CSendBuffer> _spSendBuffer);

		bool Connect();

		void Disconnect(const WCHAR* _pCause);

		void HandleError(int32 _iErrorCode);

	public:
		virtual HANDLE GetHandle() override;

		virtual void Dispatch(FIOCPEvent* _pIOCPEvent, int32 _iNumBytes = 0) override;

	private:
		// IOCP 큐 전처리
		bool RegisterConnect();

		bool RegisterDisconnect();

		void RegisterReceive();

		void RegisterSend();

		// IOCP 큐 후처리
		void ProcessConnect();

		void ProcessDisconnect();

		void ProcessReceive(int32 _iNumBytes);

		void ProcessSend(int32 _iNumBytes);

	protected:
		virtual void OnConnected() { return; }

		virtual int32 OnReceived(BYTE* _pBuffer, int32 _iLength) { return _iLength; }

		virtual void OnSend(int32 _iLength) { return; }

		virtual void OnDisconnected() { return; }

	/********************
		Getter/Setter
	********************/
	public:
		void SetService(std::shared_ptr<CService> _spService) { m_wpService = _spService; }

		std::shared_ptr<CService> GetService() const { return m_wpService.lock(); }

		void SetSockAddr(CSockAddrEx _oSockAddr) { m_oSockAddr = _oSockAddr; }

		CSockAddrEx GetSockAddr() const { return m_oSockAddr; }

		SOCKET GetSocket() const { return m_iSocket; }

		bool IsConnected() const { return m_bConnected; }

		std::shared_ptr<CSession> GetSession() { return std::static_pointer_cast<CSession>(shared_from_this()); }

	/********************
		Data Members
	********************/
	private:
		// 이 세션을 관리하는 서비스
		std::weak_ptr<CService> m_wpService;

		// 소켓
		SOCKET m_iSocket = INVALID_SOCKET;
		
		// 소켓 정보
		CSockAddrEx m_oSockAddr{};

		// 연결되었는지 여부
		std::atomic<bool> m_bConnected = false;

	private:
		DECLARE_LOCK;

		// 수신 버퍼
		CReceiveBuffer m_oReceiveBuffer;

		// 발신 버퍼
		std::queue<std::shared_ptr<CSendBuffer>> m_qSendBuffers;

		std::atomic<bool> m_bSendRegistered = false;

	private:
		// 수락 이벤트는 리스너만 갖고 있습니다.

		FIOCPEvent m_tConnectEvent{ EIOCPEventType::CONNECT };

		FIOCPEvent m_tDisconnectEvent{ EIOCPEventType::DISCONNECT };

		FIOCPEvent m_tReceiveEvent{ EIOCPEventType::RECEIVE };

		FIOCPEvent m_tSendEvent{ EIOCPEventType::SEND };
	};

	struct FPacketHeader
	{
		// 크기
		uint16 m_iSize = 0;

		// 프로토콜
		uint16 m_iID = 0;
	};

	class CPacketSession : public CSession
	{
	public:
		explicit CPacketSession();

		virtual ~CPacketSession();

	public:
		std::shared_ptr<CPacketSession> GetPacketSession() { return static_pointer_cast<CPacketSession>(shared_from_this()); }

	protected:
		virtual int32 OnReceived(BYTE* _pBuffer, int32 _iLength) sealed;

		virtual void OnReceivedPacket(BYTE* _pBuffer, int32 _iLength) abstract;
	};
}
