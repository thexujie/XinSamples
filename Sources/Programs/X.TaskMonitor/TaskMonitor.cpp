#include "PCH.h"
#include "TaskMonitor.h"

#include "Process/ProcessManager.h"

#include <shellapi.h>
//#include <Pdh.h>
//#pragma comment(lib, "Pdh.lib")

#include "Xin.Direct2D/Xin.Direct2D.h"
#include "Xin.Skia/Xin.Skia.h"
#include "Xin.Windows.Informer/Informer.h"

#include "Xin.RHI.D3D12/Xin.RHI.D3D12.h"
#include "Xin.RHI.Vulkan/Xin.RHI.Vulkan.h"

namespace Xin::TaskMonitor
{
	template<typename CharT>
	class comma_numpunct : public std::numpunct<CharT>
	{
	public:
		comma_numpunct(size_t _Refs = 0) : std::numpunct<CharT>(_Refs) {}

	protected:
		CharT do_thousands_sep() const override
		{
			return CharT(',');
		}

		std::string do_grouping() const override
		{
			return "\003";
		}
	};

	struct FCPUUsageConverter
	{
		FStringV operator()(float32 Usage)
		{
			return Format(u8"{}"V, Clamp(int32(Usage * 100), 0, 100));
		}
	};

	struct FByteSizeConverter
	{
		FStringV operator()(uint64 FileSize)
		{
			comma_numpunct<char8> CommaNumPunct { 1 };
			std::locale comma_locale(std::locale(), &CommaNumPunct);
			return Format(comma_locale, u8"{:L} K"V, FileSize >> 10);
			return Format(u8"{:L}K"V, FileSize >> 10);
			//if (FileSize < 1024)
			//	return Format(u8"{}B"V, FileSize);
			//else if (FileSize < 1024 * 1024)
			//	return Format(u8"{}KB"V, FileSize >> 10);
			//else if (FileSize < 1024 * 1024 * 1024)
			//	return Format(u8"{}MB"V, FileSize >> 20);
			//else
			//	return Format(u8"{}GB"V, FileSize >> 30);
		}
	};

	static FText GetFrequencyFriendlyText(uint64 FrequencyHz)
	{
		if (FrequencyHz < 1000)
			return Format(u8"{:.2}Hz"V, float64(FrequencyHz));
		else if (FrequencyHz < 1000 * 1000)
			return Format(u8"{:.2}KHz"V, float64(FrequencyHz) / float64(1000));
		else if (FrequencyHz < 1000 * 1000 * 1000)
			return Format(u8"{:.2}MHz"V, float64(FrequencyHz) / float64(1000 * 1000));
		else
			return Format(u8"{:.2f}GHz"V, float64(FrequencyHz) / float64(1000 * 1000 * 1000));
	}

	class FPerformanceItemTitleStyle : public TControlStyle<UTextBlock>
	{
	public:
		FPerformanceItemTitleStyle() = default;

		void LoadStyleT(UTextBlock & TextBlock) override
		{
			TextBlock.FontSize = 18.0f;
			TextBlock.FontWeight = EFontWeight::Bold;
		}
	};

	static IControlStyleRef PerformanceItemTitleStyle = MakeRefer<FPerformanceItemTitleStyle>();

