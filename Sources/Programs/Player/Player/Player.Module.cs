using ProjectModel;

namespace Xin;

public class Module : IModule
{
    public Module()
    {
        Type = ModuleType.DynamicLibrary;
        PublicDependencyModules.AddRange(new[] { "Core" });
    }
}
