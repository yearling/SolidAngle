// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/AssertionMacros.h"
#include "HAL/PlatformFilemanager.h"
#include "Math/UnrealMathUtility.h"
#include "HAL/UnrealMemory.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "Logging/LogMacros.h"
#include "CoreGlobals.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Internationalization/Text.h"
#include "Internationalization/Internationalization.h"
#include "Misc/Guid.h"
#include "Math/Color.h"
#include "GenericPlatform/GenericPlatformCompression.h"
#include "Misc/ConfigCacheIni.h"
#include "GenericPlatform/GenericApplication.h"
#include "Misc/App.h"
#include "GenericPlatform/GenericPlatformChunkInstall.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/VarargsHelper.h"
#include "Misc/SecureHash.h"
#include "HAL/ExceptionHandling.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "GenericPlatform/GenericPlatformDriver.h"

#include "Misc/UProjectInfo.h"

#if UE_ENABLE_ICU
	THIRD_PARTY_INCLUDES_START
		#include <unicode/locid.h>
	THIRD_PARTY_INCLUDES_END
#endif

DEFINE_LOG_CATEGORY_STATIC(LogGenericPlatformMisc, Log, All);

/** Holds an override path if a program has special needs */
YString OverrideGameDir;


/* EBuildConfigurations interface
 *****************************************************************************/

namespace EBuildConfigurations
{
	EBuildConfigurations::Type FromString( const YString& Configuration )
	{
		if (FCString::Strcmp(*Configuration, TEXT("Debug")) == 0)
		{
			return Debug;
		}
		else if (FCString::Strcmp(*Configuration, TEXT("DebugGame")) == 0)
		{
			return DebugGame;
		}
		else if (FCString::Strcmp(*Configuration, TEXT("Development")) == 0)
		{
			return Development;
		}
		else if (FCString::Strcmp(*Configuration, TEXT("Shipping")) == 0)
		{
			return Shipping;
		}
		else if(FCString::Strcmp(*Configuration, TEXT("Test")) == 0)
		{
			return Test;
		}

		return Unknown;
	}

	const TCHAR* ToString( EBuildConfigurations::Type Configuration )
	{
		switch (Configuration)
		{
			case Debug:
				return TEXT("Debug");

			case DebugGame:
				return TEXT("DebugGame");

			case Development:
				return TEXT("Development");

			case Shipping:
				return TEXT("Shipping");

			case Test:
				return TEXT("Test");

			default:
				return TEXT("Unknown");
		}
	}

	FText ToText( EBuildConfigurations::Type Configuration )
	{
		switch (Configuration)
		{
		case Debug:
			return NSLOCTEXT("UnrealBuildConfigurations", "DebugName", "Debug");

		case DebugGame:
			return NSLOCTEXT("UnrealBuildConfigurations", "DebugGameName", "DebugGame");

		case Development:
			return NSLOCTEXT("UnrealBuildConfigurations", "DevelopmentName", "Development");

		case Shipping:
			return NSLOCTEXT("UnrealBuildConfigurations", "ShippingName", "Shipping");

		case Test:
			return NSLOCTEXT("UnrealBuildConfigurations", "TestName", "Test");

		default:
			return NSLOCTEXT("UnrealBuildConfigurations", "UnknownName", "Unknown");
		}
	}
}


/* EBuildConfigurations interface
 *****************************************************************************/

namespace EBuildTargets
{
	EBuildTargets::Type FromString( const YString& Target )
	{
		if (FCString::Strcmp(*Target, TEXT("Editor")) == 0)
		{
			return Editor;
		}
		else if (FCString::Strcmp(*Target, TEXT("Game")) == 0)
		{
			return Game;
		}
		else if (FCString::Strcmp(*Target, TEXT("Server")) == 0)
		{
			return Server;
		}

		return Unknown;
	}

	const TCHAR* ToString( EBuildTargets::Type Target )
	{
		switch (Target)
		{
			case Editor:
				return TEXT("Editor");

			case Game:
				return TEXT("Game");

			case Server:
				return TEXT("Server");

			default:
				return TEXT("Unknown");
		}
	}
}

YString FSHA256Signature::ToString() const
{
	YString LocalHashStr;
	for (int Idx = 0; Idx < 32; Idx++)
	{
		LocalHashStr += YString::Printf(TEXT("%02x"), Signature[Idx]);
	}
	return LocalHashStr;
}

/* YGenericPlatformMisc interface
 *****************************************************************************/

#if !UE_BUILD_SHIPPING
	bool YGenericPlatformMisc::bShouldPromptForRemoteDebugging = false;
	bool YGenericPlatformMisc::bPromptForRemoteDebugOnEnsure = false;
#endif	//#if !UE_BUILD_SHIPPING


GenericApplication* YGenericPlatformMisc::CreateApplication()
{
	return new GenericApplication( nullptr );
}

