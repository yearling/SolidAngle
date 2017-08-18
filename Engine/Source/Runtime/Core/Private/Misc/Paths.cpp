// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

// Core includes.
#include "Misc/Paths.h"
#include "UObject/NameTypes.h"
#include "Logging/LogMacros.h"
#include "HAL/FileManager.h"
#include "Misc/Parse.h"
#include "Misc/ScopeLock.h"
#include "Misc/CommandLine.h"
#include "Internationalization/Text.h"
#include "Internationalization/Internationalization.h"
#include "Misc/Guid.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/App.h"
#include "Misc/EngineVersion.h"

DEFINE_LOG_CATEGORY_STATIC(LogPaths, Log, All);


/*-----------------------------------------------------------------------------
	Path helpers for retrieving game dir, engine dir, etc.
-----------------------------------------------------------------------------*/

namespace UE4Paths_Private
{
	auto IsSlashOrBackslash    = [](TCHAR C) { return C == TEXT('/') || C == TEXT('\\'); };
	auto IsNotSlashOrBackslash = [](TCHAR C) { return C != TEXT('/') && C != TEXT('\\'); };

	YString GameSavedDir()
	{
		YString Result = YPaths::GameUserDir();

		YString NonDefaultSavedDirSuffix;
		if (FParse::Value(FCommandLine::Get(), TEXT("-saveddirsuffix="), NonDefaultSavedDirSuffix))
		{
			for (int32 CharIdx = 0; CharIdx < NonDefaultSavedDirSuffix.Len(); ++CharIdx)
			{
				if (!FCString::Strchr(VALID_SAVEDDIRSUFFIX_CHARACTERS, NonDefaultSavedDirSuffix[CharIdx]))
				{
					NonDefaultSavedDirSuffix.RemoveAt(CharIdx, 1, false);
					--CharIdx;
				}
			}

			if (!NonDefaultSavedDirSuffix.IsEmpty())
			{
				Result += TEXT("Saved_") + NonDefaultSavedDirSuffix + TEXT("/");
			}
		}
		else
		{
			Result += TEXT("Saved/");
		}

		return Result;
	}

	YString ConvertRelativePathToFullInternal(YString&& BasePath, YString&& InPath)
	{
		YString FullyPathed;
		if ( YPaths::IsRelative(InPath) )
		{
			FullyPathed  = MoveTemp(BasePath);
			FullyPathed /= MoveTemp(InPath);
		}
		else
		{
			FullyPathed = MoveTemp(InPath);
		}

		YPaths::NormalizeFilename(FullyPathed);
		YPaths::CollapseRelativeDirectories(FullyPathed);

		if (FullyPathed.Len() == 0)
		{
			// Empty path is not absolute, and '/' is the best guess across all the platforms.
			// This substituion is not valid for Windows of course; however CollapseRelativeDirectories() will not produce an empty
			// absolute path on Windows as it takes care not to remove the drive letter.
			FullyPathed = TEXT("/");
		}

		return FullyPathed;
	}
}

bool YPaths::ShouldSaveToUserDir()
{
	static bool bShouldSaveToUserDir = FApp::IsInstalled() || FParse::Param(FCommandLine::Get(), TEXT("SaveToUserDir"));
	return bShouldSaveToUserDir;
}

YString YPaths::LaunchDir()
{
	return YString(YPlatformMisc::LaunchDir());
}

YString YPaths::EngineDir()
{
	return YString(YPlatformMisc::EngineDir());
}

YString YPaths::EngineUserDir()
{
	if (ShouldSaveToUserDir() || FApp::IsEngineInstalled())
	{
		return YPaths::Combine(FPlatformProcess::UserSettingsDir(), *FApp::GetEpicProductIdentifier(), *FEngineVersion::Current().ToString(EVersionComponent::Minor)) + TEXT("/");
	}
	else
	{
		return YPaths::EngineDir();
	}
}

YString YPaths::EngineVersionAgnosticUserDir()
{
	if (ShouldSaveToUserDir() || FApp::IsEngineInstalled())
	{
		return YPaths::Combine(FPlatformProcess::UserSettingsDir(), *FApp::GetEpicProductIdentifier(), TEXT("Common")) + TEXT("/");
	}
	else
	{
		return YPaths::EngineDir();
	}
}

YString YPaths::EngineContentDir()
{
	return YPaths::EngineDir() + TEXT("Content/");
}

YString YPaths::EngineConfigDir()
{
	return YPaths::EngineDir() + TEXT("Config/");
}

