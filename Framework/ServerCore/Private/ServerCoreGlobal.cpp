// default
#include "pch.h"
#include "ServerCoreGlobal.h"

// manager
#include "ThreadManager.h"

std::unique_ptr<CThreadManager> ServerCore::g_upThreadManager = std::make_unique<CThreadManager>();