#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "SendBuffer.h"
#include "Session.h"

void Server::CServerPacketHandler::HandlePacket(BYTE* _pBuffer, int32 _iLength)
{
	CBufferReader obufferReader(_pBuffer, _iLength);

	FPacketHeader tHeader;
	obufferReader.Peek(&tHeader);

	switch (tHeader.m_iID)
	{
	case S_TEST:
		break;

	default:
		break;
	}
}

std::shared_ptr<CSendBuffer> Server::CServerPacketHandler::Make_S_TEST(uint64 _iID, uint32 _iHP, uint16 _iAttack, std::vector<FBuffData> _vecBuffs)
{
	Protocol::S_TEST pkt;

	pkt.set_id(10);
	pkt.set_hp(100);
	pkt.set_attack(10);

	{
		Protocol::BuffData* data = pkt.add_buffs();
		data->set_buffid(100);
		data->set_remaintime(1.2f);
		{
			data->add_victims(10);
		}
	}
	{
		Protocol::BuffData* data = pkt.add_buffs();
		data->set_buffid(200);
		data->set_remaintime(2.2f);
		{
			data->add_victims(20);
		}
	}

	return MakeSendBuffer(pkt, S_TEST);
}
