#include "PCH.h"

namespace H3D
{
	using namespace Xin;
	using namespace Xin::Engine;

	struct alignas(alignof(__m128)) FH3DAxisAlignedBox
	{
		float4 BoundingBoxMin;
		float4 BoundingBoxMax;
	};

    enum
    {
        attrib_mask_0 = (1 << 0),
        attrib_mask_1 = (1 << 1),
        attrib_mask_2 = (1 << 2),
        attrib_mask_3 = (1 << 3),
        attrib_mask_4 = (1 << 4),
        attrib_mask_5 = (1 << 5),
        attrib_mask_6 = (1 << 6),
        attrib_mask_7 = (1 << 7),
        attrib_mask_8 = (1 << 8),
        attrib_mask_9 = (1 << 9),
        attrib_mask_10 = (1 << 10),
        attrib_mask_11 = (1 << 11),
        attrib_mask_12 = (1 << 12),
        attrib_mask_13 = (1 << 13),
        attrib_mask_14 = (1 << 14),
        attrib_mask_15 = (1 << 15),

        // friendly name aliases
        attrib_mask_position = attrib_mask_0,
        attrib_mask_texcoord0 = attrib_mask_1,
        attrib_mask_normal = attrib_mask_2,
        attrib_mask_tangent = attrib_mask_3,
        attrib_mask_bitangent = attrib_mask_4,
    };

    enum
    {
        attrib_0 = 0,
        attrib_1 = 1,
        attrib_2 = 2,
        attrib_3 = 3,
        attrib_4 = 4,
        attrib_5 = 5,
        attrib_6 = 6,
        attrib_7 = 7,
        attrib_8 = 8,
        attrib_9 = 9,
        attrib_10 = 10,
        attrib_11 = 11,
        attrib_12 = 12,
        attrib_13 = 13,
        attrib_14 = 14,
        attrib_15 = 15,

        // friendly name aliases
        attrib_position = attrib_0,
        attrib_texcoord0 = attrib_1,
        attrib_normal = attrib_2,
        attrib_tangent = attrib_3,
        attrib_bitangent = attrib_4,

        maxAttribs = 16
    };

    enum
    {
        attrib_format_none = 0,
        attrib_format_ubyte,
        attrib_format_byte,
        attrib_format_ushort,
        attrib_format_short,
        attrib_format_float,

        attrib_formats
    };

    struct FH3DVertex
    {
        float3 Position;
        float2 Texcoord;
        float3 Normal;
        float3 Tangent;
        float3 BiTangent;
    };

    struct FH3DHeader
    {
        uint32 MeshCount;
        uint32 MaterialCount;
        uint32 VertexDataByteSize;
        uint32 IndexDataByteSize;
        uint32 VertexDataByteSizeDepth;
        //uint32 __Padding0;
        //uint32 __Padding1;
        //uint32 __Padding2;
        FH3DAxisAlignedBox BoundingBox;
    };

	struct FH3DAttrib
	{
		uint16 Offset; // byte offset from the start of the vertex
		uint16 Normalized; // if true, integer formats are interpreted as [-1, 1] or [0, 1]
		uint16 Components; // 1-4
		uint16 Format;
	};

	struct FH3DMesh
	{
		FH3DAxisAlignedBox BoundingBox;

		uint32 MaterialIndex;

		uint32 AttribsEnabled;
		uint32 AttribsEnabledDepth;
		uint32 VertexStride;
		uint32 VertexStrideDepth;
		FH3DAttrib Attributes[maxAttribs];
		FH3DAttrib AttributesDepth[maxAttribs];

		uint32 VertexDataByteOffset;
		uint32 VertexCount;
		uint32 IndexDataByteOffset;
		uint32 IndexCount;

		uint32 VertexDataByteOffsetDepth;
		uint32 VertexCountDepth;
	};

    constexpr int MaxTexturePathLength = 128;
    constexpr int MaxMaterialNameLength = 128;

    struct alignas(alignof(__m128)) FH3DMaterial
    {
        float4 Diffuse;
        float4 Specular;
        float4 Ambient;
        float4 Emissive;
        float4 Transparent; // light passing through a transparent surface is multiplied by this filter color

        float Opacity;
        float Shininess; // specular exponent
        float SpecularStrength; // multiplier on top of specular color

        char8_t TextureDiffusePath[MaxTexturePathLength];
        char8_t TextureSpecularPath[MaxTexturePathLength];
        char8_t TextureEmissivePath[MaxTexturePathLength];
        char8_t TextureNormalPath[MaxTexturePathLength];
        char8_t TextureLightmapPath[MaxTexturePathLength];
        char8_t TextureReflectionPath[MaxTexturePathLength];

        char8_t Name[MaxMaterialNameLength];
    };

	class FH3DModel
	{
	public:
		FH3DModel();

		bool Load(FStringV FilePath);

	public:
        FH3DHeader Header;
        TList<FH3DMesh> Meshs;
        TList<FH3DMaterial> Materials;
        TList<byte> BinaryData;
        TView<FH3DVertex> Vertices;
        TView<uint16> Indices;

        uint32 VertexStride = 0;
        uint32 VertexStrideDepth = 0;
	};

    EError ImportH3DModel(FStringV H3DFilePath, FStringV AssetsPath, FStringV AssetFolder);
}
