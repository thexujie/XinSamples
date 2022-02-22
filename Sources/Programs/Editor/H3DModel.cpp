#include "PCH.h"
#include "H3DModel.h"

#include "LocalAsset.h"

namespace H3D
{
	FH3DModel::FH3DModel()
	{

	}

	bool FH3DModel::Load(FStringV FilePath)
	{
		FFileStream FileStream(FilePath, EFileModes::Input);

		FileStream.ReadData(&Header, sizeof(FH3DHeader));

		Meshs.Resize(Header.MeshCount, EInitializeMode::Zero);
		FileStream.ReadData(Meshs.Data, sizeof(FH3DMesh) * Header.MeshCount);

		Materials.Resize(Header.MaterialCount, EInitializeMode::Zero);
		FileStream.ReadData(Materials.Data, sizeof(FH3DMaterial) * Header.MaterialCount);

        VertexStride = Meshs[0].VertexStride;
        VertexStrideDepth = Meshs[0].VertexStrideDepth;
#if _DEBUG
        for (uint32 MeshIndex = 1; MeshIndex < Header.MeshCount; ++MeshIndex)
        {
            const FH3DMesh & Mesh = Meshs[MeshIndex];
            AssertExpr(Mesh.VertexStride == VertexStride);
            AssertExpr(Mesh.VertexStrideDepth == VertexStrideDepth);

            AssertExpr(Mesh.AttribsEnabled == (attrib_mask_position | attrib_mask_texcoord0 | attrib_mask_normal | attrib_mask_tangent | attrib_mask_bitangent));
            AssertExpr(Mesh.Attributes[0].Components == 3 && Mesh.Attributes[0].Format == H3D::attrib_format_float); // position
            AssertExpr(Mesh.Attributes[1].Components == 2 && Mesh.Attributes[1].Format == H3D::attrib_format_float); // texcoord0
            AssertExpr(Mesh.Attributes[2].Components == 3 && Mesh.Attributes[2].Format == H3D::attrib_format_float); // normal
            AssertExpr(Mesh.Attributes[3].Components == 3 && Mesh.Attributes[3].Format == H3D::attrib_format_float); // tangent
            AssertExpr(Mesh.Attributes[4].Components == 3 && Mesh.Attributes[4].Format == H3D::attrib_format_float); // bitangent

            AssertExpr(Mesh.AttribsEnabledDepth == (attrib_mask_position));
            AssertExpr(Mesh.Attributes[0].Components == 3 && Mesh.Attributes[0].Format == H3D::attrib_format_float); // position
        }
#endif

        uint32 TotalBinarySize = Header.VertexDataByteSize
            + Header.IndexDataByteSize
            + Header.VertexDataByteSizeDepth
            + Header.IndexDataByteSize;

        BinaryData.Resize(TotalBinarySize, EInitializeMode::None);
		uintx ReadBinarySize = FileStream.Read(BinaryData.Data, TotalBinarySize);
        AssertExpr(ReadBinarySize == TotalBinarySize);
        TView<FH3DVertex> TypedVertices = TView<FH3DVertex>((const FH3DVertex *)BinaryData.Data, Header.VertexDataByteSize / VertexStride);
        TView<uint16> TypedIndices = TView<uint16>((const uint16 *)(BinaryData.Data + Header.VertexDataByteSize), Header.IndexDataByteSize / sizeof(uint16));

        TView<FVector3> TypedDepthVertices = TView<FVector3>((const FVector3 *)(BinaryData.Data + Header.VertexDataByteSize + Header.IndexDataByteSize), Header.VertexDataByteSizeDepth / sizeof(FVector3));
        TView<uint16> TypedDepthIndices = TView<uint16>((const uint16 *)(BinaryData.Data + Header.VertexDataByteSize + Header.IndexDataByteSize + Header.VertexDataByteSizeDepth), Header.IndexDataByteSize / sizeof(uint16));

		Vertices = { (const FH3DVertex *)BinaryData.Data, Header.VertexDataByteSize / sizeof(FH3DVertex)};
		Indices = { (const uint16 *)(BinaryData.Data + Header.VertexDataByteSize), Header.IndexDataByteSize / 2 };
        uint16 MaxIndex = 0;
        for (auto DepthIndex : TypedDepthIndices)
        {
            if (DepthIndex > MaxIndex)
                MaxIndex = DepthIndex;
        }

		return true;
	}

