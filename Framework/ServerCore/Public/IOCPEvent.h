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

		// �̺�Ʈ�� �߻���Ų ����; �̺�Ʈ�� ó���Ǳ� ������ ������ �޸𸮿��� �������� �ʵ��� �Ѵ�.
		std::shared_ptr<CIOCPObject> m_spOwner = nullptr;

		std::shared_ptr<CSession> m_spSession = nullptr;

		// TEMP
		std::vector<BYTE> m_vecBuffer;

		std::vector<std::shared_ptr<CSendBuffer>> m_vecSendBuffers;
	};
}
