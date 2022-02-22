#include "PCH.h"
#include "SDKMesh.h"
#include "H3DModel.h"
#include "LocalAsset.h"
#include "Xin.FBX/Xin.FBX.h"

EError ImportMesh(FStringV Source, FStringV Dest, TView<FStringV> Materials, const FMatrix & Transform = FMatrix::Identity)
{
	FBX::FFbxModel FbxModel = FBX::LoadStaticMeshFromFile(Source, Transform);

	FStaticMeshData StaticMeshData;
	{
		StaticMeshData.Components = EMeshDataComponets::Position | EMeshDataComponets::Normal | EMeshDataComponets::Texcoord | EMeshDataComponets::Tangent | EMeshDataComponets::BiTangent;

		const FBX::FFbxMesh & FbxMesh0 = FbxModel.LodMeshs[0];
		StaticMeshData.Positions.ResizeTo(FbxMesh0.Vertices.Size, EInitializeMode::None);
		StaticMeshData.Normals.ResizeTo(FbxMesh0.Vertices.Size, EInitializeMode::None);
		StaticMeshData.Vertices.ResizeTo(FbxMesh0.Vertices.Size, EInitializeMode::Zero);
		for (uintx VertexIndex = 0; VertexIndex < FbxMesh0.Vertices.Size; ++VertexIndex)
		{
			StaticMeshData.Positions[VertexIndex] = FbxMesh0.Vertices[VertexIndex].Position;
			StaticMeshData.Normals[VertexIndex] = FbxMesh0.Vertices[VertexIndex].Normal;
			StaticMeshData.Vertices[VertexIndex].Texcoord = FbxMesh0.Vertices[VertexIndex].UV;
			//StaticMeshData.Vertices[VertexIndex].Tangent = FbxMesh0.Vertices[VertexIndex].BiNormal;
		}
		StaticMeshData.IndexFormat = FbxMesh0.Indices.Index32 ? EIndexFormat::Index32 : EIndexFormat::Index16;
		if (FbxMesh0.Indices.Index32)
			StaticMeshData.Indices16 = FbxMesh0.Indices.Indices16;
		else
			StaticMeshData.Indices32 = FbxMesh0.Indices.Indices32;

		StaticMeshData.Elements.ResizeTo(FbxMesh0.Elements.Size, EInitializeMode::Zero);
		for (uintx ElementIndex = 0; ElementIndex < FbxMesh0.Elements.Size; ++ElementIndex)
		{
			const FBX::FFbxMeshElement & FbxMeshElement = FbxMesh0.Elements[ElementIndex];
			FStaticMeshElement & StaticMeshElement = StaticMeshData.Elements[ElementIndex];
			StaticMeshElement.Name = FbxMeshElement.NodeName;
			StaticMeshElement.VertexOffset = FbxMeshElement.VertexOffset;
			StaticMeshElement.VertexCount = FbxMeshElement.IndexCount * 3;
			StaticMeshElement.IndexOffset = FbxMeshElement.IndexOffset;
			StaticMeshElement.IndexCount = FbxMeshElement.IndexCount;
		}
	}
	FJsonObject OutputJson;
	OutputJson[u8"Version"V].Value = u8"1.0"V;
	OutputJson[u8"Elements"V].Value = new FJsonArray();

	FJsonArray & Elements = OutputJson[u8"Elements"V].Value.Get<FJsonArray &>();
	for (uintx ElementIndex = 0; ElementIndex < Materials.Size; ++ElementIndex)
	{
		FJsonObject & Element = Elements.Items.Add(new FJsonObject()).Get<FJsonObject &>();
		Element[u8"Material"V].Value = Materials[ElementIndex];
	}

	FLocalAsset LocalAsset { Dest };
	IDataStream & OutputStream = LocalAsset.Write(OutputJson);
	FAssetHeader AssetHeader = { AssetMagic, AssetVersion, EAssetType::Mesh };
	OutputStream << AssetHeader;
	OutputStream << StaticMeshData;
	return EError::OK;
}

int main(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(399);
	
	//IImage::Load(u8R"(D:\Projects\DirectX-Graphics-Samples\MiniEngine\ModelViewer\Sponza\gi_flag.DDS)"V);
	//SDK::ImportSDKMesh(u8R"(D:\Projects\DirectX-SDK-Samples\Media\powerplant\powerplant.sdkmesh)", u8"../../Assets/PowerPlant"V);
	//return 0;
	//H3D::ImportH3DModel(u8R"(D:\Projects\DirectX-Graphics-Samples\MiniEngine\ModelViewer\Sponza\pbr\sponza.h3d)", u8"../../Assets"V, u8"Sponza"V);
	//H3D::ImportH3DModel(u8R"(D:\Projects\DirectX-Graphics-Samples\MiniEngine\ModelViewer\Sponza\sponza.h3d)", u8"../../Assets"V, u8"Sponza"V);
	H3D::ImportH3DModel(u8R"(D:\Projects\DirectX-Graphics-Samples\MiniEngine\ModelViewer\Sponza\sponza.h3d)", u8"../../Assets"V, u8"SponzaPBR"V);
	return 0;

	//ImportMesh(Str("../../.Data/Cube.fbx"), Str("../../Assets/Cube.xmsh"));
	{
		//ImportTexture(u8"../../Data/Wolf_Body.jpg"V, u8"../../Assets/Wolf/Wolf_Body.xtex"V);
		//ImportTexture(u8"../../Data/Wolf_Eyes.jpg"V, u8"../../Assets/Wolf/Wolf_Eyes.xtex"V);
		//ImportTexture(u8"../../Data/Wolf_Fur.jpg"V, u8"../../Assets/Wolf/Wolf_Fur.xtex"V);

		//ImportMaterial(DefaultMaterialShader, u8"../../Assets/Wolf/Wolf_Body.xmtl"V, u8"assets://Wolf/Wolf_Body.xtex"V);
		//ImportMaterial(DefaultMaterialShader, u8"../../Assets/Wolf/Wolf_Eyes.xmtl"V, u8"assets://Wolf/Wolf_Eyes.xtex"V);
		//ImportMaterial(DefaultMaterialShader, u8"../../Assets/Wolf/Wolf_Fur.xmtl"V, u8"assets://Wolf/Wolf_Fur.xtex"V);

		ImportMesh(u8"../../Data/Wolf.fbx"V, u8"../../Assets/Wolf/Wolf.xmsh"V,
			{
				u8"assets://Wolf/Wolf_Body.xmtl"V,
				u8"assets://Wolf/Wolf_Eyes.xmtl"V,
				u8"assets://Wolf/Wolf_Fur.xmtl"V
			},
			FMatrix::Scale(10, 10, 10));
	}

	//ImportTexture(Str("../../Data/Cat.jpg"), Str("../Assets/Cat.xtex"));

	//ImportMaterial(Str("Assets/Shaders/Default.xsf"), Str("../Assets/Default.xmtl"));
	//ImportMaterial(Str("Assets/Shaders/BaseColor.xsf"), Str("../Assets/BaseColor.xmtl"));
	return 0;
}
