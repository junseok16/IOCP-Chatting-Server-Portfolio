#pragma once

namespace ServerCore
{
	class CReceiveBuffer final
	{
		// 나열자 둔갑술로 상수를 선언합니다.
		enum { NUM_BUFFERS = 10 };
		
	/********************
		Ctor/Dtor
	********************/
	public:
		explicit CReceiveBuffer(int32 _iBufferSize);

		virtual ~CReceiveBuffer();
		
	/********************
		Methods
	********************/
	public:
		void Clear();

		bool OnRead(int32 _iNumBytes);

		bool OnWrite(int32 _iNumBytes);
		
	/********************
		Getter/Setter
	********************/
	public:
		BYTE* GetReadPosition() { return &m_vecReceiveBuffer[m_iReadCursor]; }
		
		BYTE* GetWritePosition() { return &m_vecReceiveBuffer[m_iWriteCursor]; }
		
		int32 GetReceivedDataSize() const { return m_iWriteCursor - m_iReadCursor; }

		int32 GetEmptySize() const { return m_iCapacity - m_iWriteCursor; }
		
	/********************
		Data Members
	********************/
	private:
		int32 m_iCapacity = 0;

		int32 m_iBufferSize = 0;

		// 읽기 커서 위치
		int32 m_iReadCursor = 0;

		// 쓰기 커서 위치
		int32 m_iWriteCursor = 0;

		// 수신 버퍼
		std::vector<BYTE> m_vecReceiveBuffer;
	};
}