YString YPaths::EngineIntermediateDir()
{
	return YPaths::EngineDir() + TEXT("Intermediate/");
}

YString YPaths::EngineSavedDir()
{
	return EngineUserDir() + TEXT("Saved/");
}

YString YPaths::EnginePluginsDir()
{
	return YPaths::EngineDir() + TEXT("Plugins/");
}

YString YPaths::RootDir()
{
	return YString(YPlatformMisc::RootDir());
}

YString YPaths::GameDir()
{
	return YString(YPlatformMisc::GameDir());
}

YString YPaths::GameUserDir()
{
	if (ShouldSaveToUserDir())
	{
		return YPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetGameName()) + TEXT("/");
	}
	else
	{
		YString UserDir;
		if (FParse::Value(FCommandLine::Get(), TEXT("UserDir="), UserDir))
		{
			return YPaths::Combine(*YPaths::GameDir(), *UserDir) + TEXT("/");
		}

		return YPaths::GameDir();
	}
}

YString YPaths::GameContentDir()
{
	return YPaths::GameDir() + TEXT("Content/");
}

YString YPaths::GameConfigDir()
{
	return YPaths::GameDir() + TEXT("Config/");
}

YString YPaths::GameSavedDir()
{
	static YString Result = UE4Paths_Private::GameSavedDir();
	return Result;
}

YString YPaths::GameIntermediateDir()
{
	return GameUserDir() + TEXT("Intermediate/");
}

YString YPaths::GamePluginsDir()
{
	return YPaths::GameDir() + TEXT("Plugins/");
}

YString YPaths::GamePersistentDownloadDir()
{
	return YPlatformMisc::GamePersistentDownloadDir();
}

YString YPaths::SourceConfigDir()
{
	return YPaths::GameDir() + TEXT("Config/");
}

YString YPaths::GeneratedConfigDir()
{
#if PLATFORM_MAC
	return FPlatformProcess::UserPreferencesDir();
#else
	return YPaths::GameSavedDir() + TEXT("Config/");
#endif
}

YString YPaths::SandboxesDir()
{
	return YPaths::GameDir() + TEXT("Saved/Sandboxes");
}

YString YPaths::ProfilingDir()
{
	return YPaths::GameSavedDir() + TEXT("Profiling/");
}

YString YPaths::ScreenShotDir()
{
	return YPaths::GameSavedDir() + TEXT("Screenshots/") + FPlatformProperties::PlatformName() + TEXT("/");
}

YString YPaths::BugItDir()
{
	return YPaths::GameSavedDir() + TEXT("BugIt/") + FPlatformProperties::PlatformName() + TEXT("/");
}

YString YPaths::VideoCaptureDir()
{
	return YPaths::GameSavedDir() + TEXT("VideoCaptures/");
}

YString YPaths::GameLogDir()
{
#if PLATFORM_MAC || PLATFORM_XBOXONE
	return FPlatformProcess::UserLogsDir();
#else
	return YPaths::GameSavedDir() + TEXT("Logs/");
#endif
}

YString YPaths::AutomationDir()
{
	return YPaths::GameSavedDir() + TEXT("Automation/");
}

YString YPaths::AutomationTransientDir()
{
	return YPaths::AutomationDir() + TEXT("Transient/");
}

YString YPaths::AutomationLogDir()
{
	return YPaths::AutomationDir() + TEXT("Logs/");
}

YString YPaths::CloudDir()
{
	return YPlatformMisc::CloudDir();
}

YString YPaths::GameDevelopersDir()
{
	return YPaths::GameContentDir() + TEXT("Developers/");
}

YString YPaths::GameUserDeveloperDir()
{
	static YString UserFolder;

	if ( UserFolder.Len() == 0 )
	{
		// The user folder is the user name without any invalid characters
		const YString InvalidChars = INVALID_LONGPACKAGE_CHARACTERS;
		const YString& UserName = FPlatformProcess::UserName();
		
		UserFolder = UserName;
		
		for (int32 CharIdx = 0; CharIdx < InvalidChars.Len(); ++CharIdx)
		{
			const YString Char = InvalidChars.Mid(CharIdx, 1);
			UserFolder = UserFolder.Replace(*Char, TEXT("_"), ESearchCase::CaseSensitive);
		}
	}

	return YPaths::GameDevelopersDir() + UserFolder + TEXT("/");
}

YString YPaths::DiffDir()
{
	return YPaths::GameSavedDir() + TEXT("Diff/");
}

