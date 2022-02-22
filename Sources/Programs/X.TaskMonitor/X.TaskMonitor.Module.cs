using ProjectModel;
using System.IO;

namespace Xin;

public class Module : IModule
{
    public Module()
    {
        Type = ModuleType.Application;
        PublicDependencyModules.AddRange(new[] { "Core", "UI", "Windows", "Direct2D", "Skia", "RHI.D3D12", "RHI.Vulkan", "Rendering", "Windows.Informer" });
    }
}
