using ProjectModel;

namespace Xin;

public class Module : IModule
{
    public Module()
    {
        Type = ModuleType.Application;
        PublicDependencyModules.AddRange(new[] { "Engine", "RHI.D3D12", "RHI.Vulkan", "Direct2D", "Skia", "UI" });

        PublicDependencyModules.Add("00.Test.UI");
    }
}
