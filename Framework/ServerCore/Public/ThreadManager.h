#pragma once

namespace ServerCore
{
	class CThreadManager final
	{
	/********************
		Ctor/Dtor
	********************/
	public:
		CThreadManager();

		virtual ~CThreadManager();

	/********************
		Methods
	********************/
	public:
		void Launch(std::function<void(void)> _fnCallback);

		void Join();

	private:
		static void InitializeThreadLocalStorage();

		static void ReleaseThreadLocalStorage();
		
	/********************
		Data Memers
	********************/
	private:
		std::mutex m_mtThreadManager;

		std::vector<std::thread> m_vecThreads;
	};
}
