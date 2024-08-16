#pragma once

namespace ServerCore
{
	class CSendBuffer final
	{
	public:
		explicit CSendBuffer(int32 _iBufferSize);

		virtual ~CSendBuffer();

	public:
		BYTE* GetBuffer() { return m_vecSendBuffer.data(); }

		int32 GetWriteSize() const { return m_iWriteSize; }

		int32 GetBufferSize() { return static_cast<int32>(m_vecSendBuffer.size()); }

		void CopyData(void* _pData, int32 _iLength);

		void Close(uint32 _iWriteSize);

	private:
		// 발신 버퍼
		std::vector<BYTE> m_vecSendBuffer;

		int32 m_iWriteSize = 0;
	};
}
