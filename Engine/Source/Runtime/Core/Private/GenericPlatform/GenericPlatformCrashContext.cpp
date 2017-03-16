// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "HAL/PlatformTime.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Internationalization/Culture.h"
#include "Misc/Optional.h"
#include "Internationalization/Internationalization.h"
#include "Misc/Guid.h"
#include "Containers/Ticker.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CoreDelegates.h"
#include "Misc/App.h"
#include "Misc/EngineVersion.h"
#include "Misc/EngineBuildSettings.h"

#ifndef NOINITCRASHREPORTER
#define NOINITCRASHREPORTER 0
#endif

/*-----------------------------------------------------------------------------
	FGenericCrashContext
-----------------------------------------------------------------------------*/

const ANSICHAR* FGenericCrashContext::CrashContextRuntimeXMLNameA = "CrashContext.runtime-xml";
const TCHAR* FGenericCrashContext::CrashContextRuntimeXMLNameW = TEXT( "CrashContext.runtime-xml" );

const ANSICHAR* FGenericCrashContext::CrashConfigFileNameA = "CrashReportClient.ini";
const TCHAR* FGenericCrashContext::CrashConfigFileNameW = TEXT("CrashReportClient.ini");
const YString FGenericCrashContext::CrashConfigExtension = TEXT(".ini");
const YString FGenericCrashContext::ConfigSectionName = TEXT("CrashReportClient");

const YString FGenericCrashContext::CrashContextExtension = TEXT(".runtime-xml");
const YString FGenericCrashContext::RuntimePropertiesTag = TEXT( "RuntimeProperties" );
const YString FGenericCrashContext::PlatformPropertiesTag = TEXT( "PlatformProperties" );
const YString FGenericCrashContext::UE4MinidumpName = TEXT( "UE4Minidump.dmp" );
const YString FGenericCrashContext::NewLineTag = TEXT( "&nl;" );

const YString FGenericCrashContext::CrashTypeCrash = TEXT("Crash");
const YString FGenericCrashContext::CrashTypeAssert = TEXT("Assert");
const YString FGenericCrashContext::CrashTypeEnsure = TEXT("Ensure");

const YString FGenericCrashContext::EngineModeExUnknown = TEXT("Unset");
const YString FGenericCrashContext::EngineModeExDirty = TEXT("Dirty");
const YString FGenericCrashContext::EngineModeExVanilla = TEXT("Vanilla");

bool FGenericCrashContext::bIsInitialized = false;
YPlatformMemoryStats FGenericCrashContext::CrashMemoryStats = YPlatformMemoryStats();
int32 FGenericCrashContext::StaticCrashContextIndex = 0;

namespace NCachedCrashContextProperties
{
	static bool bIsInternalBuild;
	static bool bIsPerforceBuild;
	static bool bIsSourceDistribution;
	static bool bIsUE4Release;
	static TOptional<bool> bIsVanilla;
	static YString GameName;
	static YString ExecutableName;
	static YString PlatformName;
	static YString PlatformNameIni;
	static YString DeploymentName;
	static YString BaseDir;
	static YString RootDir;
	static YString EpicAccountId;
	static YString LoginIdStr;
	static YString OsVersion;
	static YString OsSubVersion;
	static int32 NumberOfCores;
	static int32 NumberOfCoresIncludingHyperthreads;
	static YString CPUVendor;
	static YString CPUBrand;
	static YString PrimaryGPUBrand;
	static YString UserName;
	static YString DefaultLocale;
	static int32 CrashDumpMode;
	static int32 SecondsSinceStart;
	static YString CrashGUIDRoot;
	static YString UserActivityHint;
	static YString GameSessionID;
	static YString CommandLine;
	static int32 LanguageLCID;
	static YString CrashReportClientRichText;
}