void YGenericPlatformMisc::SetEnvironmentVar(const TCHAR* VariableName, const TCHAR* Value)
{
	UE_LOG(LogGenericPlatformMisc, Error, TEXT("SetEnvironmentVar not implemented for this platform: %s = %s"), VariableName, Value);
}

const TCHAR* YGenericPlatformMisc::GetPathVarDelimiter()
{
	return TEXT(";");
}

TArray<uint8> YGenericPlatformMisc::GetMacAddress()
{
	return TArray<uint8>();
}

YString YGenericPlatformMisc::GetMacAddressString()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	TArray<uint8> MacAddr = YPlatformMisc::GetMacAddress();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	YString Result;
	for (TArray<uint8>::TConstIterator it(MacAddr);it;++it)
	{
		Result += YString::Printf(TEXT("%02x"),*it);
	}
	return Result;
}

YString YGenericPlatformMisc::GetHashedMacAddressString()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	return FMD5::HashAnsiString(*YPlatformMisc::GetMacAddressString());
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

YString YGenericPlatformMisc::GetUniqueDeviceId()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	return YPlatformMisc::GetHashedMacAddressString();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

YString YGenericPlatformMisc::GetDeviceId()
{
	// @todo: When this function is finally removed, the functionality used will need to be moved in here.
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	return GetUniqueDeviceId();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void YGenericPlatformMisc::SubmitErrorReport( const TCHAR* InErrorHist, EErrorReportMode::Type InMode )
{
	if ((!YPlatformMisc::IsDebuggerPresent() || GAlwaysReportCrash) && !FParse::Param(FCommandLine::Get(), TEXT("CrashForUAT")))
	{
		if ( GUseCrashReportClient )
		{
			int32 FromCommandLine = 0;
			FParse::Value( FCommandLine::Get(), TEXT("AutomatedPerfTesting="), FromCommandLine );
			if (FApp::IsUnattended() && FromCommandLine != 0 && FParse::Param(FCommandLine::Get(), TEXT("KillAllPopUpBlockingWindows")))
			{
				UE_LOG(LogGenericPlatformMisc, Error, TEXT("This platform does not implement KillAllPopUpBlockingWindows"));
			}
		}
		else
		{
			UE_LOG(LogGenericPlatformMisc, Error, TEXT("This platform cannot submit a crash report. Report was:\n%s"), InErrorHist);
		}
	}
}


YString YGenericPlatformMisc::GetCPUVendor()
{
	// Not implemented cross-platform. Each platform may or may not choose to implement this.
	return YString( TEXT( "GenericCPUVendor" ) );
}

YString YGenericPlatformMisc::GetCPUBrand()
{
	// Not implemented cross-platform. Each platform may or may not choose to implement this.
	return YString( TEXT( "GenericCPUBrand" ) );
}

uint32 YGenericPlatformMisc::GetCPUInfo()
{
	// Not implemented cross-platform. Each platform may or may not choose to implement this.
	return 0;
}

YString YGenericPlatformMisc::GetPrimaryGPUBrand()
{
	// Not implemented cross-platform. Each platform may or may not choose to implement this.
	return YString( TEXT( "GenericGPUBrand" ) );
}

FGPUDriverInfo YGenericPlatformMisc::GetGPUDriverInfo(const YString& DeviceDescription)
{
	return FGPUDriverInfo();
}

void YGenericPlatformMisc::GetOSVersions( YString& out_OSVersionLabel, YString& out_OSSubVersionLabel )
{
	// Not implemented cross-platform. Each platform may or may not choose to implement this.
	out_OSVersionLabel = YString( TEXT( "GenericOSVersionLabel" ) );
	out_OSSubVersionLabel = YString( TEXT( "GenericOSSubVersionLabel" ) );
}


bool YGenericPlatformMisc::GetDiskTotalAndFreeSpace( const YString& InPath, uint64& TotalNumberOfBytes, uint64& NumberOfFreeBytes )
{
	// Not implemented cross-platform. Each platform may or may not choose to implement this.
	TotalNumberOfBytes = 0;
	NumberOfFreeBytes = 0;
	return false;
}


void YGenericPlatformMisc::MemoryBarrier()
{
}

void YGenericPlatformMisc::HandleIOFailure( const TCHAR* Filename )
{
	UE_LOG(LogGenericPlatformMisc, Fatal,TEXT("I/O failure operating on '%s'"), Filename ? Filename : TEXT("Unknown file"));
}

void YGenericPlatformMisc::RaiseException(uint32 ExceptionCode)
{
	/** This is the last place to gather memory stats before exception. */
	FGenericCrashContext::CrashMemoryStats = YPlatformMemory::GetStats();

#if HACK_HEADER_GENERATOR && !PLATFORM_EXCEPTIONS_DISABLED
	// We want Unreal Header Tool to throw an exception but in normal runtime code 
	// we don't support exception handling
	throw(ExceptionCode);
#else	
	*((uint32*)3) = ExceptionCode;
#endif
}

bool YGenericPlatformMisc::SetStoredValue(const YString& InStoreId, const YString& InSectionName, const YString& InKeFName, const YString& InValue)
{
	check(!InStoreId.IsEmpty());
	check(!InSectionName.IsEmpty());
	check(!InKeFName.IsEmpty());

	// This assumes that FPlatformProcess::ApplicationSettingsDir() returns a user-specific directory; it doesn't on Windows, but Windows overrides this behavior to use the registry
	const YString ConfigPath = YString(FPlatformProcess::ApplicationSettingsDir()) / InStoreId / YString(TEXT("KeyValueStore.ini"));
		
	FConfigFile ConfigFile;
	ConfigFile.Read(ConfigPath);

	FConfigSection& Section = ConfigFile.FindOrAdd(InSectionName);

	FConfigValue& KeyValue = Section.FindOrAdd(*InKeFName);
	KeyValue = FConfigValue(InValue);

	ConfigFile.Dirty = true;
	return ConfigFile.Write(ConfigPath);
}

bool YGenericPlatformMisc::GetStoredValue(const YString& InStoreId, const YString& InSectionName, const YString& InKeFName, YString& OutValue)
{
	check(!InStoreId.IsEmpty());
	check(!InSectionName.IsEmpty());
	check(!InKeFName.IsEmpty());

	// This assumes that FPlatformProcess::ApplicationSettingsDir() returns a user-specific directory; it doesn't on Windows, but Windows overrides this behavior to use the registry
	const YString ConfigPath = YString(FPlatformProcess::ApplicationSettingsDir()) / InStoreId / YString(TEXT("KeyValueStore.ini"));
		
	FConfigFile ConfigFile;
	ConfigFile.Read(ConfigPath);

	const FConfigSection* const Section = ConfigFile.Find(InSectionName);
	if(Section)
	{
		const FConfigValue* const KeyValue = Section->Find(*InKeFName);
		if(KeyValue)
		{
			OutValue = KeyValue->GetValue();
			return true;
		}
	}

	return false;
}

bool YGenericPlatformMisc::DeleteStoredValue(const YString& InStoreId, const YString& InSectionName, const YString& InKeFName)
{	
	check(!InStoreId.IsEmpty());
	check(!InSectionName.IsEmpty());
	check(!InKeFName.IsEmpty());

	// This assumes that FPlatformProcess::ApplicationSettingsDir() returns a user-specific directory; it doesn't on Windows, but Windows overrides this behavior to use the registry
	const YString ConfigPath = YString(FPlatformProcess::ApplicationSettingsDir()) / InStoreId / YString(TEXT("KeyValueStore.ini"));

	FConfigFile ConfigFile;
	ConfigFile.Read(ConfigPath);

	FConfigSection* const Section = ConfigFile.Find(InSectionName);
	if (Section)
	{
		int32 RemovedNum = Section->Remove(*InKeFName);

		ConfigFile.Dirty = true;
		return ConfigFile.Write(ConfigPath) && RemovedNum == 1;
	}

	return false;
}

void YGenericPlatformMisc::LowLevelOutputDebugString( const TCHAR *Message )
{
	YPlatformMisc::LocalPrint( Message );
}

void YGenericPlatformMisc::LowLevelOutputDebugStringf(const TCHAR *Fmt, ... )
{
	GROWABLE_LOGF(
		YPlatformMisc::LowLevelOutputDebugString( Buffer );
	);
}

void YGenericPlatformMisc::SetUTF8Output()
{
	// assume that UTF-8 is possible by default anyway
}

void YGenericPlatformMisc::LocalPrint( const TCHAR* Str )
{
#if PLATFORM_USE_LS_SPEC_FOR_WIDECHAR
	printf("%ls", Str);
#else
	printf("%s", Str);
#endif
}

bool YGenericPlatformMisc::HasSeparateChannelForDebugOutput()
{
	return true;
}

void YGenericPlatformMisc::RequestMinimize()
{
}

void YGenericPlatformMisc::RequestExit( bool Force )
{
	UE_LOG(LogGenericPlatformMisc, Log,  TEXT("YPlatformMisc::RequestExit(%i)"), Force );
	if( Force )
	{
		// Force immediate exit.
		// Dangerous because config code isn't flushed, global destructors aren't called, etc.
		// Suppress abort message and MS reports.
		abort();
	}
	else
	{
		// Tell the platform specific code we want to exit cleanly from the main loop.
		GIsRequestingExit = 1;
	}
}

void YGenericPlatformMisc::RequestExitWithStatus(bool Force, uint8 ReturnCode)
{
	// Generic implementation will ignore the return code - this may be important, so warn.
	UE_LOG(LogGenericPlatformMisc, Warning, TEXT("YPlatformMisc::RequestExitWithStatus(%i, %d) - return code will be ignored by the generic implementation."), Force, ReturnCode);

	return YPlatformMisc::RequestExit(Force);
}

const TCHAR* YGenericPlatformMisc::GetSystemErrorMessage(TCHAR* OutBuffer, int32 BufferCount, int32 Error)
{
	const TCHAR* Message = TEXT("No system errors available on this platform.");
	check(OutBuffer && BufferCount > 80);
	Error = 0;
	FCString::Strcpy(OutBuffer, BufferCount - 1, Message);
	return OutBuffer;
}

void YGenericPlatformMisc::ClipboardCopy(const TCHAR* Str)
{

}
void YGenericPlatformMisc:: ClipboardPaste(class YString& Dest)
{
	Dest = YString();
}

void YGenericPlatformMisc::CreateGuid(FGuid& Guid)
{
	static uint16 IncrementCounter = 0; 

	int32 Year = 0, Month = 0, DayOfWeek = 0, Day = 0, Hour = 0, Min = 0, Sec = 0, MSec = 0; // Use real time for baseline uniqueness
	uint32 SequentialBits = static_cast<uint32>(IncrementCounter++); // Add sequential bits to ensure sequentially generated guids are unique even if Cycles is wrong
	uint32 RandBits = YMath::Rand() & 0xFFFF; // Add randomness to improve uniqueness across machines

	FPlatformTime::SystemTime(Year, Month, DayOfWeek, Day, Hour, Min, Sec, MSec);

	Guid = FGuid(RandBits | (SequentialBits << 16), Day | (Hour << 8) | (Month << 16) | (Sec << 24), MSec | (Min << 16), Year ^ FPlatformTime::Cycles());
}

EAppReturnType::Type YGenericPlatformMisc::MessageBoxExt( EAppMsgType::Type MsgType, const TCHAR* Text, const TCHAR* Caption )
{
	if (GWarn)
	{
		UE_LOG(LogGenericPlatformMisc, Warning, TEXT("Cannot display dialog box on this platform: %s : %s"), Caption, Text);
	}

	switch(MsgType)
	{
	case EAppMsgType::Ok:
		return EAppReturnType::Ok; // Ok
	case EAppMsgType::YesNo:
		return EAppReturnType::No; // No
	case EAppMsgType::OkCancel:
		return EAppReturnType::Cancel; // Cancel
	case EAppMsgType::YesNoCancel:
		return EAppReturnType::Cancel; // Cancel
	case EAppMsgType::CancelRetryContinue:
		return EAppReturnType::Cancel; // Cancel
	case EAppMsgType::YesNoYesAllNoAll:
		return EAppReturnType::No; // No
	case EAppMsgType::YesNoYesAllNoAllCancel:
		return EAppReturnType::Yes; // Yes
	default:
		check(0);
	}
	return EAppReturnType::Cancel; // Cancel
}

const TCHAR* YGenericPlatformMisc::RootDir()
{
	static YString Path;
	if (Path.Len() == 0)
	{
		YString TempPath = YPaths::EngineDir();
		int32 chopPos = TempPath.Find(TEXT("/Engine"));
		if (chopPos != INDEX_NONE)
		{
			TempPath = TempPath.Left(chopPos + 1);
			TempPath = YPaths::ConvertRelativePathToFull(TempPath);
			Path = TempPath;
		}
		else
		{
			Path = FPlatformProcess::BaseDir();

			// if the path ends in a separator, remove it
			if( Path.Right(1)==TEXT("/") )
			{
				Path = Path.LeftChop( 1 );
			}

			// keep going until we've removed Binaries
#if IS_MONOLITHIC && !IS_PROGRAM
			int32 pos = Path.Find(*YString::Printf(TEXT("/%s/Binaries"), FApp::GetGameName()));
#else
			int32 pos = Path.Find(TEXT("/Engine/Binaries"), ESearchCase::IgnoreCase);
#endif
			if ( pos != INDEX_NONE )
			{
				Path = Path.Left(pos + 1);
			}
			else
			{
				pos = Path.Find(TEXT("/../Binaries"), ESearchCase::IgnoreCase);
				if ( pos != INDEX_NONE )
				{
					Path = Path.Left(pos + 1) + TEXT("../../");
				}
				else
				{
					while( Path.Len() && Path.Right(1)!=TEXT("/") )
					{
						Path = Path.LeftChop( 1 );
					}
				}

			}
		}
	}
	return *Path;
}

const TCHAR* YGenericPlatformMisc::EngineDir()
{
	static YString EngineDirectory = TEXT("");
	if (EngineDirectory.Len() == 0)
	{
		// See if we are a root-level project
		YString DefaultEngineDir = TEXT("../../../Engine/");
#if PLATFORM_DESKTOP
		FPlatformProcess::SetCurrentWorkingDirectoryToBaseDir();

		//@todo. Need to have a define specific for this scenario??
		if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*(DefaultEngineDir / TEXT("Binaries"))))
		{
			EngineDirectory = DefaultEngineDir;
		}
		else if (GForeignEngineDir != NULL && FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*(YString(GForeignEngineDir) / TEXT("Binaries"))))
		{
			EngineDirectory = GForeignEngineDir;
		}

		if (EngineDirectory.Len() == 0)
		{
			// Temporary work-around for legacy dependency on ../../../ (re Lightmass)
			EngineDirectory = DefaultEngineDir;
			UE_LOG(LogGenericPlatformMisc, Warning, TEXT("Failed to determine engine directory: Defaulting to %s"), *EngineDirectory);
		}
