#pragma once

namespace ServerCore
{
	class CSendBuffer;
	class CIOCPObject;
	class CSession;

	enum class EIOCPEventType : uint8
	{
		CONNECT,
		DISCONNECT,
		ACCEPT,
		RECEIVE,
		SEND,
		ENUM_END
	};

	struct FIOCPEvent : public OVERLAPPED
	{
	public:
		FIOCPEvent(EIOCPEventType _eIOCPEventType);

	public:
		void Initialize();

	public:
		EIOCPEventType m_eIOCPEventType = EIOCPEventType::ENUM_END;

		// 이벤트를 발생시킨 소켓; 이벤트가 처리되기 전까지 소켓이 메모리에서 해제되지 않도록 한다.
		std::shared_ptr<CIOCPObject> m_spOwner = nullptr;

		std::shared_ptr<CSession> m_spSession = nullptr;

		// TEMP
		std::vector<BYTE> m_vecBuffer;

		std::vector<std::shared_ptr<CSendBuffer>> m_vecSendBuffers;
	};
}
