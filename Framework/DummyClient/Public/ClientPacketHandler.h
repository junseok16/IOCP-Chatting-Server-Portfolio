#pragma once

namespace ServerCore
{
	class CSendBuffer;
	struct FPacketHeader;
}

namespace DummyClient
{
	enum { S_TEST = 1 };

	class CClientPacketHandler final
	{
	public:
		static void HandlePacket(BYTE* _pBuffer, int32 _iLength);

		// 받기
		static void Handle_S_TEST(BYTE* _pBuffer, int32 _iLength);

		// 보내기
		template<typename T>
		static std::shared_ptr<CSendBuffer> MakeSendBuffer(T& pkt, uint16 _iPacketID)
		{
			const uint16 iDataSize = static_cast<uint16>(pkt.ByteSizeLong());
			const uint16 iPacketSize = iDataSize + sizeof(FPacketHeader);

			std::shared_ptr<CSendBuffer> spSendBuffer = std::make_shared<CSendBuffer>(iPacketSize);
			FPacketHeader* pHeader = reinterpret_cast<FPacketHeader*>(spSendBuffer->GetBuffer());
			pHeader->m_iSize = iPacketSize;
			pHeader->m_iID = _iPacketID;
			assert(pkt.SerializeToArray(&pHeader[1], iDataSize));
			spSendBuffer->Close(iPacketSize);

			return spSendBuffer;
		}
	};
}