#else
		EngineDirectory = DefaultEngineDir;
#endif
	}
	return *EngineDirectory;
}

// wrap the LaunchDir variable in a function to work around static/global initialization order
static YString& GetWrappedLaunchDir()
{
	static YString LaunchDir;
	return LaunchDir;
}

void YGenericPlatformMisc::CacheLaunchDir()
{
	// we can only cache this ONCE
	static bool bOneTime = false;
	if (bOneTime)
	{
		return;
	}
	bOneTime = true;
	
	GetWrappedLaunchDir() = FPlatformProcess::GetCurrentWorkingDirectory() + TEXT("/");
}

const TCHAR* YGenericPlatformMisc::LaunchDir()
{
	return *GetWrappedLaunchDir();
}


const TCHAR* YGenericPlatformMisc::GetNullRHIShaderFormat()
{
	return TEXT("PCD3D_SM5");
}

IPlatformChunkInstall* YGenericPlatformMisc::GetPlatformChunkInstall()
{
	static FGenericPlatformChunkInstall Singleton;
	return &Singleton;
}

IPlatformCompression* YGenericPlatformMisc::GetPlatformCompression()
{
	static FGenericPlatformCompression Singleton;
	return &Singleton;
}

FLinearColor YGenericPlatformMisc::GetScreenPixelColor(const struct YVector2D& InScreenPos, float InGamma)
{ 
	return FLinearColor::Black;
}