const TArray<YString>& YPaths::GetEngineLocalizationPaths()
{
	static TArray<YString> Results;
	static bool HasInitialized = false;

	if(!HasInitialized)
	{
		if(GConfig && GConfig->IsReadyForUse())
		{
			GConfig->GetArray( TEXT("Internationalization"), TEXT("LocalizationPaths"), Results, GEngineIni );
			if(!Results.Num())
			{
				UE_LOG(LogInit, Warning, TEXT("No paths for engine localization data were specifed in the engine configuration."));
			}
			HasInitialized = true;
		}
		else
		{
			Results.AddUnique(TEXT("../../../Engine/Content/Localization/Engine")); // Hardcoded convention.
		}
	}

	return Results;
}

const TArray<YString>& YPaths::GetEditorLocalizationPaths()
{
	static TArray<YString> Results;
	static bool HasInitialized = false;

	if(!HasInitialized)
	{
		if(GConfig && GConfig->IsReadyForUse())
		{
			GConfig->GetArray( TEXT("Internationalization"), TEXT("LocalizationPaths"), Results, GEditorIni );
			if(!Results.Num())
			{
				UE_LOG(LogInit, Warning, TEXT("No paths for editor localization data were specifed in the editor configuration."));
			}
			HasInitialized = true;
		}
		else
		{
			Results.AddUnique(TEXT("../../../Engine/Content/Localization/Editor")); // Hardcoded convention.
		}
	}

	return Results;
}

const TArray<YString>& YPaths::GetPropertFNameLocalizationPaths()
{
	static TArray<YString> Results;
	static bool HasInitialized = false;

	if(!HasInitialized)
	{
		if(GConfig && GConfig->IsReadyForUse())
		{
			GConfig->GetArray( TEXT("Internationalization"), TEXT("PropertFNameLocalizationPaths"), Results, GEditorIni );
			if(!Results.Num())
			{
				UE_LOG(LogInit, Warning, TEXT("No paths for property name localization data were specifed in the editor configuration."));
			}
			HasInitialized = true;
		}
		else
		{
			Results.AddUnique(TEXT("../../../Engine/Content/Localization/PropertFNames")); // Hardcoded convention.
		}
	}

	return Results;
}

const TArray<YString>& YPaths::GetToolTipLocalizationPaths() 
{
	static TArray<YString> Results;
	static bool HasInitialized = false;

	if(!HasInitialized)
	{
		if(GConfig && GConfig->IsReadyForUse())
		{
			GConfig->GetArray( TEXT("Internationalization"), TEXT("ToolTipLocalizationPaths"), Results, GEditorIni );
			if(!Results.Num())
			{
				UE_LOG(LogInit, Warning, TEXT("No paths for tooltips localization data were specifed in the editor configuration."));
			}
			HasInitialized = true;
		}
		else
		{
			Results.AddUnique(TEXT("../../../Engine/Content/Localization/ToolTips")); // Hardcoded convention.
		}
	}

	return Results;
}

const TArray<YString>& YPaths::GetGameLocalizationPaths()
{
	static TArray<YString> Results;
	static bool HasInitialized = false;

	if(!HasInitialized)
	{
		if(GConfig && GConfig->IsReadyForUse())
		{
			GConfig->GetArray( TEXT("Internationalization"), TEXT("LocalizationPaths"), Results, GGameIni );
			if(!Results.Num()) // Failed to find localization path.
			{
				UE_LOG(LogPaths, Warning, TEXT("No paths for game localization data were specifed in the game configuration."));
			}
			HasInitialized = true;
		}
	}


	return Results;
}

YString YPaths::GameAgnosticSavedDir()
{
	return EngineSavedDir();
}

YString YPaths::EngineSourceDir()
{
	return YPaths::EngineDir() + TEXT("Source/");
}

YString YPaths::GameSourceDir()
{
	return YPaths::GameDir() + TEXT("Source/");
}

YString YPaths::FeaturePackDir()
{
	return YPaths::RootDir() + TEXT("FeaturePacks/");
}

bool YPaths::IsProjectFilePathSet()
{
	FScopeLock Lock(GameProjectFilePathLock());
	return !GameProjectFilePath.IsEmpty();
}

const YString& YPaths::GetProjectFilePath()
{
	FScopeLock Lock(GameProjectFilePathLock());
	return GameProjectFilePath;
}

void YPaths::SetProjectFilePath( const YString& NewGameProjectFilePath )
{
	FScopeLock Lock(GameProjectFilePathLock());
	GameProjectFilePath = NewGameProjectFilePath;
	YPaths::NormalizeFilename(GameProjectFilePath);
}

