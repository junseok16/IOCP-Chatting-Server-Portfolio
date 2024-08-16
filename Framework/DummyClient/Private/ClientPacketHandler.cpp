#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Session.h"

void DummyClient::CClientPacketHandler::HandlePacket(BYTE* _pBuffer, int32 _iLength)
{
	CBufferReader oBufferReader(_pBuffer, _iLength);

	FPacketHeader tHeader;
	oBufferReader >> tHeader;

	switch (tHeader.m_iID)
	{
	case S_TEST:
		Handle_S_TEST(_pBuffer, _iLength);
		break;

	default:
		break;
	}
}

void DummyClient::CClientPacketHandler::Handle_S_TEST(BYTE* _pBuffer, int32 _iLength)
{
	FPacketHeader* pHeader = reinterpret_cast<FPacketHeader*>(_pBuffer);
	//uint16 id = header->id;
	uint16 iSize = pHeader->m_iSize;

	Protocol::S_TEST pkt;
	pkt.ParseFromArray(&pHeader[1], iSize - sizeof(FPacketHeader));

	uint64 iID = pkt.id();
	uint32 iHP = pkt.hp();
	uint16 iAttack = pkt.attack();

	std::cout << "ID: " << iID << " HP : " << iHP << " ATT : " << iAttack << std::endl;

	for (int32 i = 0; i < pkt.buffs_size(); i++)
	{
		const Protocol::BuffData& data = pkt.buffs(i);
		std::cout << "BuffInfo : " << data.buffid() << " " << data.remaintime() << std::endl;
	}
}