void GenericPlatformMisc_GetProjectFilePathGameDir(YString& OutGameDir)
{
	// Here we derive the game path from the project file location.
	YString BasePath = YPaths::GetPath(YPaths::GetProjectFilePath());
	YPaths::NormalizeFilename(BasePath);
	BasePath = FFileManagerGeneric::DefaultConvertToRelativePath(*BasePath);
	if(!BasePath.EndsWith("/")) BasePath += TEXT("/");
	OutGameDir = BasePath;
}

const TCHAR* YGenericPlatformMisc::GameDir()
{
	static YString GameDir = TEXT("");

	// track if last time we called this function the .ini was ready and had fixed the GameName case
	static bool bWasIniReady = false;
	bool bIsIniReady = GConfig && GConfig->IsReadyForUse();
	if (bWasIniReady != bIsIniReady)
	{
		GameDir = TEXT("");
		bWasIniReady = bIsIniReady;
	}

	// try using the override game dir if it exists, which will override all below logic
	if (GameDir.Len() == 0)
	{
		GameDir = OverrideGameDir;
	}

	if (GameDir.Len() == 0)
	{
		if (FPlatformProperties::IsProgram())
		{
			// monolithic, game-agnostic executables, the ini is in Engine/Config/Platform
			GameDir = YString::Printf(TEXT("../../../Engine/Programs/%s/"), FApp::GetGameName());
		}
		else
		{
			if (YPaths::IsProjectFilePathSet())
			{
				GenericPlatformMisc_GetProjectFilePathGameDir(GameDir);
			}
			else if ( FApp::HasGameName() )
			{
				if (FPlatformProperties::IsMonolithicBuild() == false)
				{
					// No game project file, but has a game name, use the game folder next to the working directory
					GameDir = YString::Printf(TEXT("../../../%s/"), FApp::GetGameName());
					YString GameBinariesDir = GameDir / TEXT("Binaries/");
					if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*GameBinariesDir) == false)
					{
						// The game binaries folder was *not* found
						// 
						YPlatformMisc::LowLevelOutputDebugStringf(TEXT("Failed to find game directory: %s\n"), *GameDir);

						// Use the uprojectdirs
						YString GameProjectFile = FUProjectDictionary::GetDefault().GetRelativeProjectPathForGame(FApp::GetGameName(), FPlatformProcess::BaseDir());
						if (GameProjectFile.IsEmpty() == false)
						{
							// We found a project folder for the game
							YPaths::SetProjectFilePath(GameProjectFile);
							GameDir = YPaths::GetPath(GameProjectFile);
							if (GameDir.EndsWith(TEXT("/")) == false)
							{
								GameDir += TEXT("/");
							}
						}
					}
				}
				else
				{
#if !PLATFORM_DESKTOP
					GameDir = YString::Printf(TEXT("../../../%s/"), FApp::GetGameName());
#else
					// This assumes the game executable is in <GAME>/Binaries/<PLATFORM>
					GameDir = TEXT("../../");

					// Determine a relative path that includes the game folder itself, if possible...
					YString LocalBaseDir = YString(FPlatformProcess::BaseDir());
					YString LocalRootDir = YPaths::RootDir();
					YString BaseToRoot = LocalRootDir;
					YPaths::MakePathRelativeTo(BaseToRoot, *LocalBaseDir);
					YString LocalGameDir = LocalBaseDir / TEXT("../../");
					YPaths::CollapseRelativeDirectories(LocalGameDir);
					YPaths::MakePathRelativeTo(LocalGameDir, *(YPaths::RootDir()));
					LocalGameDir = BaseToRoot / LocalGameDir;
					if (LocalGameDir.EndsWith(TEXT("/")) == false)
					{
						LocalGameDir += TEXT("/");
					}

					YString CheckLocal = YPaths::ConvertRelativePathToFull(LocalGameDir);
					YString CheckGame = YPaths::ConvertRelativePathToFull(GameDir);
					if (CheckLocal == CheckGame)
					{
						GameDir = LocalGameDir;
					}

					if (GameDir.EndsWith(TEXT("/")) == false)
					{
						GameDir += TEXT("/");
					}
#endif
				}
			}
			else
			{
				// Get a writable engine directory
				GameDir = YPaths::EngineUserDir();
				YPaths::NormalizeFilename(GameDir);
				GameDir = FFileManagerGeneric::DefaultConvertToRelativePath(*GameDir);
				if(!GameDir.EndsWith(TEXT("/"))) GameDir += TEXT("/");
			}
		}
	}

	return *GameDir;
}