	static void AdjustPrivileges()
	{
		static constexpr LPCWSTR PrivilegeNames[] =
		{
			SE_DEBUG_NAME,
			SE_INC_BASE_PRIORITY_NAME,
			SE_INC_WORKING_SET_NAME,
			SE_LOAD_DRIVER_NAME,
			SE_PROF_SINGLE_PROCESS_NAME,
			SE_BACKUP_NAME,
			SE_RESTORE_NAME,
			SE_SHUTDOWN_NAME,
			SE_TAKE_OWNERSHIP_NAME,
			SE_SECURITY_NAME,
		};

		FHandle ProcessToken;
		if (::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &ProcessToken))
		{
			TList<LUID_AND_ATTRIBUTES> LuidAndAttributeses;
			//for (LPCWSTR PrivilegeName : PrivilegeNames)
			//{
			//	LUID Luid { };
			//	if (::LookupPrivilegeValueW(NULL, PrivilegeName, &Luid))
			//		LuidAndAttributeses.Add({ Luid , SE_PRIVILEGE_ENABLED });
			//}

			byte PrivilegesBuffer[offsetof(TOKEN_PRIVILEGES, Privileges) + sizeof(LUID_AND_ATTRIBUTES) * ArraySize(PrivilegeNames)] = {};
			TOKEN_PRIVILEGES & TokenPrivileges = *(TOKEN_PRIVILEGES *)PrivilegesBuffer;

			for (LPCWSTR PrivilegeName : PrivilegeNames)
			{
				LUID Luid {};
				if (::LookupPrivilegeValueW(NULL, PrivilegeName, &Luid))
				{
					TokenPrivileges.Privileges[TokenPrivileges.PrivilegeCount].Luid = Luid;
					TokenPrivileges.Privileges[TokenPrivileges.PrivilegeCount].Attributes = SE_PRIVILEGE_ENABLED;
					++TokenPrivileges.PrivilegeCount;
				}
			}

			//TokenPrivileges.PrivilegeCount = LuidAndAttributeses.Size;
			//ArrayCopy(TokenPrivileges.Privileges, LuidAndAttributeses.Data, LuidAndAttributeses.Size);

			BOOL AsjustSucceeded = ::AdjustTokenPrivileges(ProcessToken, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL);
			AssertExpr(AsjustSucceeded);
		}
	}

	FTaskMonitor::FTaskMonitor()
	{
		//AdjustPrivileges();

		//Internal::FFileVersion FileVersion;
		//FileVersion.LoadFromFile(u8R"(C:\Windows\System32\svchost.exe)"V);

		GlobalResourceTable->Resources.AddRange(Themes::Default::LoadDefaultThemesResources());

		MainWindow = MakeRefer<UWindow>(u8"Task Monitor"T, nullptr);
		MainWindow->ResourceTable.Tables.Add(GlobalResourceTable);

		MainWindow->Size = { 1920, 1080 };
		MainWindow->WindowShowPosition = EWindowShowPosition::ScreenCenter;

		UTextBlockRef TextBlockAA;
		UTextBlockRef TextBlockBB;
		UTextBlockRef TextBlockCC;
		UButtonRef Button0;

		MainWindow->Content = StackPanel(EOrientation::Vertical,
			{
				//TextBlockAA = TextBlock(u8"可爱的文字 in TextBlock AA"T, { { UElement::HorizontalAlignmentProperty(), EElementAlignment::Center }, { UElement::SizeProperty(), FDimen2 { FDimen::Percent90, FDimen::Auto } } }),
				Navigator = ControlsEx::Navigator({ { UElement::SizeProperty(), FDimen2 { 100pct, 100pct } } }),
			},
			{
				//{ UPanel::ElementAlignmentProperty(), EElementAlignment::Center },
			});

		CreatePerformanceScrollPanel();
		CreateProcessListView();

		//ProcessLV->ResourceSheet.Resources.Add(MakeRefer<FProcessItemTemplate_IconAndName>(ShellItemImageCache));

		UImageRef Image_AryaStark_256x256 = Image(u8"../../Data/AryaStark_256x256.jpg"V, { { UElement::SizeProperty(), FDimen2 { 512, 512 } } });
		UImageRef Image_Cat = Image(u8"../../Data/Cat.jpg"V, { { UElement::SizeProperty(), FDimen2 { 512, 512 } } });
		UImageRef Image_Standard = Image(u8"../../Data/Standard.png"V, { { UElement::SizeProperty(), FDimen2 { 512, 512 } } });
		UImageRef Image_AryaStark_Auto = Image(u8"../../Data/AryaStark_256x256.jpg"V);

		//IPathRef Tiger = D2DDevice.CreatePath();
		//Tiger->Open();
		////IPath::LoadFromSVG(u8R"(..\..\Data\svgs\arc-path.svg)", Tiger.Ref());
		//IPath::LoadFromSVG(u8R"(..\..\Data\svgs\tiger.svg)", Tiger.Ref());
		//Tiger->Close();
		//Tiger->Transform = { 10, { 200, 200 } };

		FString SvgFilePath = u8R"(..\..\Data\svgs\tiger.svg)";
		SvgFilePath = u8R"(..\..\Data\svgs\Cowboy.svg)";
		//SvgFilePath = u8R"(..\..\Data\svgs\SpglGrfC.svg)";
		//SvgFilePath = u8R"(..\..\Data\svgs\sprite.svg)";
		//SvgFilePath = u8R"(..\..\Data\svgs\arc-path.svg)";
		UVectorGraphRef SvgGraph;
		UElementRef SvgElement = ScrollViewer(SvgGraph = VectorGraph(SvgFilePath,
			{
				{ UElement::HorizontalAlignmentProperty(), EElementAlignment::Center },
			}));
		SvgGraph->SvgScale = { 4.0f, 4.0f };

		Image_AryaStark_256x256->Stretch = EStretch::Uniform;
		Image_Cat->Stretch = EStretch::Uniform;
		Image_Standard->Stretch = EStretch::Uniform;
		Image_AryaStark_Auto->Stretch = EStretch::Uniform;
		Navigator->NavigatorItems =
		{
			{ u8"Performance"N, u8"性能监视"T, PerformanceSV },
			{ u8"Programs"N, u8"应用"T, Image_AryaStark_256x256 },
			{ u8"Processes"N, u8"本地进程"T, ProcessLV },
			{ u8"Services"N, u8"服务"T, Image_Standard },
			{ u8"Startups"N, u8"启动项目"T, SvgElement },
		};
		Navigator->IndicatingIndex = 0;
	}

	FTaskMonitor::~FTaskMonitor()
	{
#if TASK_MONITOR_RHI_PAINTER
		if (Executor)
			Executor->WaitFinish();
#endif
	}

	int32 FTaskMonitor::Run()
	{
		UpdateMonitor();
		ProcessUpdatetTimer.Tick += { this, &FTaskMonitor::UpdateMonitor };
		ProcessUpdatetTimer.Start();

		MainWindow->Show();
		//MainWindow.FindChild(u8"ButtonB"N)->MouseMove += [&](const FMouseEventArgs & Args)
		{
			//FConsole::WriteLineF(u8"MouseMove {}"V, Args.Position);
		};

		MainWindow->Closed = [this](const auto & Args)
		{
			FApplication::Instance().Exit(0);
		};

		return FApplication::Instance().Loop();
	}

	void FTaskMonitor::UpdateMonitor()
	{
		UpdatePerformance();
		UpdateProcesses();
	}

	void FTaskMonitor::CreatePerformanceScrollPanel()
	{
		PerformanceSV = MakeRefer<UScrollViewer>();
		//PerformanceSV->Content = Image(D2DDevice.CreateImage(u8"../../Data/CrystalMaiden.jpg"V));

		UStackPanelRef MainPanel = MakeRefer<UStackPanel>();
		MainPanel->Orientation = EOrientation::Vertical;

		UStackPanelRef CpuPanel = MakeRefer<UStackPanel>();
		{
			CpuPanel->Margin = { 0, 8dim };
			CpuPanel->Orientation = EOrientation::Vertical;
			UStackPanelRef TitlePanel = MakeRefer<UStackPanel>();
			{
				TReferPtr<UTextBlock> CpuLabel = MakeRefer<UTextBlock>(u8"CPU"T);
				CpuLabel->Style = PerformanceItemTitleStyle;

				TReferPtr<UTextBlock> CpuBrand = MakeRefer<UTextBlock>(FText(Informer::GetCpuBrand()));
				CpuBrand->Margin = { 8, 0, 0, 0 };
				CpuBrand->VerticalAlignment = EElementAlignment::Far;
				TitlePanel->Elements = { CpuLabel, Separator({ 100pct, FDimen::Auto }), CpuBrand };
			}
			UHLineRef SplitLine = MakeRefer<UHLine>();
			{
				SplitLine->Margin = { 0, 4, 0, 2 };
				SplitLine->StrokeThickness = 1.0f;
				SplitLine->Stroke = MakeRefer<FSolidColorBrush>(Themes::Default::GDefaultTheme.Shape.NormalColor);
			}
			UStackPanelRef SpeedPanel = MakeRefer<UStackPanel>();
			{
				TReferPtr<UTextBlock> SpeedLabel = MakeRefer<UTextBlock>(u8"Speed:"T);
				//Speed->Style = PerformanceItemTitleStyle;

				CurrentCpuFrequencyValue = MakeRefer<UTextBlock>(GetFrequencyFriendlyText(CpuPerformance.MaxMhz * 1000 * 1000));
				CurrentCpuFrequencyValue->Margin = { 8, 0, 0, 0 };
				SpeedPanel->Elements = { SpeedLabel, Separator({ 100pct, FDimen::Auto }), CurrentCpuFrequencyValue };
			}
			CpuPanel->Elements = { TitlePanel, SplitLine, SpeedPanel };
		}
		UStackPanelRef GpuPanel = MakeRefer<UStackPanel>();
		{
			GpuPanel->Margin = { 0, 8dim };
			TReferPtr<UTextBlock> Title = MakeRefer<UTextBlock>(u8"GPU"T);
			{
				Title->Style = PerformanceItemTitleStyle;
			}
			GpuPanel->Elements = { Title };
		}
		UStackPanelRef MemoryPanel = MakeRefer<UStackPanel>();
		{
			MemoryPanel->Margin = { 0, 8dim };
			TReferPtr<UTextBlock> Title = MakeRefer<UTextBlock>(u8"Memory"T);
			{
				Title->Style = PerformanceItemTitleStyle;
			}
			MemoryPanel->Elements = { Title };
		}

		MainPanel->Elements = { CpuPanel, GpuPanel, MemoryPanel };
		PerformanceSV->Child = MainPanel;
	}

	void FTaskMonitor::UpdatePerformance()
	{
		if (!PerformanceSV->Appeared)
			return;

		CpuPerformance.Update();

		//HQUERY hquery;
		//PdhOpenQueryW(nullptr, NULL, &hquery);
		//HCOUNTER hcounter;
		//PdhAddCounterW(hquery, L"\\Processor Information(_Total)\\% Processor Performance", NULL, &hcounter);
		//PdhCollectQueryData(hquery);
		//Sleep(200);
		//PdhCollectQueryData(hquery);
		//PDH_FMT_COUNTERVALUE value;
		//PdhGetFormattedCounterValue(hcounter, PDH_FMT_DOUBLE, nullptr, &value);

		//CurrentCpuFrequencyValue->Text = GetFrequencyFriendlyText(CpuPerformance.ActiveFrenquencyMhz * 1000 * 1000);
		CurrentCpuFrequencyValue->Text = FText(Format(u8"{:.4}%"V, (/*CpuPerformance.UserUsage + CpuPerformance.KernelUsage*/CpuPerformance.Utilization) * 100.0f));
	}


	void FTaskMonitor::CreateProcessListView()
	{
		ProcessLV = MakeRefer<UListView>();
		ProcessLV->ContextMenu = u8"我看看是怎么回事"T;
		ProcessLV->SortDescriptors =
		{
			{
				u8"Process"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					int32 Result = StringCompareIC<char8>(ProcessItemA.ProcessName, ProcessItemB.ProcessName);
					return Result ? (Result < 0) : (ProcessItemA.ProcessId < ProcessItemB.ProcessId);
				}
			},
			{
				u8"PID"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					return ProcessItemA.ProcessId < ProcessItemB.ProcessId;
				}
			},
			{
				u8"CPU"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					AssertExpr(!IsNaN(ProcessItemA.CPUUsage));
					AssertExpr(!IsNaN(ProcessItemB.CPUUsage));
					if (ProcessItemA.CycleTime.Delta == ProcessItemB.CycleTime.Delta)
						return ProcessItemA.ProcessId < ProcessItemB.ProcessId;
					return ProcessItemA.CycleTime.Delta > ProcessItemB.CycleTime.Delta;
				}
			},
			{
				u8"Memory"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					if (ProcessItemA.WorkingSetSize == ProcessItemB.WorkingSetSize)
						return ProcessItemA.ProcessId < ProcessItemB.ProcessId;
					return ProcessItemA.WorkingSetSize > ProcessItemB.WorkingSetSize;
				}
			},
			{
				u8"CD"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					int32 Result = StringCompareIC<char8>(ProcessItemA.CurrentDirectory, ProcessItemB.CurrentDirectory);
					return Result ? (Result < 0) : (ProcessItemA.ProcessId < ProcessItemB.ProcessId);
				}
			},
			{
				u8"Description"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					int32 Result = StringCompareIC<char8>(ProcessItemA.Description, ProcessItemB.Description);
					return Result ? (Result < 0) : (ProcessItemA.ProcessId < ProcessItemB.ProcessId);
				}
			},
			{
				u8"Company"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					int32 Result = StringCompareIC<char8>(ProcessItemA.CompanyName, ProcessItemB.CompanyName);
					return Result ? (Result < 0) : (ProcessItemA.ProcessId < ProcessItemB.ProcessId);
				}
			},
			{
				u8"Product"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					int32 Result = StringCompareIC<char8>(ProcessItemA.ProductName, ProcessItemB.ProductName);
					return Result ? (Result < 0) : (ProcessItemA.ProcessId < ProcessItemB.ProcessId);
				}
			},
			{
				u8"Version"N,
				[](const UListViewItemRef & A, const UListViewItemRef & B)
				{
					FProcessItem & ProcessItemA = A->Content.Get<FProcessItem&>();
					FProcessItem & ProcessItemB = B->Content.Get<FProcessItem&>();
					int32 Result = StringCompareIC<char8>(ProcessItemA.FileVersion, ProcessItemB.FileVersion);
					return Result ? (Result < 0) : (ProcessItemA.ProcessId < ProcessItemB.ProcessId);
				}
			},
		};
		//ProcessList->ResourceSheet.Resources.Add(MakeRefer<FProcessItemTemplate>());
		ProcessLV->Columns =
		{
			{ FProperty::None, u8"Process"T, FDimen::Auto, nullptr, MakeRefer<FProcessItemTemplate_IconAndName>(ShellItemImageCache), EElementAlignment::Near, u8"Process"N },
			//{ FProcessItem::IconProperty(), u8"Icon"T, FDimen::Auto, },
			//{ FProcessItem::ProcessNameProperty(), u8"Process"T },
			{ FProcessItem::ProcessIdProperty(), u8"PID"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Far, u8"PID"N },
			{ FProcessItem::ThreadCountProperty(), u8"Thread Count"T },
			{ { FProcessItem::CPUUsageProperty(), FCPUUsageConverter {} }, u8"CPU"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Far, u8"CPU"N },
			{ { FProcessItem::PagefileUsageProperty(), FByteSizeConverter {} }, u8"Private Bytes"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Far },
			{ { FProcessItem::WorkingSetSizeProperty(), FByteSizeConverter {} }, u8"Memory"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Far, u8"Memory"N },
			{ FProcessItem::DescriptionProperty(), u8"Description"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Near, u8"Description"N },
			{ FProcessItem::CompanyNameProperty(), u8"Company"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Near, u8"Company"N },
			{ FProcessItem::ProductNameProperty(), u8"Product"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Near, u8"Product"N },
			{ FProcessItem::FileVersionProperty(), u8"Version"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Near, u8"Version"N },
			{ FProcessItem::CurrentDirectoryProperty(), u8"Current Directory"T, FDimen::Auto, nullptr, nullptr, EElementAlignment::Near, u8"CD"N },
		};
		ProcessLV->OnQueryItemContextMenu += { this, &FTaskMonitor::QueryProcessItemContextMenu };
	}

	void FTaskMonitor::UpdateProcesses()
	{
		if (!ProcessLV->Appeared)
			return;

		bool FirstUpdate = ProcessItems.Empty();
		//if (!FirstUpdate)
		//	return;

		Internal::FSystemDynamicInfo SystemDynamicInfo;
		TList<Internal::FProcessDynamicInfo> DynamicInfos;
		Tie(SystemDynamicInfo, DynamicInfos) = Internal::FProcess::QueryProcessDynamicInfos();
		TMap<uint32, Internal::FProcessDynamicInfo *> MappedDynamicInfos;
		for (Internal::FProcessDynamicInfo & DynamicInfo : DynamicInfos)
			MappedDynamicInfos[DynamicInfo.ProcessId] = &DynamicInfo;

		uint64 TotalCycleTime = 0;
		TList<FProcessItemRef> ProcessItemsToAdd;
		TList<FProcessInfo> ProcessInfos = ProcessManager.EnumProcesses();
#if XIN_DEBUG
		ProcessInfos.Resize(20);
#else
		//ProcessInfos.Resize(50);
#endif

		TMap<uint32, FProcessInfo *> MappedProcessInfos;
		for (FProcessInfo & ProcessInfo : ProcessInfos)
		{
			MappedProcessInfos[ProcessInfo.ProcessId] = &ProcessInfo;

			if (!MappedProcessItems.Contains(ProcessInfo.ProcessId))
			{
				FProcessItemRef ProcessItem = MakeRefer<FProcessItem>();

				ProcessItem->ProcessId = ProcessInfo.ProcessId;
				ProcessItem->ProcessName = ProcessInfo.ProcessName;

				ProcessItem->FilePath = ProcessInfo.FilePath;
				ProcessItem->CommandLine = ProcessInfo.CommandLine;

				ProcessItem->ThreadCount = ProcessInfo.ThreadCount;
				ProcessItem->CurrentDirectory = ProcessInfo.CurrentDirectory;

				if (auto Iter = MappedDynamicInfos.Find(ProcessInfo.ProcessId); Iter != MappedDynamicInfos.End())
				{
					Internal::FProcessDynamicInfo & ProcessDynamicInfo = *Iter->Second;

					ProcessItem->WorkingSetSize = ProcessDynamicInfo.WorkingSetSize;
					ProcessItem->PagefileUsage = ProcessDynamicInfo.PagefileUsage;

					ProcessItem->CycleTime = ProcessDynamicInfo.CycleTime;
					TotalCycleTime += ProcessItem->CycleTime.Delta;
				}
				ProcessItemsToAdd.Add(ProcessItem);
			}
		}

		TList<FProcessItemRef> ProcessItemsToRemove;

		for (FProcessItemRef & ProcessItemToAdd : ProcessItemsToAdd)
		{
			if (!FirstUpdate)
				FConsole::WriteLine(u8"Add:[{}] {}"V, ProcessItemToAdd->ProcessId, ProcessItemToAdd->FilePath);

			if (const Internal::FFileVersion * FileVersion = ImageInfoCache.FetchFileVersion(ProcessItemToAdd->FilePath))
			{
				ProcessItemToAdd->Description = FileVersion->Blocks[Internal::EFileVersionBlock::FileDescription];
				ProcessItemToAdd->CompanyName = FileVersion->Blocks[Internal::EFileVersionBlock::CompanyName];
				ProcessItemToAdd->ProductName = FileVersion->Blocks[Internal::EFileVersionBlock::ProductName];
				ProcessItemToAdd->FileVersion = FileVersion->Blocks[Internal::EFileVersionBlock::FileVersion];
			}

			ProcessLV->Items.AddItem(ProcessItemToAdd);
			ProcessItems.Add(ProcessItemToAdd);
			MappedProcessItems.Insert({ ProcessItemToAdd->ProcessId, ProcessItemToAdd });
		}

		for (FProcessItemRef & ProcessItem : ProcessItems)
		{
			if (auto Iter = MappedProcessInfos.Find(ProcessItem->ProcessId); Iter != MappedProcessInfos.End())
			{
				FProcessInfo & ProcessInfo = *Iter->Second;

				// Don't update then.
				//ProcessItem->ProcessId = ProcessInfo.ProcessId;
				//ProcessItem->ProcessName = ProcessInfo.ProcessName;
				//ProcessItem->Icon = FetchModuleIcon(ProcessInfo.FilePath);

				//ProcessItem->FilePath = ProcessInfo.FilePath;
				//ProcessItem->CommandLine = ProcessInfo.CommandLine;

				ProcessItem->ThreadCount = ProcessInfo.ThreadCount;
				ProcessItem->CurrentDirectory = ProcessInfo.CurrentDirectory;

				if (auto DynamicInfoIter = MappedDynamicInfos.Find(ProcessInfo.ProcessId); DynamicInfoIter != MappedDynamicInfos.End())
				{
					Internal::FProcessDynamicInfo & ProcessDynamicInfo = *DynamicInfoIter->Second;

					ProcessItem->WorkingSetSize = ProcessDynamicInfo.WorkingSetSize;
					ProcessItem->PagefileUsage = ProcessDynamicInfo.PagefileUsage;

					ProcessItem->CycleTime = ProcessDynamicInfo.CycleTime;
					TotalCycleTime += ProcessItem->CycleTime.Delta;
				}
			}
			else
			{
				ProcessItemsToRemove.Add(ProcessItem);
			}
		}

		for (FProcessItemRef & ProcessItemToRemove : ProcessItemsToRemove)
		{
			if (!FirstUpdate)
				FConsole::WriteLine(u8"Remove:[{}] {}"V, ProcessItemToRemove->ProcessId, ProcessItemToRemove->FilePath);

			ProcessLV->Items.RemoveItem(ProcessItemToRemove);
			ProcessItems.Remove(ProcessItemToRemove);
			MappedProcessItems.Remove(ProcessItemToRemove->ProcessId);
		}

		// Update delta resource usage.
		AssertExpr(TotalCycleTime > 0);
		for (FProcessItemRef & ProcessItem : ProcessItems)
		{
			ProcessItem->CPUUsage = float32((float64)ProcessItem->CycleTime.Delta / (float64)TotalCycleTime);
		}

		ProcessLV->SortItems();
	}

	void FTaskMonitor::QueryProcessItemContextMenu(FListViewQueryItemContextMenuEventArgs & Args)
	{
		FProcessItemRef ProcessItem = Args.ListViewItem.Content.Get<FProcessItemRef>();
		if (!ProcessItem)
			return;

		Args.Menu = Menu({
			Format(u8"进程 [\"{}\"]"V, ProcessItem->ProcessName),
			MenuSaperator(),
			MenuItem(u8"结束任务(&E)"T),
			MenuItem(u8"提供反馈(&B)"T),
			MenuSaperator(),
			MenuItem(u8"设置优先级(&P)"T, nullptr,
				Menu({
					MenuItem(u8"实时(&R)"T),
					MenuItem(u8"高(&G)"T),
					MenuItem(u8"高于正常(&A)"T),
					MenuItem(u8"正常(&N)"T),
					MenuItem(u8"低于正常(&B)"T),
					MenuItem(u8"低(&L)"T),
				})
			),
			MenuItem(u8"设置相关性(&F)"T),
			MenuSaperator(),
			MenuItem(u8"打开文件所在的位置(&O)"T, [Item = TEntryPtr<FProcessItem>(ProcessItem)](const FRoutedEventArgs & Args)
			{
				FProcessItemRef ProcessItem = Item.Lock();
				if (!ProcessItem)
					return;

				//FWString FilePathW = Strings::ToWide(FileInfo->FilePath);
				FWString CommandLineParameters = FEncoding::ToWide(Format(u8"/select,\"{}\""V, ProcessItem->FilePath));
				CommandLineParameters.Slice(1).Replace(L'/', L'\\');

				SHELLEXECUTEINFOW ShellExecuteInfoW = {};

				ShellExecuteInfoW.cbSize = sizeof(ShellExecuteInfoW);
				ShellExecuteInfoW.lpFile = L"explorer.exe";
				ShellExecuteInfoW.nShow = SW_SHOW;
				ShellExecuteInfoW.fMask = SEE_MASK_DEFAULT;
				ShellExecuteInfoW.lpParameters = CommandLineParameters.Data;
				ShellExecuteInfoW.lpVerb = L"open";

				ShellExecuteExW(&ShellExecuteInfoW);
			}),
			MenuItem(u8"属性(&R)"T, [Item = TEntryPtr<FProcessItem>(ProcessItem)](const FRoutedEventArgs & Args)
			{
				//SHOpenPropSheetW()
				FProcessItemRef ProcessItem = Item.Lock();
				if (!ProcessItem)
					return;

				FWString FilePathW = FEncoding::ToWide(ProcessItem->FilePath);
				SHELLEXECUTEINFOW ShellExecuteInfoW = {};

				ShellExecuteInfoW.cbSize = sizeof(ShellExecuteInfoW);
				ShellExecuteInfoW.lpFile = FilePathW.Data;
				ShellExecuteInfoW.nShow = SW_SHOW;
				ShellExecuteInfoW.fMask = SEE_MASK_INVOKEIDLIST;
				ShellExecuteInfoW.lpVerb = L"properties";
				ShellExecuteExW(&ShellExecuteInfoW);
			})
		});
	}
}
