#pragma once

const FStringV DefaultMaterialShader = u8"Engine/Shaders/Default.xsf"V;
const FStringV SponzaDefaultMaterialAssetPath = u8"/Sponza/Materials/Sponza.xmtt"V;
const FStringV DiffuseTexture = u8"DiffuseTexture"V;

EError ImportTexture(FStringV SourceFilePath, FStringV AssetsPath, FStringV AssetFolder, FStringV AssetFileName, bool sRGB);
EError ImportMaterial(FStringV ParentAssetPath, FStringV XmtlFilePath, TView<TPair<FString, FString>> MaterialParameters, TView<TPair<FString, FString>> Definetions = {});

class FLocalAsset
{
public:
	FLocalAsset(FStringV FilePath) : FilePath(FilePath) { }

	IDataStream & Read(FJsonObject & Json)
	{
		FFileStream XxxxStream { FilePath, EFileModes::Input };
		XxxxStream >> Json;

		Stream.Close();
		Stream.Open(FilePath + u8".data"V, EFileModes::Input);
		return Stream;
	}

	IDataStream & Write(const FJsonObject & Json)
	{
		FDirectory::EnsureExists(FPath::GetDirectory(FilePath));

		FFileStream XxxxStream { FilePath, EFileModes::Output };
		XxxxStream << Json;

		Stream.Close();
		Stream.Open(FilePath + u8".data"V, EFileModes::Output);
		return Stream;
	}

private:
	FStringV FilePath;
	FFileStream Stream;
};
