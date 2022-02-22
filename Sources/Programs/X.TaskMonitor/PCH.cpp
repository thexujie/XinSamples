#include "PCH.h"

#pragma comment(lib, "imm32.lib")

namespace Xin
{
	void Log(ELogLevel LogLevel, FStringV String)
	{
		Logger().WriteLine(LogLevel, u8"Xin.TaskMonitor"V, String);
	}
}