void FGenericCrashContext::Initialize()
{
#if !NOINITCRASHREPORTER
	NCachedCrashContextProperties::bIsInternalBuild = FEngineBuildSettings::IsInternalBuild();
	NCachedCrashContextProperties::bIsPerforceBuild = FEngineBuildSettings::IsPerforceBuild();
	NCachedCrashContextProperties::bIsSourceDistribution = FEngineBuildSettings::IsSourceDistribution();
	NCachedCrashContextProperties::bIsUE4Release = FApp::IsEngineInstalled();

	NCachedCrashContextProperties::GameName = YString::Printf( TEXT("UE4-%s"), FApp::GetGameName() );
	NCachedCrashContextProperties::ExecutableName = FPlatformProcess::ExecutableName();
	NCachedCrashContextProperties::PlatformName = FPlatformProperties::PlatformName();
	NCachedCrashContextProperties::PlatformNameIni = FPlatformProperties::IniPlatformName();
	NCachedCrashContextProperties::DeploymentName = FApp::GetDeploymentName();
	NCachedCrashContextProperties::BaseDir = FPlatformProcess::BaseDir();
	NCachedCrashContextProperties::RootDir = YPlatformMisc::RootDir();
	NCachedCrashContextProperties::EpicAccountId = YPlatformMisc::GetEpicAccountId();
	NCachedCrashContextProperties::LoginIdStr = YPlatformMisc::GetLoginId();
	YPlatformMisc::GetOSVersions(NCachedCrashContextProperties::OsVersion, NCachedCrashContextProperties::OsSubVersion);
	NCachedCrashContextProperties::NumberOfCores = YPlatformMisc::NumberOfCores();
	NCachedCrashContextProperties::NumberOfCoresIncludingHyperthreads = YPlatformMisc::NumberOfCoresIncludingHyperthreads();

	NCachedCrashContextProperties::CPUVendor = YPlatformMisc::GetCPUVendor();
	NCachedCrashContextProperties::CPUBrand = YPlatformMisc::GetCPUBrand();
	NCachedCrashContextProperties::PrimaryGPUBrand = YPlatformMisc::GetPrimaryGPUBrand();
	NCachedCrashContextProperties::UserName = FPlatformProcess::UserName();
	NCachedCrashContextProperties::DefaultLocale = YPlatformMisc::GetDefaultLocale();
	NCachedCrashContextProperties::CommandLine = FCommandLine::IsInitialized() ? FCommandLine::GetOriginalForLogging() : TEXT(""); 

	if (FInternationalization::IsAvailable())
	{
		NCachedCrashContextProperties::LanguageLCID = FInternationalization::Get().GetCurrentCulture()->GetLCID();
	}
	else
	{
		FCulturePtr DefaultCulture = FInternationalization::Get().GetCulture(TEXT("en"));
		if (DefaultCulture.IsValid())
		{
			NCachedCrashContextProperties::LanguageLCID = DefaultCulture->GetLCID();
		}
		else
		{
			const int DefaultCultureLCID = 1033;
			NCachedCrashContextProperties::LanguageLCID = DefaultCultureLCID;
		}
	}

	// Using the -fullcrashdump parameter will cause full memory minidumps to be created for crashes
	NCachedCrashContextProperties::CrashDumpMode = (int32)ECrashDumpMode::Default;
	if (YPlatformMisc::SupportsFullCrashDumps() && FCommandLine::IsInitialized())
	{
		const TCHAR* CmdLine = FCommandLine::Get();
		if (FParse::Param( CmdLine, TEXT("fullcrashdumpalways") ))
		{
			NCachedCrashContextProperties::CrashDumpMode = (int32)ECrashDumpMode::FullDumpAlways;
		}
		else if (FParse::Param( CmdLine, TEXT("fullcrashdump") ))
		{
			NCachedCrashContextProperties::CrashDumpMode = (int32)ECrashDumpMode::FullDump;
		}
	}

	const YGuid Guid = YGuid::NewGuid();
	NCachedCrashContextProperties::CrashGUIDRoot = YString::Printf(TEXT("UE4CC-%s-%s"), *NCachedCrashContextProperties::PlatformNameIni, *Guid.ToString(EGuidFormats::Digits));

	// Initialize delegate for updating SecondsSinceStart, because FPlatformTime::Seconds() is not POSIX safe.
	const float PollingInterval = 1.0f;
	FTicker::GetCoreTicker().AddTicker( FTickerDelegate::CreateLambda( []( float DeltaTime )
	{
		NCachedCrashContextProperties::SecondsSinceStart = int32(FPlatformTime::Seconds() - GStartTime);
		return true;
	} ), PollingInterval );

	FCoreDelegates::UserActivityStringChanged.AddLambda([](const YString& InUserActivity)
	{
		NCachedCrashContextProperties::UserActivityHint = InUserActivity;
	});

	FCoreDelegates::GameSessionIDChanged.AddLambda([](const YString& InGameSessionID)
	{
		NCachedCrashContextProperties::GameSessionID = InGameSessionID;
	});

	FCoreDelegates::CrashOverrideParamsChanged.AddLambda([](const FCrashOverrideParameters& InParams)
	{
		NCachedCrashContextProperties::CrashReportClientRichText = InParams.CrashReportClientMessageText;
	});

	FCoreDelegates::IsVanillaProductChanged.AddLambda([](bool bIsVanilla)
	{
		NCachedCrashContextProperties::bIsVanilla = bIsVanilla;
	});

	FCoreDelegates::ConfigReadyForUse.AddStatic(FGenericCrashContext::InitializeFromConfig);

	bIsInitialized = true;
#endif
}

