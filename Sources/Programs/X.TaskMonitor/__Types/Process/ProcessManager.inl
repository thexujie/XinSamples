// D8A23A22FAAF8481445C08A3F5CF74A4 2024-04-22 15:46:12
// Generated code begin.

#pragma once
#include "PCH.h"
#include "Process/ProcessManager.h"

namespace Xin
{
	class FProcessInfo_Type : public TClass<TaskMonitor::FProcessInfo>
	{
	public:
		using FProcessInfo = TaskMonitor::FProcessInfo;

		// --- 0 Constructors
		static inline const TConstructor<FProcessInfo, TaskMonitor::FProcessInfo const &> Constructor___FProcessInfo_const_lref { u8"!Constructor___FProcessInfo_const_lref"N, FMetadata::None };
		static inline const TConstructor<FProcessInfo, TaskMonitor::FProcessInfo &&> Constructor___FProcessInfo_ref { u8"!Constructor___FProcessInfo_ref"N, FMetadata::None };
		static inline const TReferenceWrapper<const FConstructor> ReflectedConstructors[] =
		{
			Constructor___FProcessInfo_const_lref,
			Constructor___FProcessInfo_ref,
		};

		// --- 1 Methods

		// --- 2 Fields
		struct Field___CreationTime
		{
			static inline const TField Field { u8"CreationTime"N, &FProcessInfo::CreationTime, FMetadata::None };
		};
		struct Field___Icon
		{
			static inline const TField Field { u8"Icon"N, &FProcessInfo::Icon, FMetadata::None };
		};
		struct Field___ProcessName
		{
			static inline const TField Field { u8"ProcessName"N, &FProcessInfo::ProcessName, FMetadata::None };
		};
		struct Field___ProcessId
		{
			static inline const TField Field { u8"ProcessId"N, &FProcessInfo::ProcessId, FMetadata::None };
		};
		struct Field___ThreadCount
		{
			static inline const TField Field { u8"ThreadCount"N, &FProcessInfo::ThreadCount, FMetadata::None };
		};
		struct Field___CurrentDirectory
		{
			static inline const TField Field { u8"CurrentDirectory"N, &FProcessInfo::CurrentDirectory, FMetadata::None };
		};
		struct Field___CommandLine
		{
			static inline const TField Field { u8"CommandLine"N, &FProcessInfo::CommandLine, FMetadata::None };
		};
		struct Field___FilePath
		{
			static inline const TField Field { u8"FilePath"N, &FProcessInfo::FilePath, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FField> ReflectedFields[] =
		{
			Field___CreationTime::Field,
			Field___Icon::Field,
			Field___ProcessName::Field,
			Field___ProcessId::Field,
			Field___ThreadCount::Field,
			Field___CurrentDirectory::Field,
			Field___CommandLine::Field,
			Field___FilePath::Field,
		};

		// --- 3 Properties

		// --- 4 Base Types

		// --- 5 Constructor
		FProcessInfo_Type(FName Name = u8"FProcessInfo"N) : TClass(Name, FClass::None)
		{
			FClass::Constructors = ReflectedConstructors;
			FClass::Fields = ReflectedFields;
		}
	};

	template<> struct XIN_EXPORT TStaticType<TaskMonitor::FProcessInfo>
	{
		static FType & Type()
		{
			static FProcessInfo_Type FProcessInfoType { u8"FProcessInfo"N };
			return FProcessInfoType;
		}
	};

	template<> XIN_EXPORT inline FType & Typeof<TaskMonitor::FProcessInfo>()
	{
		return TStaticType<TaskMonitor::FProcessInfo>().Type();
	}

	static TTypeRegister<TaskMonitor::FProcessInfo> FProcessInfoTypeRegister { TStaticType<TaskMonitor::FProcessInfo>().Type() };
}

// Generated code end.
