#pragma once

// Windows ���̺귯��
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

// C++ ǥ�� ���̺귯��
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <utility>
#include <functional>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <format>
#include <atlstr.h>
#include <any>

// C ���̺귯��
#include <cassert>
#include <cmath>
#include <ctime>
#include <climits>
#include <cstdio>

// ����� ���� ���̺귯��
#include "ServerCoreType.h"
#include "ServerCoreGlobal.h"
#include "ServerCoreMacro.h"
#include "ServerCoreTLS.h"
using namespace ServerCore;