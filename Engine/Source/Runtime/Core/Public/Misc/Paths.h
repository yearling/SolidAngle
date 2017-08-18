// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/SolidAngleString.h"
#include "HAL/CriticalSection.h"

/**
* Path helpers for retrieving game dir, engine dir, etc.
*/
class CORE_API YPaths
{
public:

	/**
	* Should the "saved" directory structures be rooted in the user dir or relative to the "engine/game"
	*/
	static bool ShouldSaveToUserDir();

	/**
	* Returns the directory the application was launched from (useful for commandline utilities)
	*/
	static YString LaunchDir();

	/**
	* Returns the base directory of the "core" engine that can be shared across
	* several games or across games & mods. Shaders and base localization files
	* e.g. reside in the engine directory.
	*
	* @return engine directory
	*/
	static YString EngineDir();

	/**
	* Returns the root directory for user-specific engine files. Always writable.
	*
	* @return root user directory
	*/
	static YString EngineUserDir();

	/**
	* Returns the root directory for user-specific engine files which can be shared between versions. Always writable.
	*
	* @return root user directory
	*/
	static YString EngineVersionAgnosticUserDir();

	/**
	* Returns the content directory of the "core" engine that can be shared across
	* several games or across games & mods.
	*
	* @return engine content directory
	*/
	static YString EngineContentDir();

	/**
	* Returns the directory the root configuration files are located.
	*
	* @return root config directory
	*/
	static YString EngineConfigDir();

	/**
	* Returns the intermediate directory of the engine
	*
	* @return content directory
	*/
	static YString EngineIntermediateDir();

	/**
	* Returns the saved directory of the engine
	*
	* @return Saved directory.
	*/
	static YString EngineSavedDir();

	/**
	* Returns the plugins directory of the engine
	*
	* @return Plugins directory.
	*/
	static YString EnginePluginsDir();

	/**
	* Returns the root directory of the engine directory tree
	*
	* @return Root directory.
	*/
	static YString RootDir();

	/**
	* Returns the base directory of the current game by looking at FApp::GetGameName().
	* This is usually a subdirectory of the installation
	* root directory and can be overridden on the command line to allow self
	* contained mod support.
	*
	* @return base directory
	*/
	static YString GameDir();

	/**
	* Returns the root directory for user-specific game files.
	*
	* @return game user directory
	*/
	static YString GameUserDir();

	/**
	* Returns the content directory of the current game by looking at FApp::GetGameName().
	*
	* @return content directory
	*/
	static YString GameContentDir();

	/**
	* Returns the directory the root configuration files are located.
	*
	* @return root config directory
	*/
	static YString GameConfigDir();

	/**
	* Returns the saved directory of the current game by looking at FApp::GetGameName().
	*
	* @return saved directory
	*/
	static YString GameSavedDir();

	/**
	* Returns the intermediate directory of the current game by looking at FApp::GetGameName().
	*
	* @return intermediate directory
	*/
	static YString GameIntermediateDir();

	/**
	* Returns the plugins directory of the current game by looking at FApp::GetGameName().
	*
	* @return plugins directory
	*/
	static YString GamePluginsDir();

	/*
	* Returns the writable directory for downloaded data that persists across play sessions.
	*/
	static YString GamePersistentDownloadDir();

	/**
	* Returns the directory the engine uses to look for the source leaf ini files. This
	* can't be an .ini variable for obvious reasons.
	*
	* @return source config directory
	*/
	static YString SourceConfigDir();

	/**
	* Returns the directory the engine saves generated config files.
	*
	* @return config directory
	*/
	static YString GeneratedConfigDir();

	/**
	* Returns the directory the engine stores sandbox output
	*
	* @return sandbox directory
	*/
	static YString SandboxesDir();

	/**
	* Returns the directory the engine uses to output profiling files.
	*
	* @return log directory
	*/
	static YString ProfilingDir();

	/**
	* Returns the directory the engine uses to output screenshot files.
	*
	* @return screenshot directory
	*/
	static YString ScreenShotDir();

	/**
	* Returns the directory the engine uses to output BugIt files.
	*
	* @return screenshot directory
	*/
	static YString BugItDir();

	/**
	* Returns the directory the engine uses to output user requested video capture files.
	*
	* @return Video capture directory
	*/
	static YString VideoCaptureDir();

	/**
	* Returns the directory the engine uses to output logs. This currently can't
	* be an .ini setting as the game starts logging before it can read from .ini
	* files.
	*
	* @return log directory
	*/
	static YString GameLogDir();

	/**
	* @return The directory for automation save files
	*/
	static YString AutomationDir();

	/**
	* @return The directory for automation save files that are meant to be deleted every run
	*/
	static YString AutomationTransientDir();

	/**
	* @return The directory for automation log files.
	*/
	static YString AutomationLogDir();

	/**
	* @return The directory for local files used in cloud emulation or support
	*/
	static YString CloudDir();

	/**
	* @return The directory that contains subfolders for developer-specific content
	*/
	static YString GameDevelopersDir();

