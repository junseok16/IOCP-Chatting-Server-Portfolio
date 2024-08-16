#pragma once

namespace ServerCore
{
	class CSendBuffer;
	struct FPacketHeader;
}

namespace Server
{
	enum { S_TEST = 1 };

	struct FBuffData
	{
		uint64 m_iBuffID = 0;
		float m_iRemainTime = 0.0f;
	};

	class CServerPacketHandler final
	{
	public:
		static void HandlePacket(BYTE* _pBuffer, int32 _iLength);

		static std::shared_ptr<CSendBuffer> Make_S_TEST(uint64 id, uint32 hp, uint16 attack, std::vector<FBuffData> buffs);

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
