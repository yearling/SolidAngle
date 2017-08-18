// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/EngineBuildSettings.h"
#include "Misc/Paths.h"


bool FEngineBuildSettings::IsInternalBuild()
{
	static bool bIsInternalBuild = YPaths::FileExists( YPaths::EngineDir() / TEXT("Build/NotForLicensees/EpicInternal.txt") );
	return bIsInternalBuild;
}


bool FEngineBuildSettings::IsPerforceBuild()
{
	static bool bIsPerforceBuild = YPaths::FileExists(YPaths::EngineDir() / TEXT("Build/PerforceBuild.txt"));
	return bIsPerforceBuild;
}


bool FEngineBuildSettings::IsSourceDistribution()
{
	return IsSourceDistribution( YPaths::RootDir() );
}


bool FEngineBuildSettings::IsSourceDistribution(const FString& RootDir)
{
	static bool bIsSourceDistribution = YPaths::FileExists(RootDir / TEXT("Engine/Build/SourceDistribution.txt"));
	return bIsSourceDistribution;
}