YString YPaths::GetExtension( const YString& InPath, bool bIncludeDot )
{
	const YString Filename = GetCleanFilename(InPath);
	int32 DotPos = Filename.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	if (DotPos != INDEX_NONE)
	{
		return Filename.Mid(DotPos + (bIncludeDot ? 0 : 1));
	}

	return TEXT("");
}

YString YPaths::GetCleanFilename(const YString& InPath)
{
	static_assert(INDEX_NONE == -1, "INDEX_NONE assumed to be -1");

	int32 EndPos   = InPath.FindLastCharByPredicate(UE4Paths_Private::IsNotSlashOrBackslash) + 1;
	int32 StartPos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash, EndPos) + 1;

	YString Result = InPath.Mid(StartPos, EndPos - StartPos);
	return Result;
}

YString YPaths::GetCleanFilename(YString&& InPath)
{
	static_assert(INDEX_NONE == -1, "INDEX_NONE assumed to be -1");

	int32 EndPos   = InPath.FindLastCharByPredicate(UE4Paths_Private::IsNotSlashOrBackslash) + 1;
	int32 StartPos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash, EndPos) + 1;

	InPath.RemoveAt(EndPos, InPath.Len() - EndPos, false);
	InPath.RemoveAt(0, StartPos, false);

	return MoveTemp(InPath);
}

YString YPaths::GetBaseFilename( const YString& InPath, bool bRemovePath )
{
	YString Wk = bRemovePath ? GetCleanFilename(InPath) : InPath;

	// remove the extension
	int32 ExtPos = Wk.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	
	// determine the position of the path/leaf separator
	int32 LeafPos = INDEX_NONE;
	if (!bRemovePath)
	{
		LeafPos = Wk.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash);
	}

	if (ExtPos != INDEX_NONE && (LeafPos == INDEX_NONE || ExtPos > LeafPos))
	{
		Wk = Wk.Left(ExtPos);
	}

	return Wk;
}

YString YPaths::GetPath(const YString& InPath)
{
	int32 Pos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash);

	YString Result;
	if (Pos != INDEX_NONE)
	{
		Result = InPath.Left(Pos);
	}

	return Result;
}

YString YPaths::GetPath(YString&& InPath)
{
	int32 Pos = InPath.FindLastCharByPredicate(UE4Paths_Private::IsSlashOrBackslash);

	YString Result;
	if (Pos != INDEX_NONE)
	{
		InPath.RemoveAt(Pos, InPath.Len() - Pos, false);
		Result = MoveTemp(InPath);
	}

	return Result;
}

YString YPaths::ChangeExtension(const YString& InPath, const YString& InNewExtension)
{
	int32 Pos = INDEX_NONE;
	if (InPath.FindLastChar('.', Pos))
	{
		YString Result = InPath.Left(Pos);

		if (InNewExtension.Len() && InNewExtension[0] != '.')
		{
			Result += '.';
		}

		Result += InNewExtension;

		return Result;
	}

	return InPath;
}

bool YPaths::FileExists(const YString& InPath)
{
	return IFileManager::Get().FileExists(*InPath);
}

bool YPaths::DirectoryExists(const YString& InPath)
{
	return IFileManager::Get().DirectoryExists(*InPath);
}

