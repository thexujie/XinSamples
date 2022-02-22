#include "PCH.h"
#include "ModelViewer.h"

namespace X::ModelViewer
{
	FModelViewer::FModelViewer(IGraphics & Graphics) : Graphics(Graphics)
	{
		//Internal::FFileVersion FileVersion;
		//FileVersion.LoadFromFile(u8R"(C:\Windows\System32\svchost.exe)"V);

		GlobalResourceTable->Resources.AddRange(Themes::Default::LoadDefaultThemesResources());

		IWindowContextRef WindowContext = IWindowContext::CreateDefault(Graphics);
		MainWindow = MakeRefer<UWindow>(u8"X Model Viewer"T, nullptr, WindowContext);
		MainWindow->ResourceTable.Tables.Add(GlobalResourceTable);

		MainWindow->Size = { 1920, 1080 };
		MainWindow->WindowShowPosition = EShowPosition::ScreenCenter;
	}

	int32 FModelViewer::Loop()
	{
		MainWindow->ShowWindow();
		//MainWindow.FindChild(u8"ButtonB"N)->MouseMove += [&](const FMouseEventArgs & Args)
		//{
			//FConsole::WriteLineF(u8"MouseMove {}"V, Args.Position);
		//};

		MainWindow->Closed = [this](const auto & Args)
		{
			FApplication::Instance().Exit(0);
		};

		return FApplication::Instance().Loop();
	}
}