YString YGenericPlatformMisc::CloudDir()
{
	return YPaths::GameSavedDir() + TEXT("Cloud/");
}

const TCHAR* YGenericPlatformMisc::GamePersistentDownloadDir()
{
	static YString GamePersistentDownloadDir = TEXT("");

	if (GamePersistentDownloadDir.Len() == 0)
	{
		YString BaseGameDir = GameDir();

		if (BaseGameDir.Len() > 0)
		{
			GamePersistentDownloadDir = BaseGameDir / TEXT("PersistentDownloadDir");
		}
	}
	return *GamePersistentDownloadDir;
}

uint32 YGenericPlatformMisc::GetStandardPrintableKeyMap(uint32* KeyCodes, YString* KeFNames, uint32 MaxMappings, bool bMapUppercaseKeys, bool bMapLowercaseKeys)
{
	uint32 NumMappings = 0;

#define ADDKEYMAP(KeyCode, KeFName)		if (NumMappings<MaxMappings) { KeyCodes[NumMappings]=KeyCode; KeFNames[NumMappings]=KeFName; ++NumMappings; };

	ADDKEYMAP( '0', TEXT("Zero") );
	ADDKEYMAP( '1', TEXT("One") );
	ADDKEYMAP( '2', TEXT("Two") );
	ADDKEYMAP( '3', TEXT("Three") );
	ADDKEYMAP( '4', TEXT("Four") );
	ADDKEYMAP( '5', TEXT("Five") );
	ADDKEYMAP( '6', TEXT("Six") );
	ADDKEYMAP( '7', TEXT("Seven") );
	ADDKEYMAP( '8', TEXT("Eight") );
	ADDKEYMAP( '9', TEXT("Nine") );

	// we map both upper and lower
	if (bMapUppercaseKeys)
	{
		ADDKEYMAP( 'A', TEXT("A") );
		ADDKEYMAP( 'B', TEXT("B") );
		ADDKEYMAP( 'C', TEXT("C") );
		ADDKEYMAP( 'D', TEXT("D") );
		ADDKEYMAP( 'E', TEXT("E") );
		ADDKEYMAP( 'F', TEXT("F") );
		ADDKEYMAP( 'G', TEXT("G") );
		ADDKEYMAP( 'H', TEXT("H") );
		ADDKEYMAP( 'I', TEXT("I") );
		ADDKEYMAP( 'J', TEXT("J") );
		ADDKEYMAP( 'K', TEXT("K") );
		ADDKEYMAP( 'L', TEXT("L") );
		ADDKEYMAP( 'M', TEXT("M") );
		ADDKEYMAP( 'N', TEXT("N") );
		ADDKEYMAP( 'O', TEXT("O") );
		ADDKEYMAP( 'P', TEXT("P") );
		ADDKEYMAP( 'Q', TEXT("Q") );
		ADDKEYMAP( 'R', TEXT("R") );
		ADDKEYMAP( 'S', TEXT("S") );
		ADDKEYMAP( 'T', TEXT("T") );
		ADDKEYMAP( 'U', TEXT("U") );
		ADDKEYMAP( 'V', TEXT("V") );
		ADDKEYMAP( 'W', TEXT("W") );
		ADDKEYMAP( 'X', TEXT("X") );
		ADDKEYMAP( 'Y', TEXT("Y") );
		ADDKEYMAP( 'Z', TEXT("Z") );
	}

	if (bMapLowercaseKeys)
	{
		ADDKEYMAP( 'a', TEXT("A") );
		ADDKEYMAP( 'b', TEXT("B") );
		ADDKEYMAP( 'c', TEXT("C") );
		ADDKEYMAP( 'd', TEXT("D") );
		ADDKEYMAP( 'e', TEXT("E") );
		ADDKEYMAP( 'f', TEXT("F") );
		ADDKEYMAP( 'g', TEXT("G") );
		ADDKEYMAP( 'h', TEXT("H") );
		ADDKEYMAP( 'i', TEXT("I") );
		ADDKEYMAP( 'j', TEXT("J") );
		ADDKEYMAP( 'k', TEXT("K") );
		ADDKEYMAP( 'l', TEXT("L") );
		ADDKEYMAP( 'm', TEXT("M") );
		ADDKEYMAP( 'n', TEXT("N") );
		ADDKEYMAP( 'o', TEXT("O") );
		ADDKEYMAP( 'p', TEXT("P") );
		ADDKEYMAP( 'q', TEXT("Q") );
		ADDKEYMAP( 'r', TEXT("R") );
		ADDKEYMAP( 's', TEXT("S") );
		ADDKEYMAP( 't', TEXT("T") );
		ADDKEYMAP( 'u', TEXT("U") );
		ADDKEYMAP( 'v', TEXT("V") );
		ADDKEYMAP( 'w', TEXT("W") );
		ADDKEYMAP( 'x', TEXT("X") );
		ADDKEYMAP( 'y', TEXT("Y") );
		ADDKEYMAP( 'z', TEXT("Z") );
	}

	ADDKEYMAP( ';', TEXT("Semicolon") );
	ADDKEYMAP( '=', TEXT("Equals") );
	ADDKEYMAP( ',', TEXT("Comma") );
	ADDKEYMAP( '-', TEXT("Hyphen") );
	ADDKEYMAP( '.', TEXT("Period") );
	ADDKEYMAP( '/', TEXT("Slash") );
	ADDKEYMAP( '`', TEXT("Tilde") );
	ADDKEYMAP( '[', TEXT("LeftBracket") );
	ADDKEYMAP( '\\', TEXT("Backslash") );
	ADDKEYMAP( ']', TEXT("RightBracket") );
	ADDKEYMAP( '\'', TEXT("Apostrophe") );
	ADDKEYMAP(' ', TEXT("SpaceBar"));

	// AZERTY Keys
	ADDKEYMAP( '&', TEXT("Ampersand") );
	ADDKEYMAP( '*', TEXT("Asterix") );
	ADDKEYMAP( '^', TEXT("Caret") );
	ADDKEYMAP( ':', TEXT("Colon") );
	ADDKEYMAP( '$', TEXT("Dollar") );
	ADDKEYMAP( '!', TEXT("Exclamation") );
	ADDKEYMAP( '(', TEXT("LeftParantheses") );
	ADDKEYMAP( ')', TEXT("RightParantheses") );
	ADDKEYMAP( '"', TEXT("Quote") );
	ADDKEYMAP( '_', TEXT("Underscore") );
	ADDKEYMAP( 224, TEXT("A_AccentGrave") );
	ADDKEYMAP( 231, TEXT("C_Cedille") );
	ADDKEYMAP( 233, TEXT("E_AccentAigu") );
	ADDKEYMAP( 232, TEXT("E_AccentGrave") );
	ADDKEYMAP( 167, TEXT("Section") );

	return NumMappings;
}

