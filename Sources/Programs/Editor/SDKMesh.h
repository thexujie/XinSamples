#pragma once

using namespace Xin;
using namespace Xin::Engine;

namespace SDK
{
	class FSDKMesh
	{
	public:
		void LoadFromFile(FStringV FilePath);

	public:
		FStaticMeshData StaticMeshData;
		TList<uint32> MaterialIndices;

		struct FMeshMaterial
		{
			FString Name;
			FString DiffuseTexture;
		};
		TList<FMeshMaterial> Materials;
	};

	EError ImportSDKMesh(FStringV Source, FStringV DestFolderPath, const FTransform & Transform = FTransform::Identity);

}