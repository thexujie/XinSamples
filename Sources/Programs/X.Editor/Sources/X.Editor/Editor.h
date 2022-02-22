#pragma once

#include "X.ModelViewer.Types.h"

namespace X::ModelViewer
{
	class FModelViewer : public IObject
	{
	public:
		FModelViewer(IGraphics & Graphics);

		int32 Loop();

	public:
		IGraphics & Graphics;

	public:
		FResourceTableRef GlobalResourceTable = MakeRefer<FResourceTable>();
		UWindowRef MainWindow;
	};
}