bool YPaths::IsDrive(const YString& InPath)
{
	YString ConvertedPathString = InPath;

	ConvertedPathString = ConvertedPathString.Replace(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);
	const TCHAR* ConvertedPath= *ConvertedPathString;

	// Does Path refer to a drive letter or BNC path?
	if (ConvertedPath[0] == TCHAR(0))
	{
		return true;
	}
	else if (FChar::ToUpper(ConvertedPath[0])!=FChar::ToLower(ConvertedPath[0]) && ConvertedPath[1]==TEXT(':') && ConvertedPath[2]==0)
	{
		return true;
	}
	else if (FCString::Strcmp(ConvertedPath,TEXT("\\"))==0)
	{
		return true;
	}
	else if (FCString::Strcmp(ConvertedPath,TEXT("\\\\"))==0)
	{
		return true;
	}
	else if (ConvertedPath[0]==TEXT('\\') && ConvertedPath[1]==TEXT('\\') && !FCString::Strchr(ConvertedPath+2,TEXT('\\')))
	{
		return true;
	}
	else
	{
		// Need to handle cases such as X:\A\B\..\..\C\..
		// This assumes there is no actual filename in the path (ie, not c:\DIR\File.ext)!
		YString TempPath(ConvertedPath);
		// Make sure there is a '\' at the end of the path
		if (TempPath.Find(TEXT("\\"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) != (TempPath.Len() - 1))
		{
			TempPath += TEXT("\\");
		}

		YString CheckPath = TEXT("");
		int32 ColonSlashIndex = TempPath.Find(TEXT(":\\"), ESearchCase::CaseSensitive);
		if (ColonSlashIndex != INDEX_NONE)
		{
			// Remove the 'X:\' from the start
			CheckPath = TempPath.Right(TempPath.Len() - ColonSlashIndex - 2);
		}
		else
		{
			// See if the first two characters are '\\' to handle \\Server\Foo\Bar cases
			if (TempPath.StartsWith(TEXT("\\\\"), ESearchCase::CaseSensitive) == true)
			{
				CheckPath = TempPath.Right(TempPath.Len() - 2);
				// Find the next slash
				int32 SlashIndex = CheckPath.Find(TEXT("\\"), ESearchCase::CaseSensitive);
				if (SlashIndex != INDEX_NONE)
				{
					CheckPath = CheckPath.Right(CheckPath.Len() - SlashIndex  - 1);
				}
				else
				{
					CheckPath = TEXT("");
				}
			}
		}

		if (CheckPath.Len() > 0)
		{
			// Replace any remaining '\\' instances with '\'
			CheckPath.Replace(TEXT("\\\\"), TEXT("\\"), ESearchCase::CaseSensitive);

			int32 CheckCount = 0;
			int32 SlashIndex = CheckPath.Find(TEXT("\\"), ESearchCase::CaseSensitive);
			while (SlashIndex != INDEX_NONE)
			{
				YString FolderName = CheckPath.Left(SlashIndex);
				if (FolderName == TEXT(".."))
				{
					// It's a relative path, so subtract one from the count
					CheckCount--;
				}
				else
				{
					// It's a real folder, so add one to the count
					CheckCount++;
				}
				CheckPath = CheckPath.Right(CheckPath.Len() - SlashIndex  - 1);
				SlashIndex = CheckPath.Find(TEXT("\\"), ESearchCase::CaseSensitive);
			}

			if (CheckCount <= 0)
			{
				// If there were the same number or greater relative to real folders, it's the root dir
				return true;
			}
		}
	}

	// It's not a drive...
	return false;
}

bool YPaths::IsRelative(const YString& InPath)
{
	// The previous implementation of this function seemed to handle normalized and unnormalized paths, so this one does too for legacy reasons.

	const bool IsRooted = InPath.StartsWith(TEXT("\\"), ESearchCase::CaseSensitive)	||					// Root of the current directory on Windows. Also covers "\\" for UNC or "network" paths.
						  InPath.StartsWith(TEXT("/"), ESearchCase::CaseSensitive)	||					// Root of the current directory on Windows, root on UNIX-likes.  Also covers "\\", considering normalization replaces "\\" with "//".						
						  InPath.StartsWith(TEXT("root:/")) |											// Feature packs use this
						  (InPath.Len() >= 2 && FChar::IsAlpha(InPath[0]) && InPath[1] == TEXT(':'));	// Starts with "<DriveLetter>:"

	return !IsRooted;
}

void YPaths::NormalizeFilename(YString& InPath)
{
	InPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);

	YPlatformMisc::NormalizePath(InPath);
}

void YPaths::NormalizeDirectorFName(YString& InPath)
{
	InPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
	if (InPath.EndsWith(TEXT("/"), ESearchCase::CaseSensitive) && !InPath.EndsWith(TEXT("//"), ESearchCase::CaseSensitive) && !InPath.EndsWith(TEXT(":/"), ESearchCase::CaseSensitive))
	{
		// overwrite trailing slash with terminator
		InPath.GetCharArray()[InPath.Len() - 1] = 0;
		// shrink down
		InPath.TrimToNullTerminator();
	}

	YPlatformMisc::NormalizePath(InPath);
}