void FGenericCrashContext::InitializeFromConfig()
{
#if !NO_LOGGING
	const bool bForceGetSection = false;
	const bool bConstSection = true;
	FConfigSection* CRCConfigSection = GConfig->GetSectionPrivate(*ConfigSectionName, bForceGetSection, bConstSection, GEngineIni);

	if (CRCConfigSection != nullptr)
	{
		// Create a config file and save to a temp location. This file will be copied to
		// the crash folder for all crash reports create by this session.
		FConfigFile CrashConfigFile;

		FConfigSection CRCConfigSectionCopy(*CRCConfigSection);
		CrashConfigFile.Add(ConfigSectionName, CRCConfigSectionCopy);

		CrashConfigFile.Dirty = true;
		CrashConfigFile.Write(GetCrashConfigFilePath());
	}
#endif
}

FGenericCrashContext::FGenericCrashContext()
	: bIsEnsure(false)
{
	CommonBuffer.Reserve( 32768 );
	CrashContextIndex = StaticCrashContextIndex++;
}

void FGenericCrashContext::SerializeContentToBuffer()
{
	TCHAR CrashGUID[CrashGUIDLength];
	GetUniqueCrashName(CrashGUID, CrashGUIDLength);

	// Must conform against:
	// https://www.securecoding.cert.org/confluence/display/seccode/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers
	AddHeader();

	BeginSection( *RuntimePropertiesTag );
	AddCrashProperty( TEXT( "CrashVersion" ), (int32)ECrashDescVersions::VER_3_CrashContext );
	AddCrashProperty( TEXT( "CrashGUID" ), (const TCHAR*)CrashGUID);
	AddCrashProperty( TEXT( "ProcessId" ), FPlatformProcess::GetCurrentProcessId() );
	AddCrashProperty( TEXT( "IsInternalBuild" ), NCachedCrashContextProperties::bIsInternalBuild );
	AddCrashProperty( TEXT( "IsPerforceBuild" ), NCachedCrashContextProperties::bIsPerforceBuild );
	AddCrashProperty( TEXT( "IsSourceDistribution" ), NCachedCrashContextProperties::bIsSourceDistribution );
	AddCrashProperty( TEXT( "IsEnsure" ), bIsEnsure );
	AddCrashProperty( TEXT( "IsAssert" ), YDebug::bHasAsserted );
	AddCrashProperty( TEXT( "CrashType" ), GetCrashTypeString(bIsEnsure, YDebug::bHasAsserted) );

	AddCrashProperty( TEXT( "SecondsSinceStart" ), NCachedCrashContextProperties::SecondsSinceStart );

	// Add common crash properties.
	AddCrashProperty( TEXT( "GameName" ), *NCachedCrashContextProperties::GameName );
	AddCrashProperty( TEXT( "ExecutableName" ), *NCachedCrashContextProperties::ExecutableName );
	AddCrashProperty( TEXT( "BuildConfiguration" ), EBuildConfigurations::ToString( FApp::GetBuildConfiguration() ) );
	AddCrashProperty( TEXT( "GameSessionID" ), *NCachedCrashContextProperties::GameSessionID );

	AddCrashProperty( TEXT( "PlatformName" ), *NCachedCrashContextProperties::PlatformName );
	AddCrashProperty( TEXT( "PlatformNameIni" ), *NCachedCrashContextProperties::PlatformNameIni );
	AddCrashProperty( TEXT( "EngineMode" ), YPlatformMisc::GetEngineMode() );
	AddCrashProperty( TEXT( "EngineModeEx" ), EngineModeExString());

	AddCrashProperty( TEXT( "DeploymentName"), *NCachedCrashContextProperties::DeploymentName );

	AddCrashProperty( TEXT( "EngineVersion" ), *FEngineVersion::Current().ToString() );
	AddCrashProperty( TEXT( "CommandLine" ), *NCachedCrashContextProperties::CommandLine );
	AddCrashProperty( TEXT( "LanguageLCID" ), NCachedCrashContextProperties::LanguageLCID );
	AddCrashProperty( TEXT( "AppDefaultLocale" ), *NCachedCrashContextProperties::DefaultLocale );
	AddCrashProperty( TEXT( "BuildVersion" ), FApp::GetBuildVersion() );
	AddCrashProperty( TEXT( "IsUE4Release" ), NCachedCrashContextProperties::bIsUE4Release );

	// Remove periods from user names to match AutoReporter user names
	// The name prefix is read by CrashRepository.AddNewCrash in the website code
	const bool bSendUserName = NCachedCrashContextProperties::bIsInternalBuild;
	AddCrashProperty( TEXT( "UserName" ), bSendUserName ? *NCachedCrashContextProperties::UserName.Replace( TEXT( "." ), TEXT( "" ) ) : TEXT( "" ) );

	AddCrashProperty( TEXT( "BaseDir" ), *NCachedCrashContextProperties::BaseDir );
	AddCrashProperty( TEXT( "RootDir" ), *NCachedCrashContextProperties::RootDir );
	AddCrashProperty( TEXT( "MachineId" ), *NCachedCrashContextProperties::LoginIdStr.ToUpper() );
	AddCrashProperty( TEXT( "LoginId" ), *NCachedCrashContextProperties::LoginIdStr );
	AddCrashProperty( TEXT( "EpicAccountId" ), *NCachedCrashContextProperties::EpicAccountId );

	AddCrashProperty( TEXT( "CallStack" ), TEXT( "" ) );
	AddCrashProperty( TEXT( "SourceContext" ), TEXT( "" ) );
	AddCrashProperty( TEXT( "UserDescription" ), TEXT( "" ) );
	AddCrashProperty( TEXT( "UserActivityHint" ), *NCachedCrashContextProperties::UserActivityHint );
	AddCrashProperty( TEXT( "ErrorMessage" ), (const TCHAR*)GErrorMessage ); // GErrorMessage may be broken.
	AddCrashProperty( TEXT( "CrashDumpMode" ), NCachedCrashContextProperties::CrashDumpMode );
	AddCrashProperty( TEXT( "CrashReporterMessage" ), *NCachedCrashContextProperties::CrashReportClientRichText );

	// Add misc stats.
	AddCrashProperty( TEXT( "Misc.NumberOfCores" ), NCachedCrashContextProperties::NumberOfCores );
	AddCrashProperty( TEXT( "Misc.NumberOfCoresIncludingHyperthreads" ), NCachedCrashContextProperties::NumberOfCoresIncludingHyperthreads );
	AddCrashProperty( TEXT( "Misc.Is64bitOperatingSystem" ), (int32)YPlatformMisc::Is64bitOperatingSystem() );

	AddCrashProperty( TEXT( "Misc.CPUVendor" ), *NCachedCrashContextProperties::CPUVendor );
	AddCrashProperty( TEXT( "Misc.CPUBrand" ), *NCachedCrashContextProperties::CPUBrand );
	AddCrashProperty( TEXT( "Misc.PrimaryGPUBrand" ), *NCachedCrashContextProperties::PrimaryGPUBrand );
	AddCrashProperty( TEXT( "Misc.OSVersionMajor" ), *NCachedCrashContextProperties::OsVersion );
	AddCrashProperty( TEXT( "Misc.OSVersionMinor" ), *NCachedCrashContextProperties::OsSubVersion );


	// #CrashReport: 2015-07-21 Move to the crash report client.
	/*{
		uint64 AppDiskTotalNumberOfBytes = 0;
		uint64 AppDiskNumberOfFreeBytes = 0;
		YPlatformMisc::GetDiskTotalAndFreeSpace( FPlatformProcess::BaseDir(), AppDiskTotalNumberOfBytes, AppDiskNumberOfFreeBytes );
		AddCrashProperty( TEXT( "Misc.AppDiskTotalNumberOfBytes" ), AppDiskTotalNumberOfBytes );
		AddCrashProperty( TEXT( "Misc.AppDiskNumberOfFreeBytes" ), AppDiskNumberOfFreeBytes );
	}*/

	// YPlatformMemory::GetConstants is called in the GCreateMalloc, so we can assume it is always valid.
	{
		// Add memory stats.
		const YPlatformMemoryConstants& MemConstants = YPlatformMemory::GetConstants();

		AddCrashProperty( TEXT( "MemoryStats.TotalPhysical" ), (uint64)MemConstants.TotalPhysical );
		AddCrashProperty( TEXT( "MemoryStats.TotalVirtual" ), (uint64)MemConstants.TotalVirtual );
		AddCrashProperty( TEXT( "MemoryStats.PageSize" ), (uint64)MemConstants.PageSize );
		AddCrashProperty( TEXT( "MemoryStats.TotalPhysicalGB" ), MemConstants.TotalPhysicalGB );
	}

	AddCrashProperty( TEXT( "MemoryStats.AvailablePhysical" ), (uint64)CrashMemoryStats.AvailablePhysical );
	AddCrashProperty( TEXT( "MemoryStats.AvailableVirtual" ), (uint64)CrashMemoryStats.AvailableVirtual );
	AddCrashProperty( TEXT( "MemoryStats.UsedPhysical" ), (uint64)CrashMemoryStats.UsedPhysical );
	AddCrashProperty( TEXT( "MemoryStats.PeakUsedPhysical" ), (uint64)CrashMemoryStats.PeakUsedPhysical );
	AddCrashProperty( TEXT( "MemoryStats.UsedVirtual" ), (uint64)CrashMemoryStats.UsedVirtual );
	AddCrashProperty( TEXT( "MemoryStats.PeakUsedVirtual" ), (uint64)CrashMemoryStats.PeakUsedVirtual );
	AddCrashProperty( TEXT( "MemoryStats.bIsOOM" ), (int32)YPlatformMemory::bIsOOM );
	AddCrashProperty( TEXT( "MemoryStats.OOMAllocationSize"), (uint64)YPlatformMemory::OOMAllocationSize );
	AddCrashProperty( TEXT( "MemoryStats.OOMAllocationAlignment"), (int32)YPlatformMemory::OOMAllocationAlignment );

	//Architecture
	//CrashedModuleName
	//LoadedModules
	EndSection( *RuntimePropertiesTag );
	
	// Add platform specific properties.
	BeginSection( *PlatformPropertiesTag );
	AddPlatformSpecificProperties();
	EndSection( *PlatformPropertiesTag );

	AddFooter();
}

