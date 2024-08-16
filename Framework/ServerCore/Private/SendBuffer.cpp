#include "pch.h"
#include "SendBuffer.h"

ServerCore::CSendBuffer::CSendBuffer(int32 _iBufferSize)
{
	m_vecSendBuffer.resize(_iBufferSize);
}

ServerCore::CSendBuffer::~CSendBuffer()
{
}

void ServerCore::CSendBuffer::CopyData(void* _pData, int32 _iLength)
{
	assert(_iLength <= GetBufferSize());
	::memcpy(m_vecSendBuffer.data(), _pData, _iLength);
	m_iWriteSize = _iLength;
}

void ServerCore::CSendBuffer::Close(uint32 _iWriteSize)
{
	m_iWriteSize = _iWriteSize;
}