bool YPaths::CollapseRelativeDirectories(YString& InPath)
{
	const TCHAR ParentDir[] = TEXT("/..");
	const int32 ParentDirLength = ARRAY_COUNT( ParentDir ) - 1; // To avoid hardcoded values

	for (;;)
	{
		// An empty path is finished
		if (InPath.IsEmpty())
			break;

		// Consider empty paths or paths which start with .. or /.. as invalid
		if (InPath.StartsWith(TEXT(".."), ESearchCase::CaseSensitive) || InPath.StartsWith(ParentDir))
			return false;

		// If there are no "/.."s left then we're done
		const int32 Index = InPath.Find(ParentDir, ESearchCase::CaseSensitive);
		if (Index == -1)
			break;

		int32 PreviousSeparatorIndex = Index;
		for (;;)
		{
			// Find the previous slash
			PreviousSeparatorIndex = YMath::Max(0, InPath.Find( TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd, PreviousSeparatorIndex - 1));

			// Stop if we've hit the start of the string
			if (PreviousSeparatorIndex == 0)
				break;

			// Stop if we've found a directory that isn't "/./"
			if ((Index - PreviousSeparatorIndex) > 1 && (InPath[PreviousSeparatorIndex + 1] != TEXT('.') || InPath[PreviousSeparatorIndex + 2] != TEXT('/')))
				break;
		}

		// If we're attempting to remove the drive letter, that's illegal
		int32 Colon = InPath.Find(TEXT(":"), ESearchCase::CaseSensitive, ESearchDir::FromStart, PreviousSeparatorIndex);
		if (Colon >= 0 && Colon < Index)
			return false;

		InPath.RemoveAt(PreviousSeparatorIndex, Index - PreviousSeparatorIndex + ParentDirLength, false);
	}

	InPath.ReplaceInline(TEXT("./"), TEXT(""), ESearchCase::CaseSensitive);

	InPath.TrimToNullTerminator();

	return true;
}

void YPaths::RemoveDuplicateSlashes(YString& InPath)
{
	while (InPath.Contains(TEXT("//"), ESearchCase::CaseSensitive))
	{
		InPath = InPath.Replace(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
	}
}

void YPaths::MakeStandardFilename(YString& InPath)
{
	// if this is an empty path, use the relative base dir
	if (InPath.Len() == 0)
	{
#if !PLATFORM_HTML5
		InPath = FPlatformProcess::BaseDir();
		YPaths::MakeStandardFilename(InPath);
#else
		// @todo: revisit this as needed
//		InPath = TEXT("/");
#endif
		return;
	}

	YString WithSlashes = InPath.Replace(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);

	YString RootDirectory = YPaths::ConvertRelativePathToFull(YPaths::RootDir());

	// look for paths that cannot be made relative, and are therefore left alone
	// UNC (windows) network path
	bool bCannotBeStandardized = InPath.StartsWith(TEXT("\\\\"), ESearchCase::CaseSensitive);
	// windows drive letter path that doesn't start with base dir
	bCannotBeStandardized |= ((InPath.Len() > 1) && (InPath[1] == ':') && !WithSlashes.StartsWith(RootDirectory));
	// Unix style absolute path that doesn't start with base dir
	bCannotBeStandardized |= (WithSlashes.GetCharArray()[0] == '/' && !WithSlashes.StartsWith(RootDirectory));

	// if it can't be standardized, just return itself
	if (bCannotBeStandardized)
	{
		return;
	}

	// make an absolute path
	
	YString Standardized = YPaths::ConvertRelativePathToFull(InPath);

	// remove duplicate slashes
	YPaths::RemoveDuplicateSlashes(Standardized);

	// make it relative to Engine\Binaries\Platform
	InPath = Standardized.Replace(*RootDirectory, *YPaths::GetRelativePathToRoot());
}

void YPaths::MakePlatformFilename( YString& InPath )
{
	InPath.ReplaceInline(TEXT( "\\" ), YPlatformMisc::GetDefaultPathSeparator(), ESearchCase::CaseSensitive);
	InPath.ReplaceInline(TEXT( "/" ), YPlatformMisc::GetDefaultPathSeparator(), ESearchCase::CaseSensitive);
}

bool YPaths::MakePathRelativeTo( YString& InPath, const TCHAR* InRelativeTo )
{
	YString Target = YPaths::ConvertRelativePathToFull(InPath);
	YString Source = YPaths::ConvertRelativePathToFull(InRelativeTo);
	
	Source = YPaths::GetPath(Source);
	Source.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
	Target.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);

	TArray<YString> TargetArray;
	Target.ParseIntoArray(TargetArray, TEXT("/"), true);
	TArray<YString> SourceArray;
	Source.ParseIntoArray(SourceArray, TEXT("/"), true);

	if (TargetArray.Num() && SourceArray.Num())
	{
		// Check for being on different drives
		if ((TargetArray[0][1] == TEXT(':')) && (SourceArray[0][1] == TEXT(':')))
		{
			if (FChar::ToUpper(TargetArray[0][0]) != FChar::ToUpper(SourceArray[0][0]))
			{
				// The Target and Source are on different drives... No relative path available.
				return false;
			}
		}
	}

	while (TargetArray.Num() && SourceArray.Num() && TargetArray[0] == SourceArray[0])
	{
		TargetArray.RemoveAt(0);
		SourceArray.RemoveAt(0);
	}
	YString Result;
	for (int32 Index = 0; Index < SourceArray.Num(); Index++)
	{
		Result += TEXT("../");
	}
	for (int32 Index = 0; Index < TargetArray.Num(); Index++)
	{
		Result += TargetArray[Index];
		if (Index + 1 < TargetArray.Num())
		{
			Result += TEXT("/");
		}
	}
	
	InPath = Result;
	return true;
}

