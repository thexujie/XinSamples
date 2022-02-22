#pragma once

#include "X.TaskMonitor.Types.h"

#include "Utils/ShellItemImage.h"

namespace Xin::TaskMonitor
{
	template<typename T>
	struct TDeltaValue
	{
		T Value = 0;
		T Delta = 0;

		TDeltaValue() = default;
		TDeltaValue(const TDeltaValue &) = default;
		~TDeltaValue() = default;

		void Update(T NewValue)
		{
			Delta = NewValue - Value;
			Value = NewValue, Delta;
		}

		TDeltaValue & operator=(uint64 NewValue)
		{
			Update(NewValue);
			return *this;
		}
	};

	struct FProcessMemoryInfo
	{
		uint32 PageFault; // 缺页中断次数。

		uint32 Working; // 当前内存使用。
		uint32 PeakWorking; // 内存使用高峰。

		uint32 Page; // 页面文件。
		uint32 PeakPage; // 页面文件。

		uint32 QuotaPage; // 当前页面缓冲池使用。
		uint32 QuotaPeakPage; // 页面缓冲池使用高峰。

		uint32 QuotaNonePage; // 当前非页面缓冲池使用。
		uint32 QuotaPeakNonePage; // 非页面缓冲池使用高峰。
	};

	struct [[Meta]] FProcessItem : IDependency
	{
		META_DECL

		FDateTime CreationTime;

		ProxyPropertyRW(uint32, ProcessId) = 0;
		ProxyPropertyRW(FString, ProcessName);

		FString CommandLine;
		FString FilePath;

		ProxyPropertyRW(uint32, ThreadCount) = 0;
		ProxyPropertyRW(uint64, WorkingSetSize) = 0;
		ProxyPropertyRW(uint64, PagefileUsage) = 0;
		ProxyPropertyRW(FString, CurrentDirectory);
		ProxyPropertyRW(float32, CPUUsage);

		ProxyPropertyRW(FString, Description);
		ProxyPropertyRW(FString, CompanyName);
		ProxyPropertyRW(FString, ProductName);
		ProxyPropertyRW(FString, FileVersion);

		TDeltaValue<uint64> CycleTime;
	};
	using FProcessItemRef = TReferPtr<FProcessItem>;

	class FProcessItemTemplate : public IDataTemplate
	{
	public:
		FProcessItemTemplate() : IDataTemplate(Typeof<FProcessItem>()) { }
		UElementRef Instantiate(UControl & Control, const FVariant & Variant) override;
	};

	class FProcessItemTemplate_IconAndName : public IDataTemplate
	{
	public:
		FProcessItemTemplate_IconAndName(FShellItemImageCache & ShellItemImageCache) : IDataTemplate(Typeof<FProcessItem>()), ShellItemImageCache(ShellItemImageCache) { }
		UElementRef Instantiate(UControl & Control, const FVariant & Variant) override;

	private:
		FShellItemImageCache & ShellItemImageCache;
	};
}
