#pragma once

#include "EngineTypes.h"
#include "SceneRenderer.h"
#include "Resource.h"
#include "ResourceAllocator.h"

namespace Xin::Engine
{
	class ENGINE_API FDefferedSceneRenderer : public ISceneRenderer
	{
	public:
		FDefferedSceneRenderer(FEngine & Engine);

		void Render(FRenderGraph & RenderGraph, FResource * BackBuffer) override;

	public:
		FEngine & Engine;
		FResourceRef SceneExposure;
		TList<TReferPtr<FTransientAllocator>> TransientAllocators;
	};
}
