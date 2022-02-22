#include "PCH.h"

#include "ProcessManager.h"
//#include "Xin.Core/Platform/Windows/Internal/Private/InternalNT.h"
#include "Xin.Core/Platform/Windows/Internal/Internal.h"

#include <winternl.h>

#include <TlHelp32.h>
#include <shobjidl_core.h>

#ifdef EnumProcesses
#undef EnumProcesses
#endif

namespace Xin::TaskMonitor
{
	struct FHandleCloser
	{
		void operator()(HANDLE Handle)
		{
			if (Handle != INVALID_HANDLE_VALUE)
				::CloseHandle(Handle);
		}
	};

	TList<FProcessInfo> FProcessManager::EnumProcesses()
	{
		TList<FProcessInfo> ProcessInfos;
		FHandle ProcessSnapshot { ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
		if (ProcessSnapshot.Handle != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32W Processentry32W = { sizeof(PROCESSENTRY32W) };
			if (::Process32FirstW(ProcessSnapshot.Handle, &Processentry32W))
			{
				do
				{
					FProcessInfo & ProcessInfo = ProcessInfos.AddDefault();
					ProcessInfo.ProcessId = Processentry32W.th32ProcessID;
					//ProcessItem.ParentID = Processentry32W.th32ParentProcessID;

					ProcessInfo.ProcessName = FEncoding::ToUtf8(FWStringV(Processentry32W.szExeFile));
					ProcessInfo.ThreadCount = Processentry32W.cntThreads;

					//{
					//	MODULEENTRY32W ModuleEntry32W = { sizeof(MODULEENTRY32W) };
					//	FHandle ModuleSnapshot { ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Processentry32W.th32ProcessID) };
					//	if (::Module32FirstW(ModuleSnapshot.Handle, &ModuleEntry32W))
					//	{
					//		ProcessItem.FilePath = FEncoding::ToUtf8(FWStringV(ModuleEntry32W.szExePath));
					//	}
					//}

					if (Processentry32W.th32ProcessID == Internal::FProcess::IdelProcessId ||
						Processentry32W.th32ProcessID == Internal::FProcess::SystemProcessId)
					{
						ProcessInfo.FilePath = FEncoding::ToUtf8(Internal::NtFilePathToDosFilePath(L"\\SystemRoot\\System32\\ntoskrnl.exe"));
					}
					else
					{
						ProcessInfo.FilePath = FEncoding::ToUtf8(Internal::FProcess::QueryProcessImageFilePath(Processentry32W.th32ProcessID));

						Internal::FProcess Process { Processentry32W.th32ProcessID, PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ };
						if (Process)
						{
							ProcessInfo.CommandLine = FEncoding::ToUtf8(Process.QueryCommandLine());
							ProcessInfo.CurrentDirectory = FEncoding::ToUtf8(Process.QueryCurrentDirectory());
						}
					}
				}
				while (::Process32NextW(ProcessSnapshot.Handle, &Processentry32W));
			}
		}

		//ProcessInfos.Resize(20);
		return ProcessInfos;
	}
}
