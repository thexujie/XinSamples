#include "PCH.h"

namespace Xin
{
	void Log(ELogLevel LogLevel, FStringV String)
	{
		Logger().WriteLine(LogLevel, u8"00.Test.RHI"V, String);
	}
}
