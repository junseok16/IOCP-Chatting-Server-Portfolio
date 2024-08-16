#pragma once

namespace ServerCore
{
	class CBufferReader final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CBufferReader();

		CBufferReader(BYTE* _pBuffer, uint32 _iSize, uint32 _iPosition = 0);

		virtual ~CBufferReader();
		
	/********************
		Getter/Setter
	********************/
	public:
		BYTE* GetBuffer() const { return m_pBuffer; }

		uint32 GetSize() const { return m_iSize; }

		uint32 GetReadPosition() const { return m_iPosition; }

		uint32 GetEmptySize() const { return m_iSize - m_iPosition; }

	public:
		template<typename T>
		bool Peek(T* _pDest) { return Peek(_pDest, sizeof(T)); }

		bool Peek(void* _pDest, uint32 _iLength);

		template<typename T>
		bool Read(T* _pDest) { return Read(_pDest, sizeof(T)); }

		bool Read(void* _pDest, uint32 _iLength);

		template<typename T>
		CBufferReader& operator>>(OUT T& _tyDest);
		
	/********************
		Data Members
	********************/
	private:
		BYTE* m_pBuffer = nullptr;

		uint32 m_iSize = 0;

		uint32 m_iPosition = 0;
	};

	template<typename T>
	inline CBufferReader& CBufferReader::operator>>(OUT T& _tyDest)
	{
		_tyDest = *reinterpret_cast<T*>(&m_pBuffer[m_iPosition]);
		m_iPosition += sizeof(T);
		return *this;
	}
}