void FGenericCrashContext::GetUniqueCrashName(TCHAR* GUIDBuffer, int32 BufferSize) const
{
	FCString::Snprintf(GUIDBuffer, BufferSize, TEXT("%s_%04i"), *NCachedCrashContextProperties::CrashGUIDRoot, CrashContextIndex);
}

const bool FGenericCrashContext::IsFullCrashDump() const
{
	return (NCachedCrashContextProperties::CrashDumpMode == (int32)ECrashDumpMode::FullDump) ||
		(NCachedCrashContextProperties::CrashDumpMode == (int32)ECrashDumpMode::FullDumpAlways);
}

const bool FGenericCrashContext::IsFullCrashDumpOnEnsure() const
{
	return (NCachedCrashContextProperties::CrashDumpMode == (int32)ECrashDumpMode::FullDumpAlways);
}

void FGenericCrashContext::SerializeAsXML( const TCHAR* Filename )
{
	SerializeContentToBuffer();
	// Use OS build-in functionality instead.
	FFileHelper::SaveStringToFile( CommonBuffer, Filename, FFileHelper::EEncodingOptions::AutoDetect );
}

void FGenericCrashContext::AddCrashProperty( const TCHAR* PropertyName, const TCHAR* PropertyValue )
{
	CommonBuffer += TEXT( "<" );
	CommonBuffer += PropertyName;
	CommonBuffer += TEXT( ">" );


	CommonBuffer += EscapeXMLString( PropertyValue );

	CommonBuffer += TEXT( "</" );
	CommonBuffer += PropertyName;
	CommonBuffer += TEXT( ">" );
	CommonBuffer += LINE_TERMINATOR;
}

