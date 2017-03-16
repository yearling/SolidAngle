#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "Containers/Map.h"

/**
* Dictionary of all the non-foreign projects for an engine installation, found by parsing .uprojectdirs files for source directories.
*/
class CORE_API FUProjectDictionary
{
public:
	/** Scans the engine root directory for all the known projects. */
	FUProjectDictionary(const YString& InRootDir);

	/** Refreshes the list of known projects */
	void Refresh();

	/** Determines whether a project is a foreign project or not. */
	bool IsForeignProject(const YString& ProjectFileName) const;

	/** Gets the project filename for the given game. Empty if not found. */
	YString GetRelativeProjectPathForGame(const TCHAR* GameName, const YString& BaseDir) const;

	/** Gets a list of all the known projects. */
	TArray<YString> GetProjectPaths() const;

	/** Gets the project dictionary for the active engine installation. */
	static FUProjectDictionary& GetDefault();

private:
	/** The root directory for this engine installation */
	YString RootDir;

	/** List of project root directories */
	TArray<YString> ProjectRootDirs;

	/** Map of short game names to full project paths. */
	TMap<YString, YString> ShortProjectNameDictionary;
};
