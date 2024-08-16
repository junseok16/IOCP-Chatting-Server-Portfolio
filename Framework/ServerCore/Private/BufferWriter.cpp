#include "pch.h"
#include "BufferWriter.h"

CBufferWriter::CBufferWriter()
{

}

CBufferWriter::CBufferWriter(BYTE* _pBuffer, uint32 _iSize, uint32 _iPosition)
	: m_pBuffer(_pBuffer)
	, m_iSize(_iSize)
	, m_iPosition(_iPosition)
{

}

CBufferWriter::~CBufferWriter()
{

}

bool CBufferWriter::Write(void* _pSrc, uint32 _iLength)
{
	if (GetEmptySize() < _iLength) { return false; }
	
	::memcpy(&m_pBuffer[m_iPosition], _pSrc, _iLength);
	m_iPosition += _iLength;
	return true;
}