#pragma once

#include "X.TaskMonitor.h"
#include "Process/ProcessItem.h"
#include "Process/ProcessManager.h"
#include "Utils/ShellItemImage.h"

#include "Xin.Rendering/Xin.Rendering.h"
#include "Xin.Windows.Informer/Xin.Windows.Informer.h"

namespace Xin::TaskMonitor
{
	class FTaskMonitor : public IObject
	{
	public:
		FTaskMonitor();
		~FTaskMonitor();

		int32 Run();

	private:
		void UpdateMonitor();

	private:
		void CreatePerformanceScrollPanel();
		void UpdatePerformance();

	private:
		void CreateProcessListView();
		void UpdateProcesses();
		void QueryProcessItemContextMenu(FListViewQueryItemContextMenuEventArgs & Args);

	public:
		FResourceTableRef GlobalResourceTable = MakeRefer<FResourceTable>();

		UWindowRef MainWindow;

		ControlsEx::UNavigatorRef Navigator;

	public:
		// Performance
		UScrollViewerRef PerformanceSV;
		TReferPtr<UTextBlock> CurrentCpuFrequencyValue;
		Informer::FCpuPerformance CpuPerformance;

		// Process
		UListViewRef ProcessLV;

		TList<FProcessItemRef> ProcessItems;
		TMap<uint32, FProcessItemRef> MappedProcessItems;
		FDispatcherTimer ProcessUpdatetTimer { 1000 };

	private:
		FProcessManager ProcessManager;
		Internal::FImageInfoCache ImageInfoCache;
		FShellItemImageCache ShellItemImageCache;
	};
}
