// 8E057B7D1000DB8EFDF601F227019A63 2024-04-26 09:50:41
// Generated code begin.

#pragma once
#include "PCH.h"
#include "Process/ProcessItem.h"

namespace Xin
{
	class FProcessItem_Type : public TClass<TaskMonitor::FProcessItem>
	{
	public:
		using FProcessItem = TaskMonitor::FProcessItem;

		// --- 0 Constructors

		// --- 1 Methods
		struct Method___FType_lref___Type____const
		{
			static inline const TMethod<FProcessItem, FType &()> Method { u8"Type"N, &FProcessItem::Type, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FMethod> ReflectedMethods[] =
		{
			Method___FType_lref___Type____const::Method,
		};

		// --- 2 Fields
		struct Field___CreationTime
		{
			static inline const TField Field { u8"CreationTime"N, &FProcessItem::CreationTime, FMetadata::None };
		};
		struct Field___CommandLine
		{
			static inline const TField Field { u8"CommandLine"N, &FProcessItem::CommandLine, FMetadata::None };
		};
		struct Field___FilePath
		{
			static inline const TField Field { u8"FilePath"N, &FProcessItem::FilePath, FMetadata::None };
		};
		struct Field___CycleTime
		{
			static inline const TField Field { u8"CycleTime"N, &FProcessItem::CycleTime, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FField> ReflectedFields[] =
		{
			Field___CreationTime::Field,
			Field___CommandLine::Field,
			Field___FilePath::Field,
			Field___CycleTime::Field,
		};

		// --- 3 Properties
		struct Property___ProcessId
		{
			static inline const TProxyProperty Property { u8"ProcessId"N, &FProcessItem::__ProcessId, &FProcessItem::__GetProcessId, &FProcessItem::__SetProcessId, FMetadata::None };
		};
		struct Property___ProcessName
		{
			static inline const TProxyProperty Property { u8"ProcessName"N, &FProcessItem::__ProcessName, &FProcessItem::__GetProcessName, &FProcessItem::__SetProcessName, FMetadata::None };
		};
		struct Property___ThreadCount
		{
			static inline const TProxyProperty Property { u8"ThreadCount"N, &FProcessItem::__ThreadCount, &FProcessItem::__GetThreadCount, &FProcessItem::__SetThreadCount, FMetadata::None };
		};
		struct Property___WorkingSetSize
		{
			static inline const TProxyProperty Property { u8"WorkingSetSize"N, &FProcessItem::__WorkingSetSize, &FProcessItem::__GetWorkingSetSize, &FProcessItem::__SetWorkingSetSize, FMetadata::None };
		};
		struct Property___PagefileUsage
		{
			static inline const TProxyProperty Property { u8"PagefileUsage"N, &FProcessItem::__PagefileUsage, &FProcessItem::__GetPagefileUsage, &FProcessItem::__SetPagefileUsage, FMetadata::None };
		};
		struct Property___CurrentDirectory
		{
			static inline const TProxyProperty Property { u8"CurrentDirectory"N, &FProcessItem::__CurrentDirectory, &FProcessItem::__GetCurrentDirectory, &FProcessItem::__SetCurrentDirectory, FMetadata::None };
		};
		struct Property___CPUUsage
		{
			static inline const TProxyProperty Property { u8"CPUUsage"N, &FProcessItem::__CPUUsage, &FProcessItem::__GetCPUUsage, &FProcessItem::__SetCPUUsage, FMetadata::None };
		};
		struct Property___Description
		{
			static inline const TProxyProperty Property { u8"Description"N, &FProcessItem::__Description, &FProcessItem::__GetDescription, &FProcessItem::__SetDescription, FMetadata::None };
		};
		struct Property___CompanyName
		{
			static inline const TProxyProperty Property { u8"CompanyName"N, &FProcessItem::__CompanyName, &FProcessItem::__GetCompanyName, &FProcessItem::__SetCompanyName, FMetadata::None };
		};
		struct Property___ProductName
		{
			static inline const TProxyProperty Property { u8"ProductName"N, &FProcessItem::__ProductName, &FProcessItem::__GetProductName, &FProcessItem::__SetProductName, FMetadata::None };
		};
		struct Property___FileVersion
		{
			static inline const TProxyProperty Property { u8"FileVersion"N, &FProcessItem::__FileVersion, &FProcessItem::__GetFileVersion, &FProcessItem::__SetFileVersion, FMetadata::None };
		};
		static inline const TReferenceWrapper<const FProperty> ReflectedProperties[] =
		{
			Property___ProcessId::Property,
			Property___ProcessName::Property,
			Property___ThreadCount::Property,
			Property___WorkingSetSize::Property,
			Property___PagefileUsage::Property,
			Property___CurrentDirectory::Property,
			Property___CPUUsage::Property,
			Property___Description::Property,
			Property___CompanyName::Property,
			Property___ProductName::Property,
			Property___FileVersion::Property,
		};

		// --- 4 Base Types
		static inline const TReferenceWrapper<const FClass> ReflectedBaseTypes[] =
		{
			Classof<IDependency>(),
		};

		// --- 5 Constructor
		FProcessItem_Type(FName Name = u8"FProcessItem"N) : TClass(Name, Classof<IDependency>())
		{
			FClass::Fields = ReflectedFields;
			FClass::Methods = ReflectedMethods;
			FClass::Properties = ReflectedProperties;
			FClass::BaseTypes = ReflectedBaseTypes;
		}
	};

	const TProperty<uint32> & TaskMonitor::FProcessItem::ProcessIdProperty() { return FProcessItem_Type::Property___ProcessId::Property; }

	const TProperty<FString> & TaskMonitor::FProcessItem::ProcessNameProperty() { return FProcessItem_Type::Property___ProcessName::Property; }

	const TProperty<uint32> & TaskMonitor::FProcessItem::ThreadCountProperty() { return FProcessItem_Type::Property___ThreadCount::Property; }

	const TProperty<uint64> & TaskMonitor::FProcessItem::WorkingSetSizeProperty() { return FProcessItem_Type::Property___WorkingSetSize::Property; }

	const TProperty<uint64> & TaskMonitor::FProcessItem::PagefileUsageProperty() { return FProcessItem_Type::Property___PagefileUsage::Property; }

	const TProperty<FString> & TaskMonitor::FProcessItem::CurrentDirectoryProperty() { return FProcessItem_Type::Property___CurrentDirectory::Property; }

	const TProperty<float32> & TaskMonitor::FProcessItem::CPUUsageProperty() { return FProcessItem_Type::Property___CPUUsage::Property; }

	const TProperty<FString> & TaskMonitor::FProcessItem::DescriptionProperty() { return FProcessItem_Type::Property___Description::Property; }

	const TProperty<FString> & TaskMonitor::FProcessItem::CompanyNameProperty() { return FProcessItem_Type::Property___CompanyName::Property; }

	const TProperty<FString> & TaskMonitor::FProcessItem::ProductNameProperty() { return FProcessItem_Type::Property___ProductName::Property; }

	const TProperty<FString> & TaskMonitor::FProcessItem::FileVersionProperty() { return FProcessItem_Type::Property___FileVersion::Property; }

	template<> struct XIN_EXPORT TStaticType<TaskMonitor::FProcessItem>
	{
		static FType & Type()
		{
			static FProcessItem_Type FProcessItemType { u8"FProcessItem"N };
			return FProcessItemType;
		}
	};

	FType & TaskMonitor::FProcessItem::StaticType()
	{
		return TStaticType<TaskMonitor::FProcessItem>().Type();
	}

	static TTypeRegister<TaskMonitor::FProcessItem> FProcessItemTypeRegister { TStaticType<TaskMonitor::FProcessItem>().Type() };
}

// Generated code end.
