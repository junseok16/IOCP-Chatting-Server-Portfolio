#pragma once

#define size16(val) static_cast<int16>(sizeof(val))
#define size32(val) static_cast<int32>(sizeof(val))
#define len16(arr)	static_cast<int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)	static_cast<int32>(sizeof(arr)/sizeof(arr[0]))

#define DECLARE_LOCKS(_iCntLocks)	std::mutex m_mtLocks[_iCntLocks];
#define DECLARE_LOCK				DECLARE_LOCKS(1)
#define	WRITE_LOCK_IDX(_iIndex)		std::lock_guard<std::mutex> m_lgWrite_##idx(m_mtLocks[_iIndex]);
#define WRITE_LOCK					WRITE_LOCK_IDX(0)