using ProjectModel;

namespace Xin;

public class Module : IModule
{
    public Module()
    {
        Type = ModuleType.Application;
        PublicDependencyModules.AddRange(new[] { "Core", "UI" });
        
        PublicDependencyModules.AddRange(new[] { "Direct2D" });
    }
}
