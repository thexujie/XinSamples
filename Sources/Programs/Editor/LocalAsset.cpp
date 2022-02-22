#include "PCH.h"
#include "LocalAsset.h"

EError ImportTexture(FStringV SourceFilePath, FStringV AssetsPath, FStringV AssetFolder, FStringV AssetFileName, bool sRGB)
{
	FString AssetBaseName = FPath::GetBaseName(AssetFileName);
	FString XtexFilePath = FPath::Combine(AssetsPath, AssetFolder, AssetFileName);

	FJsonObject OutputJson;
	OutputJson[u8"Version"V].Value = u8"1.0"V;
	OutputJson[u8"AssetName"V].Value = AssetBaseName;

	FFileStream XxxxStream { XtexFilePath, EFileModes::Output };
	XxxxStream << OutputJson;

	{
		FString AssetFilePathBC = FPath::Combine(AssetsPath, AssetFolder, AssetFileName + u8".dxtc"V);
		IImageRef CompressedImage;
		if (SourceFilePath.EndsWith(u8".dds"V))
			CompressedImage = IImage::Load(SourceFilePath);
		else
			CompressedImage = IImage::CompressDDS(*IImage::Load(SourceFilePath), sRGB ? EFormat::BC1UFsRGB : EFormat::BC1UF);

		FTextureData TextureData { CompressedImage->ImageData };

		FFileStream FileStream(AssetFilePathBC, EFileModes::Output);
		FileStream << TextureData;
	}

	if (false)
	{
		FString AssetFilePathASTC = FPath::Combine(AssetsPath, AssetFolder, AssetFileName + u8".astc"V);
		IImageRef CompressedImage;
		if (SourceFilePath.EndsWith(u8".as"V))
			CompressedImage = IImage::Load(SourceFilePath);
		else
			CompressedImage = IImage::CompressASTC(*IImage::Load(SourceFilePath), EFormat::ASTC4x4);

		FTextureData TextureData { CompressedImage->ImageData };

		FFileStream FileStream(AssetFilePathASTC, EFileModes::Output);
		FileStream << TextureData;
	}

	LogInfo(u8"ImportTexture: {0}", SourceFilePath);
	return EError::OK;
}

EError ImportMaterial(FStringV ParentAssetPath, FStringV XmtlFilePath, TView<TPair<FString, FString>> MaterialParameters, TView<TPair<FString, FString>> Definetions)
{
	FJsonObject OutputJson;
	OutputJson[u8"Version"V].Value = u8"1.0"V;
	OutputJson[u8"Parent"V].Value = ParentAssetPath;

	OutputJson[u8"Parameters"V].Value = new FJsonObject();

	FJsonObject & Parameters = OutputJson[u8"Parameters"V].Value.Get<FJsonObject &>();
	for (const auto & Pair : MaterialParameters)
		Parameters[Pair.First].Value = Pair.Second;

	if (Definetions.Size)
	{
		OutputJson[u8"Definetions"V].Value = new FJsonObject();
		FJsonObject & DefinetionsObject = OutputJson[u8"Definetions"V].Value.Get<FJsonObject &>();
		for (const auto & Pair : Definetions)
			DefinetionsObject[Pair.First].Value = Pair.Second;
	}

	FFileStream XxxxStream { XmtlFilePath, EFileModes::Output };
	XxxxStream << OutputJson;
	return EError::OK;
}