void FGenericCrashContext::AddPlatformSpecificProperties()
{
	// Nothing really to do here. Can be overridden by the platform code.
	// @see FWindowsPlatformCrashContext::AddPlatformSpecificProperties
}

void FGenericCrashContext::AddHeader()
{
	CommonBuffer += TEXT( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" ) LINE_TERMINATOR;
	BeginSection( TEXT("FGenericCrashContext") );
}

void FGenericCrashContext::AddFooter()
{
	EndSection( TEXT( "FGenericCrashContext" ) );
}

void FGenericCrashContext::BeginSection( const TCHAR* SectionName )
{
	CommonBuffer += TEXT( "<" );
	CommonBuffer += SectionName;
	CommonBuffer += TEXT( ">" );
	CommonBuffer += LINE_TERMINATOR;
}

void FGenericCrashContext::EndSection( const TCHAR* SectionName )
{
	CommonBuffer += TEXT( "</" );
	CommonBuffer += SectionName;
	CommonBuffer += TEXT( ">" );
	CommonBuffer += LINE_TERMINATOR;
}

YString FGenericCrashContext::EscapeXMLString( const YString& Text )
{
	return Text
		.Replace( TEXT( "&" ), TEXT( "&amp;" ) )
		.Replace( TEXT( "\"" ), TEXT( "&quot;" ) )
		.Replace( TEXT( "'" ), TEXT( "&apos;" ) )
		.Replace( TEXT( "<" ), TEXT( "&lt;" ) )
		.Replace( TEXT( ">" ), TEXT( "&gt;" ) )
		// Replace newline for FXMLFile.
		.Replace( TEXT( "\n" ), *NewLineTag )
		// Ignore return carriage.
		.Replace( TEXT( "\r" ), TEXT("") );
}

YString FGenericCrashContext::UnescapeXMLString( const YString& Text )
{
	return Text
		.Replace( TEXT( "&amp;" ), TEXT( "&" ) )
		.Replace( TEXT( "&quot;" ), TEXT( "\"" ) )
		.Replace( TEXT( "&apos;" ), TEXT( "'" ) )
		.Replace( TEXT( "&lt;" ), TEXT( "<" ) )
		.Replace( TEXT( "&gt;" ), TEXT( ">" ) )
		.Replace( *NewLineTag, TEXT( "\n" ) );
}

const TCHAR* FGenericCrashContext::GetCrashTypeString(bool InIsEnsure, bool InIsAssert)
{
	if (InIsEnsure)
	{
		return *CrashTypeEnsure;
	}
	else if (InIsAssert)
	{
		return *CrashTypeAssert;
	}

	return *CrashTypeCrash;
}

const TCHAR* FGenericCrashContext::EngineModeExString()
{
	return !NCachedCrashContextProperties::bIsVanilla.IsSet() ? *FGenericCrashContext::EngineModeExUnknown :
		(NCachedCrashContextProperties::bIsVanilla.GetValue() ? *FGenericCrashContext::EngineModeExVanilla : *FGenericCrashContext::EngineModeExDirty);
}

const TCHAR* FGenericCrashContext::GetCrashConfigFilePath()
{
	static YString CrashConfigFilePath;
	if (CrashConfigFilePath.IsEmpty())
	{
		CrashConfigFilePath = YPaths::Combine(*YPaths::GameLogDir(), *NCachedCrashContextProperties::CrashGUIDRoot, FGenericCrashContext::CrashConfigFileNameW);
	}
	return *CrashConfigFilePath;
}

FProgramCounterSymbolInfoEx::FProgramCounterSymbolInfoEx( YString InModuleName, YString InFunctionName, YString InFilename, uint32 InLineNumber, uint64 InSymbolDisplacement, uint64 InOffsetInModule, uint64 InProgramCounter ) :
	ModuleName( InModuleName ),
	FunctionName( InFunctionName ),
	Filename( InFilename ),
	LineNumber( InLineNumber ),
	SymbolDisplacement( InSymbolDisplacement ),
	OffsetInModule( InOffsetInModule ),
	ProgramCounter( InProgramCounter )
{

}