const TCHAR* YGenericPlatformMisc::GetUBTPlatform()
{
	return TEXT( PREPROCESSOR_TO_STRING(UBT_COMPILED_PLATFORM) );
}

const TCHAR* YGenericPlatformMisc::GetUBTTarget()
{
    return TEXT(PREPROCESSOR_TO_STRING(UBT_COMPILED_TARGET));
}

const TCHAR* YGenericPlatformMisc::GetDefaultDeviceProfileName()
{
	return TEXT("Default");
}

void YGenericPlatformMisc::SetOverrideGameDir(const YString& InOverrideDir)
{
	OverrideGameDir = InOverrideDir;
}

int32 YGenericPlatformMisc::NumberOfCoresIncludingHyperthreads()
{
	return YPlatformMisc::NumberOfCores();
}

int32 YGenericPlatformMisc::NumberOfWorkerThreadsToSpawn()
{
	static int32 MaxGameThreads = 4;
	static int32 MaxThreads = 16;

	int32 NumberOfCores = YPlatformMisc::NumberOfCores();
	int32 MaxWorkerThreadsWanted = (IsRunningGame() || IsRunningDedicatedServer() || IsRunningClientOnly()) ? MaxGameThreads : MaxThreads;
	// need to spawn at least one worker thread (see FTaskGraphImplementation)
	return YMath::Max(YMath::Min(NumberOfCores - 1, MaxWorkerThreadsWanted), 1);
}