	EError ImportH3DModel(FStringV H3DFilePath, FStringV AssetsPath, FStringV AssetFolder)
	{
		FString ModelName = FPath::GetBaseName(H3DFilePath);
		FString SourceFolderPath = FPath::GetDirectory(H3DFilePath);
		FString TargetFolderPath = FPath::GetAbsolutePath(FPath::Combine(AssetsPath, AssetFolder));

		FDirectory::EnsureExists(TargetFolderPath);

		H3D::FH3DModel H3DModel;
		H3DModel.Load(H3DFilePath);

		TList<FString> MaterialAssetPaths(H3DModel.Materials.Size);
		// Materials
		{
			for (uintx MaterialIndex = 0; MaterialIndex < H3DModel.Materials.Size; ++MaterialIndex)
			{
				const H3D::FH3DMaterial & H3DMaterial = H3DModel.Materials[MaterialIndex];
				if (!H3DMaterial.TextureDiffusePath[0])
					continue;

				FStringV MaterialName = H3DMaterial.Name;
				FString XmtlAssetPath = FPath::Combine(AssetFolder, MaterialName + u8".xmtl"V);

				FString ModelBaseName = FPath::GetBaseName(H3DMaterial.TextureDiffusePath);
				TList<TPair<FString, FString>> Parameters;
				TList<TPair<FString, FString>> Definetions;
				// Diffuse
				{
					FString SourceFilePath = FPath::Combine(SourceFolderPath, ModelBaseName + u8".dds"V);
					if(!FFile::Exists(SourceFilePath))
						SourceFilePath = FPath::Combine(SourceFolderPath, ModelBaseName + u8".tga"V);

					FString AssetFileName = ModelBaseName + u8"_D.xtex";
					ImportTexture(SourceFilePath, AssetsPath, AssetFolder, AssetFileName, true);
					Parameters.Add({ u8"DiffuseTexture"V, AssetsPrefix + FPath::Combine(AssetFolder, AssetFileName) });
				}

				// Normal
				if (FString SourceFilePath = FPath::Combine(SourceFolderPath, ModelBaseName + u8"_normal.dds"V); FFile::Exists(SourceFilePath))
				{
					if(!FFile::Exists(SourceFilePath))
						SourceFilePath = FPath::Combine(SourceFolderPath, ModelBaseName + u8"_normal.tga"V);

					FString AssetFileName = ModelBaseName + u8"_N.xtex";
					ImportTexture(SourceFilePath, AssetsPath, AssetFolder, AssetFileName, false);
					Parameters.Add({ u8"NormalTexture"V, AssetsPrefix + FPath::Combine(AssetFolder, AssetFileName) });
				}
				else
					Definetions.Add({ u8"SPONZA_NORMAL_TEXTURE"V, u8"0"V });

				// Specular
				if (FString SourceFilePath = FPath::Combine(SourceFolderPath, ModelBaseName + u8"_specular.dds"V); FFile::Exists(SourceFilePath))
				{
					if(!FFile::Exists(SourceFilePath))
						SourceFilePath = FPath::Combine(SourceFolderPath, ModelBaseName + u8"_specular.tga"V);

					FString AssetFileName = ModelBaseName + u8"_S.xtex";
					ImportTexture(SourceFilePath, AssetsPath, AssetFolder, AssetFileName, false);
					Parameters.Add({ u8"SpecularTexture"V, AssetsPrefix + FPath::Combine(AssetFolder, AssetFileName) });
				}
				else
					Definetions.Add({ u8"SPONZA_SPECULAR_TEXTURE"V, u8"0"V });

				if (ModelBaseName.Contains(u8"thorn"V) || ModelBaseName.Contains(u8"plant"V) || ModelBaseName.Contains(u8"chain"V))
					Definetions.Add({ u8"SPONZA_MASKED"V, u8"1"V });

				MaterialAssetPaths[MaterialIndex] = XmtlAssetPath;
				ImportMaterial(SponzaDefaultMaterialAssetPath, FPath::Combine(AssetsPath, XmtlAssetPath), Parameters, Definetions);
			}
		}

		// StaticMesh
		{
			EMeshDataComponets MeshDataComponets = EMeshDataComponets::Position | EMeshDataComponets::Normal
				| EMeshDataComponets::Texcoord | EMeshDataComponets::Tangent | EMeshDataComponets::BiTangent;

			FString XmshFilePath = FPath::Combine(TargetFolderPath, ModelName + u8".xmsh"V);
			Engine::FStaticMeshData StaticMeshData;
			StaticMeshData.Components = MeshDataComponets;

			StaticMeshData.Positions.Resize(H3DModel.Vertices.Size, EInitializeMode::None);
			StaticMeshData.Normals.Resize(H3DModel.Vertices.Size, EInitializeMode::None);
			StaticMeshData.Vertices.Resize(H3DModel.Vertices.Size, EInitializeMode::None);

			for (uintx VertexIndex = 0; VertexIndex < H3DModel.Vertices.Size; ++VertexIndex)
			{
				StaticMeshData.Positions[VertexIndex] = H3DModel.Vertices[VertexIndex].Position;
				StaticMeshData.Normals[VertexIndex] = H3DModel.Vertices[VertexIndex].Normal;
				StaticMeshData.Vertices[VertexIndex].Texcoord = H3DModel.Vertices[VertexIndex].Texcoord;
				StaticMeshData.Vertices[VertexIndex].Tangent = H3DModel.Vertices[VertexIndex].Tangent;
				StaticMeshData.Vertices[VertexIndex].BiTangent = H3DModel.Vertices[VertexIndex].BiTangent;
			}

			StaticMeshData.IndexFormat = EIndexFormat::Index16;
			StaticMeshData.Indices16 = H3DModel.Indices;
			StaticMeshData.Elements.Resize(H3DModel.Meshs.Size);

			FJsonObject OutputJson;
			OutputJson[u8"Version"V].Value = u8"1.0"V;
			OutputJson[u8"Elements"V].Value = new FJsonArray();

			FJsonArray & Elements = OutputJson[u8"Elements"V].Value.Get<FJsonArray &>();

			for (uintx ElementIndex = 0; ElementIndex < H3DModel.Meshs.Size; ++ElementIndex)
			{
				const H3D::FH3DMesh & H3DMesh = H3DModel.Meshs[ElementIndex];

				StaticMeshData.Elements[ElementIndex].Name;
				StaticMeshData.Elements[ElementIndex].VertexOffset = H3DMesh.VertexDataByteOffset / sizeof(H3D::FH3DVertex);
				StaticMeshData.Elements[ElementIndex].VertexCount = H3DMesh.VertexCount;
				StaticMeshData.Elements[ElementIndex].IndexOffset = H3DMesh.IndexDataByteOffset / 2;
				StaticMeshData.Elements[ElementIndex].IndexCount = H3DMesh.IndexCount;

				FJsonObject & Element = Elements.Items.Add(new FJsonObject()).Get<FJsonObject &>();
				Element[u8"Material"V].Value = AssetsPrefix + MaterialAssetPaths[H3DMesh.MaterialIndex];
			}

			{
				FFileStream XxxxStream { XmshFilePath, EFileModes::Output };
				XxxxStream << OutputJson;

				FFileStream DataStream { XmshFilePath + u8".data"V, EFileModes::Output };
				FAssetHeader AssetHeader = { AssetMagic, AssetVersion, EAssetType::Mesh };
				DataStream << AssetHeader;
				DataStream << StaticMeshData;
			}
		}

		// Textures
		{

		}

		return EError::OK;
	}
}
