#include "pch.h"
#include "BufferReader.h"

CBufferReader::CBufferReader()
{
}

CBufferReader::CBufferReader(BYTE* _pBuffer, uint32 _iSize, uint32 _iPosition)
	: m_pBuffer(_pBuffer)
	, m_iSize(_iSize)
	, m_iPosition(_iPosition)
{

}

CBufferReader::~CBufferReader()
{

}

bool CBufferReader::Peek(void* _pDest, uint32 _iLength)
{
	if (GetEmptySize() < _iLength) { return false; }
	
	::memcpy(_pDest, &m_pBuffer[m_iPosition], _iLength);
	return true;
}

bool CBufferReader::Read(void* _pDest, uint32 _iLength)
{
	if (Peek(_pDest, _iLength) == false) { return false; }
	
	m_iPosition += _iLength;
	return true;
}
