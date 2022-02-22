#pragma once

#include "X.TaskMonitor.Types.h"
#include "ProcessItem.h"

namespace Xin::TaskMonitor
{
	struct [[Meta]] FProcessInfo
	{
		FDateTime CreationTime;

		IBitmapRef Icon;
		FString ProcessName;
		uint32 ProcessId = 0;
		uint32 ThreadCount = 0;
		FString CurrentDirectory;

		FString CommandLine;
		FString FilePath;
	};

	class FProcessManager : IDependency
	{
	public:
		FProcessManager() = default;
		~FProcessManager() = default;

	public:
		TList<FProcessInfo> EnumProcesses();
	};
}