YString YPaths::ConvertRelativePathToFull(const YString& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(YString(FPlatformProcess::BaseDir()), YString(InPath));
}

YString YPaths::ConvertRelativePathToFull(YString&& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(YString(FPlatformProcess::BaseDir()), MoveTemp(InPath));
}

YString YPaths::ConvertRelativePathToFull(const YString& BasePath, const YString& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(CopyTemp(BasePath), CopyTemp(InPath));
}

YString YPaths::ConvertRelativePathToFull(const YString& BasePath, YString&& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(CopyTemp(BasePath), MoveTemp(InPath));
}

YString YPaths::ConvertRelativePathToFull(YString&& BasePath, const YString& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(MoveTemp(BasePath), CopyTemp(InPath));
}

YString YPaths::ConvertRelativePathToFull(YString&& BasePath, YString&& InPath)
{
	return UE4Paths_Private::ConvertRelativePathToFullInternal(MoveTemp(BasePath), MoveTemp(InPath));
}

YString YPaths::ConvertToSandboxPath( const YString& InPath, const TCHAR* InSandboxName )
{
	YString SandboxDirectory = YPaths::SandboxesDir() / InSandboxName;
	YPaths::NormalizeFilename(SandboxDirectory);
	
	YString RootDirectory = YPaths::RootDir();
	YPaths::CollapseRelativeDirectories(RootDirectory);
	YPaths::NormalizeFilename(RootDirectory);

	YString SandboxPath = YPaths::ConvertRelativePathToFull(InPath);
	if (!SandboxPath.StartsWith(RootDirectory))
	{
		UE_LOG(LogInit, Fatal, TEXT("%s does not start with %s so this is not a valid sandbox path."), *SandboxPath, *RootDirectory);
	}
	check(SandboxPath.StartsWith(RootDirectory));
	SandboxPath.ReplaceInline(*RootDirectory, *SandboxDirectory);

	return SandboxPath;
}

YString YPaths::ConvertFromSandboxPath( const YString& InPath, const TCHAR* InSandboxName )
{
	YString SandboxDirectory =  YPaths::SandboxesDir() / InSandboxName;
	YPaths::NormalizeFilename(SandboxDirectory);
	YString RootDirectory = YPaths::RootDir();
	
	YString SandboxPath(InPath);
	check(SandboxPath.StartsWith(SandboxDirectory));
	SandboxPath.ReplaceInline(*SandboxDirectory, *RootDirectory);
	return SandboxPath;
}

YString YPaths::CreateTempFilename( const TCHAR* Path, const TCHAR* Prefix, const TCHAR* Extension )
{
	YString UniqueFilename;
	do
	{
		UniqueFilename = YPaths::Combine(Path, *YString::Printf(TEXT("%s%s%s"), Prefix, *FGuid::NewGuid().ToString(), Extension));
	}
	while (IFileManager::Get().FileSize(*UniqueFilename) >= 0);
	
	return UniqueFilename;
}

