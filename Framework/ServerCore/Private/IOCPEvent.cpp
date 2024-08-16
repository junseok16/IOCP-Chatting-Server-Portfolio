#include "pch.h"
#include "IOCPEvent.h"

ServerCore::FIOCPEvent::FIOCPEvent(EIOCPEventType _eIOCPEventType)
	: m_eIOCPEventType(_eIOCPEventType)
{
	Initialize();
}

void ServerCore::FIOCPEvent::Initialize()
{
	hEvent			= 0;
	Internal		= 0;
	InternalHigh	= 0;
	Offset			= 0;
	OffsetHigh		= 0;
}
