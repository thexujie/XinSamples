#include "pch.h"
#include "00.FbxViewer.Types.h"

FString GetProjectAssetsPath()
{
	return FPath::GetDirectory(FEncoding::ToUtf8(FWStringV(__FILEW__)));
}
