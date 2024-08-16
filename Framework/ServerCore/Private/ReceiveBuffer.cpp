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

	// �б�, ���� Ŀ���� ���� ��ġ�� �ִ� ���, 0���� �ʱ�ȭ�մϴ�.
	if (iReceivedDataSize == 0)
	{
		m_iReadCursor = m_iWriteCursor = 0;
	}
	// ����ִ� ������ �� ���� ũ�� �̸��� ���, �����÷ΰ� �߻����� �ʵ��� �����͸� ������ �̵��մϴ�.
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
	
	// �б� Ŀ���� ���������� �̵��մϴ�.
	m_iReadCursor += _iNumBytes;
	return true;
}

bool ServerCore::CReceiveBuffer::OnWrite(int32 _iNumBytes)
{
	if (GetEmptySize() < _iNumBytes) { return false; }
	
	// ���� Ŀ���� ���������� �̵��մϴ�.
	m_iWriteCursor += _iNumBytes;
	return true;
}
