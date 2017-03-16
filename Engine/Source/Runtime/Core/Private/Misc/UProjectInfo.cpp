// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/UProjectInfo.h"
#include "Logging/LogMacros.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUProjectInfo, Verbose, All);
DEFINE_LOG_CATEGORY(LogUProjectInfo);

FUProjectDictionary::FUProjectDictionary(const YString& InRootDir)
{
	RootDir = InRootDir;
	Refresh();
}

void FUProjectDictionary::Refresh()
{
	ProjectRootDirs.Reset();
	ShortProjectNameDictionary.Reset();

	// Find all the .uprojectdirs files contained in the root folder
	TArray<YString> ProjectDirsFiles;
	IFileManager::Get().FindFiles(ProjectDirsFiles, *(RootDir / YString(TEXT("*.uprojectdirs"))), true, false);

	// Get the normalized path to the root directory
	YString NormalizedRootDir = YPaths::ConvertRelativePathToFull(RootDir);
	YPaths::NormalizeDirectoryName(NormalizedRootDir);
	YString NormalizedRootDirPrefix = NormalizedRootDir / TEXT("");

	// Add all the project root directories to ProjectRootDirs
	for(const YString& ProjectDirsFile: ProjectDirsFiles)
	{
		TArray<YString> Lines;
		if(FFileHelper::LoadANSITextFileToStrings(*(RootDir / ProjectDirsFile), &IFileManager::Get(), Lines))
		{
			for(const YString& Line: Lines)
			{
				YString Entry = YString(Line).Trim();
				if(!Entry.IsEmpty() && !Entry.StartsWith(";"))
				{
					YString DirectoryName = YPaths::ConvertRelativePathToFull(RootDir, Entry);
					YPaths::NormalizeDirectoryName(DirectoryName);
					if (DirectoryName.StartsWith(NormalizedRootDirPrefix) || DirectoryName == NormalizedRootDir)
					{
						ProjectRootDirs.AddUnique(DirectoryName);
					}
					else
					{
						UE_LOG(LogUProjectInfo, Warning, TEXT("Project search path '%s' is not under root directory, ignoring."), *Entry);
					}
				}
			}
		}
	}

	// Search for all the projects under each root directory
	for(const YString& ProjectRootDir: ProjectRootDirs)
	{
		// Enumerate the subdirectories
		TArray<YString> ProjectDirs;
		IFileManager::Get().FindFiles(ProjectDirs, *(ProjectRootDir / YString(TEXT("*"))), false, true);

		// Check each one for project files
		for(const YString& ProjectDir: ProjectDirs)
		{
			// Enumerate all the project files
			TArray<YString> ProjectFiles;
			IFileManager::Get().FindFiles(ProjectFiles, *(ProjectRootDir / ProjectDir / TEXT("*.uproject")), true, false);

			// Add all the projects to the dictionary
			for(const YString& ProjectFile: ProjectFiles)
			{
				YString ShortName = YPaths::GetBaseFilename(ProjectFile);
				YString FullProjectFile = ProjectRootDir / ProjectDir / ProjectFile;
				ShortProjectNameDictionary.Add(ShortName, FullProjectFile);
			}
		}
	}
}

bool FUProjectDictionary::IsForeignProject(const YString& InProjectFileName) const
{
	YString ProjectFileName = YPaths::ConvertRelativePathToFull(InProjectFileName);

	// Check if it's already in the project dictionary
	for(TMap<YString, YString>::TConstIterator Iter(ShortProjectNameDictionary); Iter; ++Iter)
	{
		if(Iter.Value() == ProjectFileName)
		{
			return false;
		}
	}

	// If not, it may be a new project. Check if its parent directory is a project root dir.
	YString ProjectRootDir = YPaths::GetPath(YPaths::GetPath(ProjectFileName));
	if(ProjectRootDirs.Contains(ProjectRootDir))
	{
		return false;
	}

	// Otherwise it's a foreign project
	return true;
}

YString FUProjectDictionary::GetRelativeProjectPathForGame(const TCHAR* InGameName, const YString& BaseDir) const
{
	const YString* ProjectFile = ShortProjectNameDictionary.Find(*(YString(InGameName).ToLower()));
	if (ProjectFile != NULL)
	{
		YString RelativePath = *ProjectFile;
		YPaths::MakePathRelativeTo(RelativePath, *BaseDir);
		return RelativePath;
	}
	return TEXT("");
}

TArray<YString> FUProjectDictionary::GetProjectPaths() const
{
	TArray<YString> Paths;
	ShortProjectNameDictionary.GenerateValueArray(Paths);
	return Paths;
}

FUProjectDictionary& FUProjectDictionary::GetDefault()
{
	static FUProjectDictionary DefaultDictionary(YPaths::RootDir());

#if !NO_LOGGING
	static bool bHaveLoggedProjects = false;
	if(!bHaveLoggedProjects)
	{
		UE_LOG(LogUProjectInfo, Log, TEXT("Found projects:"));
		for(TMap<YString, YString>::TConstIterator Iter(DefaultDictionary.ShortProjectNameDictionary); Iter; ++Iter)
		{
			UE_LOG(LogUProjectInfo, Log, TEXT("    %s: \"%s\""), *Iter.Key(), *Iter.Value());
		}
		bHaveLoggedProjects = true;
	}
#endif

	return DefaultDictionary;
}