bool YPaths::ValidatePath( const YString& InPath, FText* OutReason )
{
	// Windows has the most restricted file system, and since we're cross platform, we have to respect the limitations of the lowest common denominator
	// # isn't legal. Used for revision specifiers in P4/SVN, and also not allowed on Windows anyway
	// @ isn't legal. Used for revision/label specifiers in P4/SVN
	// ^ isn't legal. While the file-system won't complain about this character, Visual Studio will				
	static const YString RestrictedChars = "/?:&\\*\"<>|%#@^";
	static const YString RestrictedNames[] = {	"CON", "PRN", "AUX", "CLOCK$", "NUL", 
												"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", 
												"LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

	YString Standardized = InPath;
	NormalizeFilename(Standardized);
	CollapseRelativeDirectories(Standardized);
	RemoveDuplicateSlashes(Standardized);

	// The loop below requires that the path not end with a /
	if(Standardized.EndsWith(TEXT("/"), ESearchCase::CaseSensitive))
	{
		Standardized = Standardized.LeftChop(1);
	}

	// Walk each part of the path looking for name errors
	for(int32 StartPos = 0, EndPos = Standardized.Find(TEXT("/"), ESearchCase::CaseSensitive); ; 
		StartPos = EndPos + 1, EndPos = Standardized.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartPos)
		)
	{
		const bool bIsLastPart = EndPos == INDEX_NONE;
		const YString PathPart = Standardized.Mid(StartPos, (bIsLastPart) ? MAX_int32 : EndPos - StartPos);

		// If this is the first part of the path, it's possible for it to be a drive name and is allowed to contain a colon
		if(StartPos == 0 && IsDrive(PathPart))
		{
			if(bIsLastPart)
			{
				break;
			}
			continue;
		}

		// Check for invalid characters
		TCHAR CharString[] = { '\0', '\0' };
		YString MatchedInvalidChars;
		for(const TCHAR* InvalidCharacters = *RestrictedChars; *InvalidCharacters; ++InvalidCharacters)
		{
			CharString[0] = *InvalidCharacters;
			if(PathPart.Contains(CharString))
			{
				MatchedInvalidChars += *InvalidCharacters;
			}
		}
		if(MatchedInvalidChars.Len())
		{
			if(OutReason)
			{
				FFormatNamedArguments Args;
				Args.Add(TEXT("IllegalPathCharacters"), FText::FromString(MatchedInvalidChars));
				*OutReason = FText::Format(NSLOCTEXT("Core", "PathContainsInvalidCharacters", "Path may not contain the following characters: {IllegalPathCharacters}"), Args);
			}
			return false;
		}

		// Check for reserved names
		for(const YString& RestrictedName : RestrictedNames)
		{
			if(PathPart.Equals(RestrictedName, ESearchCase::IgnoreCase))
			{
				if(OutReason)
				{
					FFormatNamedArguments Args;
					Args.Add(TEXT("RestrictedName"), FText::FromString(RestrictedName));
					*OutReason = FText::Format(NSLOCTEXT("Core", "PathContainsRestrictedName", "Path may not contain a restricted name: {RestrictedName}"), Args);
				}
				return false;
			}
		}

		if(bIsLastPart)
		{
			break;
		}
	}

	return true;
}

void YPaths::Split( const YString& InPath, YString& PathPart, YString& FilenamePart, YString& ExtensionPart )
{
	PathPart = GetPath(InPath);
	FilenamePart = GetBaseFilename(InPath);
	ExtensionPart = GetExtension(InPath);
}

const YString& YPaths::GetRelativePathToRoot()
{
	struct FRelativePathInitializer
	{
		YString RelativePathToRoot;

		FRelativePathInitializer()
		{
			YString RootDirectory = YPaths::RootDir();
			YString BaseDirectory = FPlatformProcess::BaseDir();

			// this is how to go from the base dir back to the root
			RelativePathToRoot = RootDirectory;
			YPaths::MakePathRelativeTo(RelativePathToRoot, *BaseDirectory);

			// Ensure that the path ends w/ '/'
			if ((RelativePathToRoot.Len() > 0) && (RelativePathToRoot.EndsWith(TEXT("/"), ESearchCase::CaseSensitive) == false) && (RelativePathToRoot.EndsWith(TEXT("\\"), ESearchCase::CaseSensitive) == false))
			{
				RelativePathToRoot += TEXT("/");
			}
		}
	};

	static FRelativePathInitializer StaticInstance;
	return StaticInstance.RelativePathToRoot;
}

void YPaths::CombineInternal(YString& OutPath, const TCHAR** Pathes, int32 NumPathes)
{
	check(Pathes != NULL && NumPathes > 0);

	int32 OutStringSize = 0;

	for (int32 i=0; i < NumPathes; ++i)
	{
		OutStringSize += FCString::Strlen(Pathes[i]) + 1;
	}

	OutPath.Empty(OutStringSize);
	OutPath += Pathes[0];
	
	for (int32 i=1; i < NumPathes; ++i)
	{
		OutPath /= Pathes[i];
	}
}

bool YPaths::IsSamePath(const YString& PathA, const YString& PathB)
{
	YString TmpA = PathA;
	YString TmpB = PathB;

	MakeStandardFilename(TmpA);
	MakeStandardFilename(TmpB);

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_XBOXONE)
	return FCString::Stricmp(*TmpA, *TmpB) == 0;
#else
	return FCString::Strcmp(*TmpA, *TmpB) == 0;
#endif
}

