#include "pch.h"
#include "ReceiveBuffer.h"

ServerCore::CReceiveBuffer::CReceiveBuffer(int32 _iBufferSize)
	: m_iBufferSize(_iBufferSize)
	, m_iCapacity(_iBufferSize * NUM_BUFFERS)
{
	m_vecReceiveBuffer.resize(m_iCapacity);
}

ServerCore::CReceiveBuffer::~CReceiveBuffer()
{
}

void ServerCore::CReceiveBuffer::Clear()
{
	int32 iReceivedDataSize = GetReceivedDataSize();

	// 읽기, 쓰기 커서가 같은 위치에 있는 경우, 0으로 초기화합니다.
	if (iReceivedDataSize == 0)
	{
		m_iReadCursor = m_iWriteCursor = 0;
	}
	// 비어있는 공간이 한 버퍼 크기 미만인 경우, 오버플로가 발생하지 않도록 데이터를 앞으로 이동합니다.
	else
	{
		if (GetEmptySize() < m_iBufferSize)
		{
			::memcpy(&m_vecReceiveBuffer[0], &m_vecReceiveBuffer[m_iReadCursor], iReceivedDataSize);
			m_iReadCursor = 0;
			m_iWriteCursor = iReceivedDataSize;
		}
	}
}

bool ServerCore::CReceiveBuffer::OnRead(int32 _iNumBytes)
{
	if (GetReceivedDataSize() < _iNumBytes) { return false; }
	
	// 읽기 커서를 오른쪽으로 이동합니다.
	m_iReadCursor += _iNumBytes;
	return true;
}

bool ServerCore::CReceiveBuffer::OnWrite(int32 _iNumBytes)
{
	if (GetEmptySize() < _iNumBytes) { return false; }
	
	// 쓰기 커서를 오른쪽으로 이동합니다.
	m_iWriteCursor += _iNumBytes;
	return true;
}
