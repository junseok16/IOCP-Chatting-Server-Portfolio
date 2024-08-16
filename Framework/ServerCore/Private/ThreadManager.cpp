#include "pch.h"
#include "ThreadManager.h"

ServerCore::CThreadManager::CThreadManager()
{
	InitializeThreadLocalStorage();
}

ServerCore::CThreadManager::~CThreadManager()
{
	Join();
}

void ServerCore::CThreadManager::Launch(std::function<void(void)> _fnCallback)
{
	// 스레드가 TLS를 초기화하고 콜백 함수를 호출합니다.
	std::lock_guard lgLock(m_mtThreadManager);

	m_vecThreads.push_back(std::thread([=]()
		{
			InitializeThreadLocalStorage();
			_fnCallback();
			ReleaseThreadLocalStorage();
		})
	);
}

void ServerCore::CThreadManager::Join()
{
	for (std::thread& thThread : m_vecThreads)
	{
		if (thThread.joinable())
		{
			thThread.join();
		}	
	}
	m_vecThreads.clear();
}

void ServerCore::CThreadManager::InitializeThreadLocalStorage()
{
	static std::atomic<uint32> s_iThreadID = 1;

	l_iThreadID = s_iThreadID.fetch_add(1);
}

void ServerCore::CThreadManager::ReleaseThreadLocalStorage()
{
}