	/**
	* @return The directory that contains developer-specific content for the current user
	*/
	static YString GameUserDeveloperDir();

	/**
	* @return The directory for temp files used for diffing
	*/
	static YString DiffDir();

	/**
	* Returns a list of engine-specific localization paths
	*/
	static const TArray<YString>& GetEngineLocalizationPaths();

	/**
	* Returns a list of editor-specific localization paths
	*/
	static const TArray<YString>& GetEditorLocalizationPaths();

	/**
	* Returns a list of property name localization paths
	*/
	static const TArray<YString>& GetPropertFNameLocalizationPaths();

	/**
	* Returns a list of tool tip localization paths
	*/
	static const TArray<YString>& GetToolTipLocalizationPaths();

	/**
	* Returns a list of game-specific localization paths
	*/
	static const TArray<YString>& GetGameLocalizationPaths();

	/**
	* Returns the saved directory that is not game specific. This is usually the same as
	* EngineSavedDir().
	*
	* @return saved directory
	*/
	static YString GameAgnosticSavedDir();

	/**
	* @return The directory where engine source code files are kept
	*/
	static YString EngineSourceDir();

	/**
	* @return The directory where game source code files are kept
	*/
	static YString GameSourceDir();

	/**
	* @return The directory where feature packs are kept
	*/
	static YString FeaturePackDir();

	/**
	* Checks whether the path to the project file, if any, is set.
	*
	* @return true if the path is set, false otherwise.
	*/
	static bool IsProjectFilePathSet();

	/**
	* Gets the path to the project file.
	*
	* @return Project file path.
	*/
	static const YString& GetProjectFilePath();

	/**
	* Sets the path to the project file.
	*
	* @param NewGameProjectFilePath - The project file path to set.
	*/
	static void SetProjectFilePath(const YString& NewGameProjectFilePath);

	/**
	* Gets the extension for this filename.
	*
	* @param	bIncludeDot		if true, includes the leading dot in the result
	*
	* @return	the extension of this filename, or an empty string if the filename doesn't have an extension.
	*/
	static YString GetExtension(const YString& InPath, bool bIncludeDot = false);

	// Returns the filename (with extension), minus any path information.
	static YString GetCleanFilename(const YString& InPath);

	// Returns the filename (with extension), minus any path information.
	static YString GetCleanFilename(YString&& InPath);

	// Returns the same thing as GetCleanFilename, but without the extension
	static YString GetBaseFilename(const YString& InPath, bool bRemovePath = true);

	// Returns the path in front of the filename
	static YString GetPath(const YString& InPath);

	// Returns the path in front of the filename
	static YString GetPath(YString&& InPath);

	// Changes the extension of the given filename
	static YString ChangeExtension(const YString& InPath, const YString& InNewExtension);

	/** @return true if this file was found, false otherwise */
	static bool FileExists(const YString& InPath);

	/** @return true if this directory was found, false otherwise */
	static bool DirectoryExists(const YString& InPath);

	/** @return true if this path represents a drive */
	static bool IsDrive(const YString& InPath);

	/** @return true if this path is relative */
	static bool IsRelative(const YString& InPath);

	/** Convert all / and \ to TEXT("/") */
	static void NormalizeFilename(YString& InPath);

	/**
	* Checks if two paths are the same.
	*
	* @param PathA First path to check.
	* @param PathB Second path to check.
	*
	* @returns True if both paths are the same. False otherwise.
	*/
	static bool IsSamePath(const YString& PathA, const YString& PathB);

	/** Normalize all / and \ to TEXT("/") and remove any trailing TEXT("/") if the character before that is not a TEXT("/") or a colon */
	static void NormalizeDirectorFName(YString& InPath);

	/**
	* Takes a fully pathed string and eliminates relative pathing (eg: annihilates ".." with the adjacent directory).
	* Assumes all slashes have been converted to TEXT('/').
	* For example, takes the string:
	*	BaseDirectory/SomeDirectory/../SomeOtherDirectory/Filename.ext
	* and converts it to:
	*	BaseDirectory/SomeOtherDirectory/Filename.ext
	*/
	static bool CollapseRelativeDirectories(YString& InPath);

	/**
	* Removes duplicate slashes in paths.
	* Assumes all slashes have been converted to TEXT('/').
	* For example, takes the string:
	*	BaseDirectory/SomeDirectory//SomeOtherDirectory////Filename.ext
	* and converts it to:
	*	BaseDirectory/SomeDirectory/SomeOtherDirectory/Filename.ext
	*/
	static void RemoveDuplicateSlashes(YString& InPath);

	/**
	* Make fully standard "Unreal" pathname:
	*    - Normalizes path separators [NormalizeFilename]
	*    - Removes extraneous separators  [NormalizeDirectorFName, as well removing adjacent separators]
	*    - Collapses internal ..'s
	*    - Makes relative to Engine\Binaries\<Platform> (will ALWAYS start with ..\..\..)
	*/
	static void MakeStandardFilename(YString& InPath);

	/** Takes an "Unreal" pathname and converts it to a platform filename. */
	static void MakePlatformFilename(YString& InPath);

