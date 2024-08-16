#pragma once

namespace ServerCore
{
	class CBufferWriter final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CBufferWriter();

		CBufferWriter(BYTE* _pBuffer, uint32 _iSize, uint32 _iPosition = 0);

		virtual ~CBufferWriter();
		
	/********************
		Getter/Setter
	********************/
	public:
		BYTE* GetBuffer() const { return m_pBuffer; }

		uint32 GetSize() const { return m_iSize; }

		uint32 GetWritePosition() const { return m_iPosition; }

		uint32 GetEmptySize() const { return m_iSize - m_iPosition; }

	public:
		template<typename T>
		bool Write(T* _pSrc) { return Write(_pSrc, sizeof(T)); }

		bool Write(void* _pSrc, uint32 _iLength);

		template<typename T>
		T* Reserve();

		template<typename T>
		CBufferWriter& operator<<(T&& src);
		
	/********************
		Data Members
	********************/
	private:
		BYTE* m_pBuffer = nullptr;

		uint32 m_iSize = 0;

		uint32 m_iPosition = 0;
	};

	template<typename T>
	T* CBufferWriter::Reserve()
	{
		if (GetEmptySize() < sizeof(T)) { return nullptr; }
		
		T* ret = reinterpret_cast<T*>(&m_pBuffer[m_iPosition]);
		m_iPosition += sizeof(T);
		return ret;
	}

	template<typename T>
	CBufferWriter& CBufferWriter::operator<<(T&& _tySrc)
	{
		using DataType = std::remove_reference_t<T>;
		*reinterpret_cast<DataType*>(&m_pBuffer[m_iPosition]) = std::forward<DataType>(_tySrc);
		m_iPosition += sizeof(T);
		return *this;
	}
}