int32 YGenericPlatformMisc::NumberOfIOWorkerThreadsToSpawn()
{
	return 4;
}

void YGenericPlatformMisc::GetValidTargetPlatforms(class TArray<class YString>& TargetPlatformNames)
{
	// by default, just return the running PlatformName as the only TargetPlatform we support
	TargetPlatformNames.Add(FPlatformProperties::PlatformName());
}

TArray<uint8> YGenericPlatformMisc::GetSystemFontBytes()
{
	return TArray<uint8>();
}

const TCHAR* YGenericPlatformMisc::GetDefaultPathSeparator()
{
	return TEXT( "/" );
}

bool YGenericPlatformMisc::GetSHA256Signature(const void* Data, uint32 ByteSize, FSHA256Signature& OutSignature)
{
	checkf(false, TEXT("No SHA256 Platform implementation"));
	FMemory::Memzero(OutSignature.Signature);
	return false;
}

YString YGenericPlatformMisc::GetDefaultLocale()
{
#if UE_ENABLE_ICU
	icu::Locale ICUDefaultLocale = icu::Locale::getDefault();
	return YString(ICUDefaultLocale.getName());
#else
	return TEXT("en");
#endif
}

FText YGenericPlatformMisc::GetFileManagerName()
{
	return NSLOCTEXT("GenericPlatform", "FileManagerName", "File Manager");
}