	/**
	* Assuming both paths (or filenames) are relative to the base dir, find the relative path to the InPath.
	*
	* @Param InPath Path to make this path relative to.
	* @return Path relative to InPath.
	*/
	static bool MakePathRelativeTo(YString& InPath, const TCHAR* InRelativeTo);

	/**
	* Converts a relative path name to a fully qualified name relative to the process BaseDir().
	*/
	static YString ConvertRelativePathToFull(const YString& InPath);

	/**
	* Converts a relative path name to a fully qualified name relative to the process BaseDir().
	*/
	static YString ConvertRelativePathToFull(YString&& InPath);

	/**
	* Converts a relative path name to a fully qualified name relative to the specified BasePath.
	*/
	static YString ConvertRelativePathToFull(const YString& BasePath, const YString& InPath);

	/**
	* Converts a relative path name to a fully qualified name relative to the specified BasePath.
	*/
	static YString ConvertRelativePathToFull(const YString& BasePath, YString&& InPath);

	/**
	* Converts a relative path name to a fully qualified name relative to the specified BasePath.
	*/
	static YString ConvertRelativePathToFull(YString&& BasePath, const YString& InPath);

	/**
	* Converts a relative path name to a fully qualified name relative to the specified BasePath.
	*/
	static YString ConvertRelativePathToFull(YString&& BasePath, YString&& InPath);

	/**
	* Converts a normal path to a sandbox path (in Saved/Sandboxes).
	*
	* @param InSandboxName The name of the sandbox.
	*/
	static YString ConvertToSandboxPath(const YString& InPath, const TCHAR* InSandboxName);

	/**
	* Converts a sandbox (in Saved/Sandboxes) path to a normal path.
	*
	* @param InSandboxName The name of the sandbox.
	*/
	static YString ConvertFromSandboxPath(const YString& InPath, const TCHAR* InSandboxName);

	/**
	* Creates a temporary filename with the specified prefix.
	*
	* @param Path The file pathname.
	* @param Prefix The file prefix.
	* @param Extension File extension ('.' required).
	*/
	static YString CreateTempFilename(const TCHAR* Path, const TCHAR* Prefix = TEXT(""), const TCHAR* Extension = TEXT(".tmp"));

	/**
	* Validates that the parts that make up the path contain no invalid characters as dictated by the operating system
	* Note that this is a different set of restrictions to those imposed by FPackageName
	*
	* @param InPath - path to validate
	* @param OutReason - optional parameter to fill with the failure reason
	*/
	static bool ValidatePath(const YString& InPath, FText* OutReason = nullptr);

	/**
	* Parses a fully qualified or relative filename into its components (filename, path, extension).
	*
	* @param	Path		[out] receives the value of the path portion of the input string
	* @param	Filename	[out] receives the value of the filename portion of the input string
	* @param	Extension	[out] receives the value of the extension portion of the input string
	*/
	static void Split(const YString& InPath, YString& PathPart, YString& FilenamePart, YString& ExtensionPart);

	/** Gets the relative path to get from BaseDir to RootDirectory  */
	static const YString& GetRelativePathToRoot();

	FORCEINLINE static YString Combine(const TCHAR* PathA, const TCHAR* PathB)
	{
		const TCHAR* Pathes[] = { PathA, PathB };
		YString Out;

		CombineInternal(Out, Pathes, 2);
		return Out;
	}

	FORCEINLINE static YString Combine(const TCHAR* PathA, const TCHAR* PathB, const TCHAR* PathC)
	{
		const TCHAR* Pathes[] = { PathA, PathB, PathC };
		YString Out;

		CombineInternal(Out, Pathes, 3);
		return Out;
	}

	FORCEINLINE static YString Combine(const TCHAR* PathA, const TCHAR* PathB, const TCHAR* PathC, const TCHAR* PathD)
	{
		const TCHAR* Pathes[] = { PathA, PathB, PathC, PathD };
		YString Out;

		CombineInternal(Out, Pathes, 4);
		return Out;
	}

	FORCEINLINE static YString Combine(const TCHAR* PathA, const TCHAR* PathB, const TCHAR* PathC, const TCHAR* PathD, const TCHAR* PathE)
	{
		const TCHAR* Pathes[] = { PathA, PathB, PathC, PathD, PathE };
		YString Out;

		CombineInternal(Out, Pathes, 5);
		return Out;
	}

	FORCEINLINE static YString Combine(const YString& PathA, const YString& PathB)
	{
		return Combine(*PathA, *PathB);
	}

	FORCEINLINE static YString Combine(const YString& PathA, const YString& PathB, const YString& PathC)
	{
		return Combine(*PathA, *PathB, *PathC);
	}

protected:

	static void CombineInternal(YString& OutPath, const TCHAR** Pathes, int32 NumPathes);

private:

	/** Holds the path to the currently loaded game project file. */
	static YString GameProjectFilePath;

	/** Thread protection for above path */
	FORCEINLINE static FCriticalSection* GameProjectFilePathLock()
	{
		static FCriticalSection Lock;
		return &Lock;
	}
};
