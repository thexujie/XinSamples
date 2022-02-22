#include "PCH.h"
#include "SDKMesh.h"

#include "LocalAsset.h"

#include "Xin.Core/Platform/Windows/WindowsPrivate.h"
#include "SDKMeshMisc.h"

namespace SDK
{
	struct FSDKMeshVertex
	{
		FVector3 Position;
		FVector3 Normal;
		FVector2 UV;
		//FVector3 Tangent;
		//FVector3 BiTangent;
	};

	void FSDKMesh::LoadFromFile(FStringV FilePath)
	{
		FMatrix Transform = FMatrix::Scale(10, 10, 10);
		TList<byte> FileContent = FFile::ReadBytes(FilePath);
		SDKMESH_HEADER & MeshHeader = InterpretCastRef<SDKMESH_HEADER>(FileContent.Data);
		if (MeshHeader.Version != SDKMESH_FILE_VERSION)
			return;

		TView<SDKMESH_VERTEX_BUFFER_HEADER> SDKVertexBufferHeaders = { (SDKMESH_VERTEX_BUFFER_HEADER *)(FileContent.Data + MeshHeader.VertexStreamHeadersOffset), MeshHeader.NumVertexBuffers };
		TView<SDKMESH_INDEX_BUFFER_HEADER> SDKIndexBufferHeaders = { (SDKMESH_INDEX_BUFFER_HEADER *)(FileContent.Data + MeshHeader.IndexStreamHeadersOffset), MeshHeader.NumIndexBuffers };
		TView<SDKMESH_MESH> SDKMeshs = { (SDKMESH_MESH *)(FileContent.Data + MeshHeader.MeshDataOffset), MeshHeader.NumMeshes };
		TView<SDKMESH_SUBSET> SDKSubsets = { (SDKMESH_SUBSET *)(FileContent.Data + MeshHeader.SubsetDataOffset), MeshHeader.NumTotalSubsets };
		TView<SDKMESH_FRAME> SDKFrames = { (SDKMESH_FRAME *)(FileContent.Data + MeshHeader.FrameDataOffset), MeshHeader.NumFrames };
		TView<SDKMESH_MATERIAL> SDKMaterials = { (SDKMESH_MATERIAL *)(FileContent.Data + MeshHeader.MaterialDataOffset), MeshHeader.NumMaterials };

		BYTE * BufferData = FileContent.Data + MeshHeader.HeaderSize + MeshHeader.NonBufferDataSize;
		UINT64 BufferDataStart = MeshHeader.HeaderSize + MeshHeader.NonBufferDataSize;

		struct FSDKBufferSpan
		{
			uint64 Offset;
			uint64 Count;
		};

		TList<FSDKBufferSpan> SDKVertexBuffers(MeshHeader.NumVertexBuffers, EInitializeMode::Zero);
		for (UINT VertexBufferIndex = 0; VertexBufferIndex < MeshHeader.NumVertexBuffers; VertexBufferIndex++)
		{
			const SDKMESH_VERTEX_BUFFER_HEADER & SDKVertexBufferHeader = SDKVertexBufferHeaders[VertexBufferIndex];
			AssertExpr(SDKVertexBufferHeader.SizeBytes == SDKVertexBufferHeader.StrideBytes * SDKVertexBufferHeader.NumVertices);
			
			const byte * Vertices = BufferData + (SDKVertexBufferHeader.DataOffset - BufferDataStart);
			uintx VertexBaseIndex = StaticMeshData.Vertices.Size;
			SDKVertexBuffers[VertexBufferIndex] = { VertexBaseIndex, SDKVertexBufferHeader.NumVertices };

			StaticMeshData.Positions.Resize(VertexBaseIndex + SDKVertexBufferHeader.NumVertices);
			StaticMeshData.Normals.Resize(VertexBaseIndex + SDKVertexBufferHeader.NumVertices);
			StaticMeshData.Vertices.Resize(VertexBaseIndex + SDKVertexBufferHeader.NumVertices);
			for (uintx VertexIndex = 0; VertexIndex < SDKVertexBufferHeader.NumVertices; ++VertexIndex)
			{
				const FSDKMeshVertex & SDKMeshVertex = InterpretCastRef<FSDKMeshVertex>(Vertices + SDKVertexBufferHeader.StrideBytes * VertexIndex);
				StaticMeshData.Positions[VertexBaseIndex + VertexIndex] = Transform.TransformCoord(SDKMeshVertex.Position);
				StaticMeshData.Normals[VertexBaseIndex + VertexIndex] = SDKMeshVertex.Normal;
				StaticMeshData.Vertices[VertexBaseIndex + VertexIndex].Texcoord = SDKMeshVertex.UV;
			}
		}

		StaticMeshData.IndexFormat = EIndexFormat::None;
		TList<FSDKBufferSpan> SDKIndexBuffers(MeshHeader.NumIndexBuffers, EInitializeMode::Zero);
		for (UINT IndexBufferIndex = 0; IndexBufferIndex < MeshHeader.NumIndexBuffers; IndexBufferIndex++)
		{
			EIndexFormat IndexFormat = SDKIndexBufferHeaders[IndexBufferIndex].IndexType == IT_16BIT ? EIndexFormat::Index16 : EIndexFormat::Index32;
			AssertExpr(StaticMeshData.IndexFormat == EIndexFormat::None || StaticMeshData.IndexFormat == IndexFormat);
			StaticMeshData.IndexFormat = IndexFormat;

			if (IndexFormat == EIndexFormat::Index16)
			{
				TView<uint16> SDKIndices { (uint16 *)(BufferData + (SDKIndexBufferHeaders[IndexBufferIndex].DataOffset - BufferDataStart)), SDKIndexBufferHeaders[IndexBufferIndex].NumIndices };
				uintx IndexBaseIndex = StaticMeshData.Indices16.Size;
				SDKIndexBuffers[IndexBufferIndex] = { IndexBaseIndex, SDKIndices.Size };
				StaticMeshData.Indices16.Reserve(IndexBaseIndex + SDKIndices.Size);
				StaticMeshData.Indices16.AddRange(SDKIndices);
			}
			else
			{
				TView<uint32> SDKIndices { (uint32 *)(BufferData + (SDKIndexBufferHeaders[IndexBufferIndex].DataOffset - BufferDataStart)), SDKIndexBufferHeaders[IndexBufferIndex].NumIndices };
				uintx IndexBaseIndex = StaticMeshData.Indices32.Size;
				SDKIndexBuffers[IndexBufferIndex] = { IndexBaseIndex, SDKIndices.Size };
				StaticMeshData.Indices32.Reserve(IndexBaseIndex + SDKIndices.Size);
				StaticMeshData.Indices32.AddRange(SDKIndices);
			}
		}

		StaticMeshData.Elements.Reserve(MeshHeader.NumMeshes);
		MaterialIndices.Reserve(MeshHeader.NumMeshes);

		for (UINT MeshIndex = 0; MeshIndex < MeshHeader.NumMeshes; MeshIndex++)
		{
			const SDKMESH_MESH & SDKMesh = SDKMeshs[MeshIndex];
			AssertExpr(SDKMesh.NumVertexBuffers == 1);
			TView<UINT> MeshSubsetIndices = { (UINT *)(FileContent.Data + SDKMeshs[MeshIndex].SubsetOffset), SDKMesh.NumSubsets };
			for (UINT SubsetIndex = 0; SubsetIndex < SDKMesh.NumSubsets; SubsetIndex++)
			{
				const SDKMESH_SUBSET & SDKSubset = SDKSubsets[MeshSubsetIndices[SubsetIndex]];
				AssertExpr(SDKSubset.PrimitiveType == SDKMESH_PRIMITIVE_TYPE::PT_TRIANGLE_LIST);
				const FSDKBufferSpan & SDKVertexBuffer = SDKVertexBuffers[SDKMesh.VertexBuffers[0]];
				const FSDKBufferSpan & SDKIndexBuffer = SDKIndexBuffers[SDKMesh.IndexBuffer];

				AssertExpr(SDKSubset.MaterialID != INVALID_MATERIAL);
				MaterialIndices.Add(SDKSubset.MaterialID);
				FStaticMeshElement & StaticMeshElement = StaticMeshData.Elements.AddDefault();

				StaticMeshElement.Name = FEncoding::ToUtf8(FAStringV(SDKSubset.Name));
				StaticMeshElement.VertexOffset = SDKVertexBuffer.Offset + SDKSubset.VertexStart;
				StaticMeshElement.VertexCount = SDKSubset.VertexCount;
				StaticMeshElement.IndexOffset = SDKIndexBuffer.Offset + SDKSubset.IndexStart;
				StaticMeshElement.IndexCount = SDKSubset.IndexCount;
			}
		}

		Materials.Resize(MeshHeader.NumMaterials, EInitializeMode::Zero);
		for (UINT MaterialIndex = 0; MaterialIndex < MeshHeader.NumMaterials; MaterialIndex++)
		{
			const SDKMESH_MATERIAL & SDKMaterial = SDKMaterials[MaterialIndex];
			FMeshMaterial & MeshMaterial = Materials[MaterialIndex];
			MeshMaterial.Name = FEncoding::ToUtf8(FAStringV { SDKMaterial.Name });
			MeshMaterial.DiffuseTexture = FEncoding::ToUtf8(FAStringV { SDKMaterial.DiffuseTexture });
		}
	}
}