bool YGenericPlatformMisc::IsRunningOnBattery()
{
	return false;
}

FGuid YGenericPlatformMisc::GetMachineId()
{
	static FGuid MachineId;
	YString MachineIdString;

	// Check to see if we already have a valid machine ID to use
	if( !MachineId.IsValid() && (!YPlatformMisc::GetStoredValue( TEXT( "Epic Games" ), TEXT( "Unreal Engine/Identifiers" ), TEXT( "MachineId" ), MachineIdString ) || !FGuid::Parse( MachineIdString, MachineId )) )
	{
		// No valid machine ID, generate and save a new one
		MachineId = FGuid::NewGuid();
		MachineIdString = MachineId.ToString( EGuidFormats::Digits );

		if( !YPlatformMisc::SetStoredValue( TEXT( "Epic Games" ), TEXT( "Unreal Engine/Identifiers" ), TEXT( "MachineId" ), MachineIdString ) )
		{
			// Failed to persist the machine ID - reset it to zero to avoid returning a transient value
			MachineId = FGuid();
		}
	}

	return MachineId;
}

YString YGenericPlatformMisc::GetLoginId()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	FGuid Id = YPlatformMisc::GetMachineId();
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	// force an empty string if we cannot determine an ID.
	if (Id == FGuid())
	{
		return YString();
	}
	return Id.ToString(EGuidFormats::Digits).ToLower();
}


YString YGenericPlatformMisc::GetEpicAccountId()
{
	YString AccountId;
	YPlatformMisc::GetStoredValue( TEXT( "Epic Games" ), TEXT( "Unreal Engine/Identifiers" ), TEXT( "AccountId" ), AccountId );
	return AccountId;
}

bool YGenericPlatformMisc::SetEpicAccountId( const YString& AccountId )
{
	return YPlatformMisc::SetStoredValue( TEXT( "Epic Games" ), TEXT( "Unreal Engine/Identifiers" ), TEXT( "AccountId" ), AccountId );
}

EConvertibleLaptopMode YGenericPlatformMisc::GetConvertibleLaptopMode()
{
	return EConvertibleLaptopMode::NotSupported;
}

const TCHAR* YGenericPlatformMisc::GetEngineMode()
{
	return	
		IsRunningCommandlet() ? TEXT( "Commandlet" ) :
		GIsEditor ? TEXT( "Editor" ) :
		IsRunningDedicatedServer() ? TEXT( "Server" ) :
		TEXT( "Game" );
}

TArray<YString> YGenericPlatformMisc::GetPreferredLanguages()
{
	// not implemented by default
	return TArray<YString>();
}

YString YGenericPlatformMisc::GetLocalCurrencyCode()
{
	// not implemented by default
	return YString();
}

YString YGenericPlatformMisc::GetLocalCurrencySymbol()
{
	// not implemented by default
	return YString();
}

void YGenericPlatformMisc::PlatformPreInit()
{
	FGenericCrashContext::Initialize();
}

YString YGenericPlatformMisc::GetOperatingSystemId()
{
	// not implemented by default.
	return YString();
}

void YGenericPlatformMisc::RegisterForRemoteNotifications()
{
	// not implemented by default
}
