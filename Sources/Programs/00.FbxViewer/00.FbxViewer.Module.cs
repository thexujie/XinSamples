using ProjectModel;

namespace Xin;

public class Module : IModule
{
    public Module()
    {
        Type = ModuleType.Application;

        PublicDependencyModules.AddRange(new[] { "Core", "RHI", "Rendering", "RHI.D3D12" });

        PublicDependencyModules.AddRange(new[] { "Xin.FBX" });

        AdditionalFiles.Add("Assets/*");
    }
}
