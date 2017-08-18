// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Internationalization/TextLocalizationManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/FileManager.h"
#include "Misc/Parse.h"
#include "Templates/ScopedPointer.h"
#include "Misc/ScopeLock.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Stats/Stats.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/App.h"
#include "Templates/UniquePtr.h"

DEFINE_LOG_CATEGORY_STATIC(LogTextLocalizationManager, Log, All);

static YString AccessedStringBeforeLocLoadedErrorMsg = TEXT("Can't access string. Loc System hasn't been initialized yet!");

void BeginInitTextLocalization()
{
	// Initialize FInternationalization before we bind to OnCultureChanged, otherwise we can accidentally initialize
	// twice since FInternationalization::Initialize sets the culture.
	FInternationalization::Get();

	FInternationalization::Get().OnCultureChanged().AddRaw( &(FTextLocalizationManager::Get()), &FTextLocalizationManager::OnCultureChanged );
}

void EndInitTextLocalization()
{
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("EndInitTextLocalization"), STAT_EndInitTextLocalization, STATGROUP_LoadTime);

	const bool ShouldLoadEditor = WITH_EDITOR;
	const bool ShouldLoadGame = FApp::IsGame();

	FInternationalization& I18N = FInternationalization::Get();

	// Set culture according to configuration now that configs are available.
#if ENABLE_LOC_TESTING
	if( FCommandLine::IsInitialized() && FParse::Param(FCommandLine::Get(), TEXT("LEET")) )
	{
		I18N.SetCurrentCulture(TEXT("LEET"));
	}
	else
#endif
	{
		YString RequestedCultureName;
		if (FParse::Value(FCommandLine::Get(), TEXT("CULTUREFORCOOKING="), RequestedCultureName))
		{
			// Write the culture passed in if first install...
			if (FParse::Param(FCommandLine::Get(), TEXT("firstinstall")))
			{
				GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *RequestedCultureName, GEngineIni);
			}
		}
		// Use culture override specified on commandline.
		else if (FParse::Value(FCommandLine::Get(), TEXT("CULTURE="), RequestedCultureName))
		{
			UE_LOG(LogInit, Log, TEXT("Overriding culture %s with command-line option."), *RequestedCultureName);
		}
		else
#if WITH_EDITOR
			// See if we've been provided a culture override in the editor
			if(GIsEditor && GConfig->GetString( TEXT("Internationalization"), TEXT("Culture"), RequestedCultureName, GEditorSettingsIni ))
			{
				UE_LOG(LogInit, Log, TEXT("Overriding culture %s with editor configuration."), *RequestedCultureName);
			}
			else
#endif // WITH_EDITOR
				// Use culture specified in game configuration.
				if(GConfig->GetString( TEXT("Internationalization"), TEXT("Culture"), RequestedCultureName, GGameUserSettingsIni ))
				{
					UE_LOG(LogInit, Log, TEXT("Overriding culture %s with game configuration."), *RequestedCultureName);
				}
				// Use culture specified in engine configuration.
				else if(GConfig->GetString( TEXT("Internationalization"), TEXT("Culture"), RequestedCultureName, GEngineIni ))
				{
					UE_LOG(LogInit, Log, TEXT("Overriding culture %s with engine configuration."), *RequestedCultureName);
				}
				else
				{
					RequestedCultureName = I18N.GetDefaultCulture()->GetName();
				}

				YString TargetCultureName = RequestedCultureName;
				{
					TArray<YString> LocalizationPaths;
					if(ShouldLoadEditor)
					{
						LocalizationPaths += YPaths::GetEditorLocalizationPaths();
					}
					if(ShouldLoadGame)
					{
						LocalizationPaths += YPaths::GetGameLocalizationPaths();
					}
					LocalizationPaths += YPaths::GetEngineLocalizationPaths();

					// Validate the locale has data or fallback to one that does.
					TArray< FCultureRef > AvailableCultures;
					I18N.GetCulturesWithAvailableLocalization(LocalizationPaths, AvailableCultures, false);

					YString ValidCultureName;
					const TArray<YString> PrioritizedCultureNames = I18N.GetPrioritizedCultureNames(TargetCultureName);
					for (const YString& CultureName : PrioritizedCultureNames)
					{
						const bool bIsValidCulture = AvailableCultures.ContainsByPredicate([&](const FCultureRef& PotentialCulture) -> bool
						{
							return PotentialCulture->GetName() == CultureName;
						});

						if (bIsValidCulture)
						{
							ValidCultureName = CultureName;
							break;
						}
					}

					if(!ValidCultureName.IsEmpty())
					{
						if(RequestedCultureName != ValidCultureName)
						{
							// Make the user aware that the localization data belongs to a parent culture.
							UE_LOG(LogTextLocalizationManager, Log, TEXT("No specific translations for ('%s') exist, so ('%s') translations will be used."), *RequestedCultureName, *ValidCultureName);
						}
					}
					else
					{
						// Fallback to English.
						UE_LOG(LogTextLocalizationManager, Log, TEXT("No translations for ('%s') exist, falling back to 'en' for localization and internationalization data."), *RequestedCultureName);
						TargetCultureName = TEXT("en");
					}
				}

				I18N.SetCurrentCulture(TargetCultureName);
	}

	FTextLocalizationManager::Get().LoadLocalizationResourcesForCulture(I18N.GetCurrentCulture()->GetName(), ShouldLoadEditor, ShouldLoadGame);
	FTextLocalizationManager::Get().bIsInitialized = true;
}

void FTextLocalizationManager::FLocalizationEntryTracker::LoadFromDirectory(const YString& DirectoryPath)
{
	// Find resources in the specified folder.
	TArray<YString> ResourceFileNames;
	IFileManager::Get().FindFiles(ResourceFileNames, *(DirectoryPath / TEXT("*.locres")), true, false);

	for (const YString& ResourceFileName : ResourceFileNames)
	{
		LoadFromFile( YPaths::ConvertRelativePathToFull(DirectoryPath / ResourceFileName) );
	}
}

bool FTextLocalizationManager::FLocalizationEntryTracker::LoadFromFile(const YString& FilePath)
{
	TUniquePtr<FArchive> Reader(IFileManager::Get().CreateFileReader( *FilePath ));
	if( !Reader )
	{
		return false;
	}

	Reader->SetForceUnicode(true);

	LoadFromLocalizationResource(*Reader, FilePath);

	bool Success = Reader->Close();

	return Success;
}

void FTextLocalizationManager::FLocalizationEntryTracker::LoadFromLocalizationResource(FArchive& Archive, const YString& LocalizationResourceIdentifier)
{
	// Read namespace count
	uint32 NamespaceCount;
	Archive << NamespaceCount;

	for(uint32 i = 0; i < NamespaceCount; ++i)
	{
		// Read namespace
		YString Namespace;
		Archive << Namespace;

		// Read key count
		uint32 KeyCount;
		Archive << KeyCount;

		FLocalizationEntryTracker::FKeysTable& KeyTable = Namespaces.FindOrAdd(*Namespace);

		for(uint32 j = 0; j < KeyCount; ++j)
		{
			// Read key
			YString Key;
			Archive << Key;

			FLocalizationEntryTracker::FEntryArray& EntryArray = KeyTable.FindOrAdd( *Key );

			FLocalizationEntryTracker::FEntry NewEntry;
			NewEntry.LocResID = LocalizationResourceIdentifier;

			// Read string entry.
			Archive << NewEntry.SourceStringHash;
			Archive << NewEntry.LocalizedString;

			EntryArray.Add(NewEntry);
		}
	}
}

void FTextLocalizationManager::FLocalizationEntryTracker::DetectAndLogConflicts() const
{
	for (const auto& NamespaceEntry : Namespaces)
	{
		const YString& NamespaceName = NamespaceEntry.Key;
		const FKeysTable& KeyTable = NamespaceEntry.Value;
		for (const auto& KeyEntry : KeyTable)
		{
			const YString& KeFName = KeyEntry.Key;
			const FEntryArray& EntryArray = KeyEntry.Value;

			bool WasConflictDetected = false;
			for(int32 k = 0; k < EntryArray.Num(); ++k)
			{
				const FEntry& LeftEntry = EntryArray[k];
				for(int32 l = k + 1; l < EntryArray.Num(); ++l)
				{
					const FEntry& RightEntry = EntryArray[l];
					const bool bDoesSourceStringHashDiffer = LeftEntry.SourceStringHash != RightEntry.SourceStringHash;
					const bool bDoesLocalizedStringDiffer = LeftEntry.LocalizedString != RightEntry.LocalizedString;
					WasConflictDetected = bDoesSourceStringHashDiffer || bDoesLocalizedStringDiffer;
				}
			}

			if(WasConflictDetected)
			{
				YString CollidingEntryListString;
				for(int32 k = 0; k < EntryArray.Num(); ++k)
				{
					const FEntry& Entry = EntryArray[k];

					if( !(CollidingEntryListString.IsEmpty()) )
					{
						CollidingEntryListString += TEXT('\n');
					}

					CollidingEntryListString += YString::Printf( TEXT("Localization Resource: (%s) Source String Hash: (%d) Localized String: (%s)"), *(Entry.LocResID), Entry.SourceStringHash, *(Entry.LocalizedString) );
				}

				UE_LOG(LogTextLocalizationManager, Warning, TEXT("Loaded localization resources contain conflicting entries for (Namespace:%s, Key:%s):\n%s"), *NamespaceName, *KeFName, *CollidingEntryListString);
			}
		}
	}
}

void FTextLocalizationManager::FDisplayStringLookupTable::Find(const YString& InNamespace, FKeysTable*& OutKeysTableForNamespace, const YString& InKey, FDisplayStringEntry*& OutDisplayStringEntry)
{
	// Find namespace's key table.
	OutKeysTableForNamespace = NamespacesTable.Find( InNamespace );

	// Find key table's entry.
	OutDisplayStringEntry = OutKeysTableForNamespace ? OutKeysTableForNamespace->Find( InKey ) : nullptr;
}

void FTextLocalizationManager::FDisplayStringLookupTable::Find(const YString& InNamespace, const FKeysTable*& OutKeysTableForNamespace, const YString& InKey, const FDisplayStringEntry*& OutDisplayStringEntry) const
{
	// Find namespace's key table.
	OutKeysTableForNamespace = NamespacesTable.Find( InNamespace );

	// Find key table's entry.
	OutDisplayStringEntry = OutKeysTableForNamespace ? OutKeysTableForNamespace->Find( InKey ) : nullptr;
}

FTextLocalizationManager& FTextLocalizationManager::Get()
{
	static FTextLocalizationManager* GTextLocalizationManager = nullptr;
	if( !GTextLocalizationManager )
	{
		GTextLocalizationManager = new FTextLocalizationManager;
	}

	return *GTextLocalizationManager;
}

FTextDisplayStringPtr FTextLocalizationManager::FindDisplayString( const YString& Namespace, const YString& Key, const YString* const SourceString )
{
	FScopeLock ScopeLock( &SynchronizationObject );

	const FDisplayStringLookupTable::FKeysTable* LiveKeyTable = nullptr;
	const FDisplayStringLookupTable::FDisplayStringEntry* LiveEntry = nullptr;

	DisplayStringLookupTable.Find(Namespace, LiveKeyTable, Key, LiveEntry);

	if ( LiveEntry != nullptr && ( !SourceString || LiveEntry->SourceStringHash == FCrc::StrCrc32(**SourceString) ) )
	{
		return LiveEntry->DisplayString;
	}

	return nullptr;
}

FTextDisplayStringRef FTextLocalizationManager::GetDisplayString(const YString& Namespace, const YString& Key, const YString* const SourceString)
{
	FScopeLock ScopeLock( &SynchronizationObject );

	// Hack fix for old assets that don't have namespace/key info.
	if(Namespace.IsEmpty() && Key.IsEmpty())
	{
		return MakeShareable( new YString( SourceString ? **SourceString : TEXT("") ) );
	}

#if ENABLE_LOC_TESTING
	const bool bShouldLEETIFYAll = bIsInitialized && FInternationalization::Get().GetCurrentCulture()->GetName() == TEXT("LEET");

	// Attempt to set bShouldLEETIFYUnlocalizedString appropriately, only once, after the commandline is initialized and parsed.
	static bool bShouldLEETIFYUnlocalizedString = false;
	{
		static bool bHasParsedCommandLine = false;
		if(!bHasParsedCommandLine && FCommandLine::IsInitialized())
		{
			bShouldLEETIFYUnlocalizedString = FParse::Param(FCommandLine::Get(), TEXT("LEETIFYUnlocalized"));
			bHasParsedCommandLine = true;
		}
	}
#endif

	FDisplayStringLookupTable::FKeysTable* LiveKeyTable = nullptr;
	FDisplayStringLookupTable::FDisplayStringEntry* LiveEntry = nullptr;
	DisplayStringLookupTable.Find(Namespace, LiveKeyTable, Key, LiveEntry);

	// Entry is present.
	if( LiveEntry )
	{
		// If we're in some sort of development setting, source may have changed but localization resources have not, in which case the source should be used.
		const uint32 SourceStringHash = SourceString ? FCrc::StrCrc32(**SourceString) : 0;

		// If the source string (hash) is different, the local source has changed and should override - can't be localized.
		if( SourceStringHash != 0 && SourceStringHash != LiveEntry->SourceStringHash )
		{
			LiveEntry->SourceStringHash = SourceStringHash;
			LiveEntry->DisplayString.Get() = SourceString ? **SourceString : TEXT("");
			DirtyLocalRevisionForDisplayString(LiveEntry->DisplayString);

#if ENABLE_LOC_TESTING
			if( (bShouldLEETIFYAll || bShouldLEETIFYUnlocalizedString) && SourceString )
			{
				FInternationalization::Leetify(*LiveEntry->DisplayString);
				if(*LiveEntry->DisplayString == *SourceString)
				{
					UE_LOG(LogTextLocalizationManager, Warning, TEXT("Leetify failed to alter a string (%s)."), **SourceString );
				}
			}
#endif

			UE_LOG(LogTextLocalizationManager, Verbose, TEXT("An attempt was made to get a localized string (Namespace:%s, Key:%s), but the source string hash does not match - the source string (%s) will be used."), *Namespace, *Key, **LiveEntry->DisplayString);

#if ENABLE_LOC_TESTING
			LiveEntry->bIsLocalized = bShouldLEETIFYAll;
#else
			LiveEntry->bIsLocalized = false;
#endif
		}

		return LiveEntry->DisplayString;
	}
	// Entry is absent.
	else
	{
		// Don't log warnings about unlocalized strings if the system hasn't been initialized - we simply don't have localization data yet.
		if( bIsInitialized )
		{
			UE_LOG(LogTextLocalizationManager, Verbose, TEXT("An attempt was made to get a localized string (Namespace:%s, Key:%s, Source:%s), but it did not exist."), *Namespace, *Key, SourceString ? **SourceString : TEXT(""));
		}

		const FTextDisplayStringRef UnlocalizedString = MakeShareable( new YString( SourceString ? **SourceString : TEXT("") ) );

#if ENABLE_LOC_TESTING
		if( (bShouldLEETIFYAll || bShouldLEETIFYUnlocalizedString) && SourceString )
		{
			FInternationalization::Leetify(*UnlocalizedString);
			if(*UnlocalizedString == *SourceString)
			{
				UE_LOG(LogTextLocalizationManager, Warning, TEXT("Leetify failed to alter a string (%s)."), **SourceString );
			}
		}
#endif

		if ( UnlocalizedString->IsEmpty() )
		{
			if ( !bIsInitialized )
			{
				*(UnlocalizedString) = AccessedStringBeforeLocLoadedErrorMsg;
			}
		}

		// Make entries so that they can be updated when system is initialized or a culture swap occurs.
		FDisplayStringLookupTable::FDisplayStringEntry NewEntry(
#if ENABLE_LOC_TESTING
			bShouldLEETIFYAll						/*bIsLocalized*/
#else
			false												/*bIsLocalized*/
#endif
			, TEXT("")
			, SourceString ? FCrc::StrCrc32(**SourceString) : 0	/*SourceStringHash*/
			, UnlocalizedString									/*String*/
			);

		if( !LiveKeyTable )
		{
			LiveKeyTable = &(DisplayStringLookupTable.NamespacesTable.Add( Namespace, FDisplayStringLookupTable::FKeysTable() ));
		}

		LiveKeyTable->Add( Key, NewEntry );

		NamespaceKeyLookupTable.Add(NewEntry.DisplayString, FNamespaceKeyEntry(Namespace, Key));


		return UnlocalizedString;
	}
}

bool FTextLocalizationManager::GetLocResID(const YString& Namespace, const YString& Key, YString& OutLocResId)
{
	FScopeLock ScopeLock(&SynchronizationObject);

	const FDisplayStringLookupTable::FKeysTable* LiveKeyTable = nullptr;
	const FDisplayStringLookupTable::FDisplayStringEntry* LiveEntry = nullptr;
	DisplayStringLookupTable.Find(Namespace, LiveKeyTable, Key, LiveEntry);

	if (LiveEntry != nullptr && !LiveEntry->LocResID.IsEmpty())
	{
		OutLocResId = LiveEntry->LocResID;
		return true;
	}

	return false;
}

bool FTextLocalizationManager::FindNamespaceAndKeyFromDisplayString(const FTextDisplayStringRef& InDisplayString, YString& OutNamespace, YString& OutKey)
{
	FScopeLock ScopeLock( &SynchronizationObject );

	FNamespaceKeyEntry* NamespaceKeyEntry = NamespaceKeyLookupTable.Find(InDisplayString);

	if (NamespaceKeyEntry)
	{
		OutNamespace = NamespaceKeyEntry->Namespace;
		OutKey = NamespaceKeyEntry->Key;
	}

	return NamespaceKeyEntry != nullptr;
}

uint16 FTextLocalizationManager::GetLocalRevisionForDisplayString(const FTextDisplayStringRef& InDisplayString)
{
	FScopeLock ScopeLock( &SynchronizationObject );

	uint16* FoundLocalRevision = LocalTextRevisions.Find(InDisplayString);
	return (FoundLocalRevision) ? *FoundLocalRevision : 0;
}

bool FTextLocalizationManager::AddDisplayString(const FTextDisplayStringRef& DisplayString, const YString& Namespace, const YString& Key)
{
	FScopeLock ScopeLock( &SynchronizationObject );

	// Try to find existing entries.
	FNamespaceKeyEntry* ReverseLiveTableEntry = NamespaceKeyLookupTable.Find(DisplayString);
	FDisplayStringLookupTable::FKeysTable* KeysTableForExistingNamespace = nullptr;
	FDisplayStringLookupTable::FDisplayStringEntry* ExistingDisplayStringEntry = nullptr;
	DisplayStringLookupTable.Find(Namespace, KeysTableForExistingNamespace, Key, ExistingDisplayStringEntry);

	// If there are any existing entries, they may cause a conflict, unless they're exactly the same as what we would be adding.
	if ( (ExistingDisplayStringEntry && ExistingDisplayStringEntry->DisplayString != DisplayString) || // Namespace and key mustn't be associated with a different display string.
		(ReverseLiveTableEntry && (ReverseLiveTableEntry->Namespace != Namespace || ReverseLiveTableEntry->Key != Key)) ) // Display string mustn't be associated with a different namespace and key.
	{
		return false;
	}

	// Add the necessary associations in both directions.
	FDisplayStringLookupTable::FKeysTable& KeysTableForNamespace = DisplayStringLookupTable.NamespacesTable.FindOrAdd(Namespace);
	KeysTableForNamespace.Add(Key, FDisplayStringLookupTable::FDisplayStringEntry(false, TEXT(""), FCrc::StrCrc32(*DisplayString.Get()), DisplayString));
	NamespaceKeyLookupTable.Add(DisplayString, FNamespaceKeyEntry(Namespace, Key));

	return true;
}

bool FTextLocalizationManager::UpdateDisplayString(const FTextDisplayStringRef& DisplayString, const YString& Value, const YString& Namespace, const YString& Key)
{
	FScopeLock ScopeLock( &SynchronizationObject );

	// Get entry from reverse live table. Contains current namespace and key values.
	FNamespaceKeyEntry& ReverseLiveTableEntry = NamespaceKeyLookupTable[DisplayString];

	// Copy old live table entry over as new live table entry and destroy old live table entry if the namespace or key has changed.
	if (ReverseLiveTableEntry.Namespace != Namespace || ReverseLiveTableEntry.Key != Key)
	{
		FDisplayStringLookupTable::FKeysTable& KeysTableForNewNamespace = DisplayStringLookupTable.NamespacesTable.FindOrAdd(Namespace);
		FDisplayStringLookupTable::FDisplayStringEntry* NewDisplayStringEntry = KeysTableForNewNamespace.Find(Key);
		if (NewDisplayStringEntry)
		{
			// Can not update, that namespace and key combination is already in use by another string.
			return false;
		}
		else
		{
			// Get old namespace's keys table and old live table entry under old key.
			FDisplayStringLookupTable::FKeysTable* KeysTableForOldNamespace = nullptr;
			FDisplayStringLookupTable::FDisplayStringEntry* OldDisplayStringEntry = nullptr;
			DisplayStringLookupTable.Find(ReverseLiveTableEntry.Namespace, KeysTableForOldNamespace, ReverseLiveTableEntry.Key, OldDisplayStringEntry);

			// Copy old live table entry to new key in the new namespace's key table.
			check(OldDisplayStringEntry);
			KeysTableForNewNamespace.Add(Key, *OldDisplayStringEntry);

			// Remove old live table entry and old key in the old namespace's key table.
			check(KeysTableForOldNamespace);
			KeysTableForOldNamespace->Remove(ReverseLiveTableEntry.Key);

			// Remove old namespace if empty.
			if(DisplayStringLookupTable.NamespacesTable[ReverseLiveTableEntry.Namespace].Num() == 0)
			{
				DisplayStringLookupTable.NamespacesTable.Remove(ReverseLiveTableEntry.Namespace);
			}
		}
	}

	// Update display string value.
	*DisplayString = Value;
	DirtyLocalRevisionForDisplayString(DisplayString);

	// Update entry from reverse live table.
	ReverseLiveTableEntry.Namespace = Namespace;
	ReverseLiveTableEntry.Key = Key;

	return true;
}

void FTextLocalizationManager::UpdateFromLocalizationResource(const YString& LocalizationResourceFilePath)
{
	TArray<FLocalizationEntryTracker> LocalizationEntryTrackers;

	FLocalizationEntryTracker& LocalizationEntryTracker = LocalizationEntryTrackers[LocalizationEntryTrackers.Add(FLocalizationEntryTracker())];
	LocalizationEntryTracker.LoadFromFile(LocalizationResourceFilePath);
	LocalizationEntryTracker.DetectAndLogConflicts();

	UpdateFromLocalizations(LocalizationEntryTrackers);
}

void FTextLocalizationManager::UpdateFromLocalizationResource(FArchive& LocResArchive, const YString& LocResID)
{
	TArray<FLocalizationEntryTracker> LocalizationEntryTrackers;

	FLocalizationEntryTracker& CultureTracker = LocalizationEntryTrackers[LocalizationEntryTrackers.Add(FLocalizationEntryTracker())];
	CultureTracker.LoadFromLocalizationResource(LocResArchive, LocResID);
	CultureTracker.DetectAndLogConflicts();

	UpdateFromLocalizations(LocalizationEntryTrackers);
}

void FTextLocalizationManager::RefreshResources()
{
	const bool ShouldLoadEditor = WITH_EDITOR;
	const bool ShouldLoadGame = FApp::IsGame();
	LoadLocalizationResourcesForCulture(FInternationalization::Get().GetCurrentCulture()->GetName(), ShouldLoadEditor, ShouldLoadGame);
}

void FTextLocalizationManager::OnCultureChanged()
{
	const bool ShouldLoadEditor = bIsInitialized && WITH_EDITOR;
	const bool ShouldLoadGame = bIsInitialized && FApp::IsGame();
	LoadLocalizationResourcesForCulture(FInternationalization::Get().GetCurrentCulture()->GetName(), ShouldLoadEditor, ShouldLoadGame);
}

void FTextLocalizationManager::LoadLocalizationResourcesForCulture(const YString& CultureName, const bool ShouldLoadEditor, const bool ShouldLoadGame)
{
#if ENABLE_LOC_TESTING
	{
		// The leet culture is fake. Just leet-ify existing strings.
		if(CultureName == TEXT("LEET"))
		{
			for(auto NamespaceIterator = DisplayStringLookupTable.NamespacesTable.CreateIterator(); NamespaceIterator; ++NamespaceIterator)
			{
				const YString& Namespace = NamespaceIterator.Key();
				FDisplayStringLookupTable::FKeysTable& LiveKeyTable = NamespaceIterator.Value();
				for(auto KeyIterator = LiveKeyTable.CreateIterator(); KeyIterator; ++KeyIterator)
				{
					const YString& Key = KeyIterator.Key();
					FDisplayStringLookupTable::FDisplayStringEntry& LiveStringEntry = KeyIterator.Value();
					LiveStringEntry.bIsLocalized = true;
					FInternationalization::Leetify( *LiveStringEntry.DisplayString );
				}
			}

			// Early-out, there can be no localization resources to load for the fake leet culture.
			DirtyTextRevision();
			return;
		}
	}
#endif

	const FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);

	// Can't load localization resources for a culture that doesn't exist, early-out.
	if (!Culture.IsValid())
	{
		return;
	}

	// Collect the localization paths to load from.
	TArray<YString> GameLocalizationPaths;
	if(ShouldLoadGame || GIsEditor)
	{
		GameLocalizationPaths += YPaths::GetGameLocalizationPaths();
	}
	TArray<YString> EditorLocalizationPaths;
	if(ShouldLoadEditor)
	{
		EditorLocalizationPaths += YPaths::GetEditorLocalizationPaths();
		EditorLocalizationPaths += YPaths::GetToolTipLocalizationPaths();

		bool bShouldLoadLocalizedPropertFNames = true;
		if( !GConfig->GetBool( TEXT("Internationalization"), TEXT("ShouldLoadLocalizedPropertFNames"), bShouldLoadLocalizedPropertFNames, GEditorSettingsIni ) )
		{
			GConfig->GetBool( TEXT("Internationalization"), TEXT("ShouldLoadLocalizedPropertFNames"), bShouldLoadLocalizedPropertFNames, GEngineIni );
		}
		if(bShouldLoadLocalizedPropertFNames)
		{
			EditorLocalizationPaths += YPaths::GetPropertFNameLocalizationPaths();
		}
	}
	TArray<YString> EngineLocalizationPaths;
	EngineLocalizationPaths += YPaths::GetEngineLocalizationPaths();

	// Gather any additional paths that are unknown to the UE4 core (such as plugins)
	TArray<YString> AdditionalLocalizationPaths;
	GatherAdditionalLocResPathsCallback.Broadcast(AdditionalLocalizationPaths);

	// Prioritized array of localization entry trackers.
	TArray<FLocalizationEntryTracker> LocalizationEntryTrackers;

	const auto MapCulturesToDirectories = [](const YString& LocalizationPath) -> TMap<YString, YString>
	{
		TMap<YString, YString> CultureToDirectoryMap;
		IFileManager& FileManager = IFileManager::Get();

		/* Visitor class used to enumerate directories of culture. */
		class FCultureDirectoryMapperVistor : public IPlatformFile::FDirectoryVisitor
		{
		public:
			FCultureDirectoryMapperVistor( TMap<YString, YString>& OutCultureToDirectoryMap )
				: CultureToDirectoryMap(OutCultureToDirectoryMap)
			{
			}

			virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
			{
				if(bIsDirectory)
				{
					// UE localization resource folders use "en-US" style while ICU uses "en_US".
					const YString LocalizationFolder = YPaths::GetCleanFilename(FilenameOrDirectory);
					const YString CanonicalName = FCulture::GetCanonicalName(LocalizationFolder);
					CultureToDirectoryMap.Add(CanonicalName, LocalizationFolder);
				}

				return true;
			}

			/** Array to fill with the names of the UE localization folders available at the given path. */
			TMap<YString, YString>& CultureToDirectoryMap;
		};

		FCultureDirectoryMapperVistor CultureEnumeratorVistor(CultureToDirectoryMap);
		FileManager.IterateDirectory(*LocalizationPath, CultureEnumeratorVistor);

		return CultureToDirectoryMap;
	};

	TMap< YString, TMap<YString, YString> > LocalizationPathToCultureDirectoryMap;
	for (const YString& LocalizationPath : GameLocalizationPaths)
	{
		LocalizationPathToCultureDirectoryMap.Add(LocalizationPath, MapCulturesToDirectories(LocalizationPath));
	}
	for (const YString& LocalizationPath : EditorLocalizationPaths)
	{
		LocalizationPathToCultureDirectoryMap.Add(LocalizationPath, MapCulturesToDirectories(LocalizationPath));
	}
	for (const YString& LocalizationPath : EngineLocalizationPaths)
	{
		LocalizationPathToCultureDirectoryMap.Add(LocalizationPath, MapCulturesToDirectories(LocalizationPath));
	}
	for (const YString& LocalizationPath : AdditionalLocalizationPaths)
	{
		LocalizationPathToCultureDirectoryMap.Add(LocalizationPath, MapCulturesToDirectories(LocalizationPath));
	}

	TArray<YString> PrioritizedLocalizationPaths;
	if (!GIsEditor)
	{
		PrioritizedLocalizationPaths += GameLocalizationPaths;
	}
	PrioritizedLocalizationPaths += EditorLocalizationPaths;
	PrioritizedLocalizationPaths += EngineLocalizationPaths;
	PrioritizedLocalizationPaths += AdditionalLocalizationPaths;

	// The editor cheats and loads the native culture's localizations.
	if (GIsEditor)
	{
		YString NativeCultureName = TEXT("");
		GConfig->GetString( TEXT("Internationalization"), TEXT("NativeGameCulture"), NativeCultureName, GEditorSettingsIni );

		if (!NativeCultureName.IsEmpty())
		{
			FLocalizationEntryTracker& CultureTracker = LocalizationEntryTrackers[LocalizationEntryTrackers.Add(FLocalizationEntryTracker())];
			for (const YString& LocalizationPath : GameLocalizationPaths)
			{
				const YString* const Entry = LocalizationPathToCultureDirectoryMap[LocalizationPath].Find(FCulture::GetCanonicalName(NativeCultureName));
				if (Entry)
				{
					const YString CulturePath = LocalizationPath / (*Entry);

					CultureTracker.LoadFromDirectory(CulturePath);
				}
			}
			CultureTracker.DetectAndLogConflicts();
		}
	}

	// Read culture localization resources.
	const TArray<YString> PrioritizedCultureNames = FInternationalization::Get().GetPrioritizedCultureNames(CultureName);
	for (const YString& ParentCultureName : PrioritizedCultureNames)
	{
		FLocalizationEntryTracker& CultureTracker = LocalizationEntryTrackers[LocalizationEntryTrackers.Add(FLocalizationEntryTracker())];
		for (const YString& LocalizationPath : PrioritizedLocalizationPaths)
		{
			const YString* const Entry = LocalizationPathToCultureDirectoryMap[LocalizationPath].Find(FCulture::GetCanonicalName(ParentCultureName));
			if (Entry)
			{
				const YString CulturePath = LocalizationPath / (*Entry);

				CultureTracker.LoadFromDirectory(CulturePath);
			}
		}
		CultureTracker.DetectAndLogConflicts();
	}

	// Replace localizations with those of the loaded localization resources.
	UpdateFromLocalizations(LocalizationEntryTrackers);
}

void FTextLocalizationManager::UpdateFromLocalizations(const TArray<FLocalizationEntryTracker>& LocalizationEntryTrackers)
{
	// Update existing localized entries/flag existing newly-unlocalized entries.
	for (auto& Namespace : DisplayStringLookupTable.NamespacesTable)
	{
		const YString& NamespaceName = Namespace.Key;
		FDisplayStringLookupTable::FKeysTable& LiveKeyTable = Namespace.Value;
		for(auto& Key : LiveKeyTable)
		{
			const YString& KeFName = Key.Key;
			FDisplayStringLookupTable::FDisplayStringEntry& LiveStringEntry = Key.Value;

			const FLocalizationEntryTracker::FEntry* SourceEntryForUpdate = nullptr;

			// Attempt to use resources in prioritized order until we find an entry.
			for(const FLocalizationEntryTracker& Tracker : LocalizationEntryTrackers)
			{
				const FLocalizationEntryTracker::FKeysTable* const UpdateKeyTable = Tracker.Namespaces.Find(NamespaceName);
				const FLocalizationEntryTracker::FEntryArray* const UpdateEntryArray = UpdateKeyTable ? UpdateKeyTable->Find(KeFName) : nullptr;
				const FLocalizationEntryTracker::FEntry* Entry = UpdateEntryArray && UpdateEntryArray->Num() ? &((*UpdateEntryArray)[0]) : nullptr;
				if(Entry)
				{
					SourceEntryForUpdate = Entry;
					break;
				}
			}

			// If the source string hashes are are the same, we can replace the display string.
			// Otherwise, it would suggest the source string has changed and the new localization may be based off of an old source string.
			if (SourceEntryForUpdate && LiveStringEntry.SourceStringHash == SourceEntryForUpdate->SourceStringHash)
			{
				LiveStringEntry.bIsLocalized = true;
				*(LiveStringEntry.DisplayString) = SourceEntryForUpdate->LocalizedString;
			}
			else
			{
				if ( !LiveStringEntry.bIsLocalized && LiveStringEntry.DisplayString.Get() == AccessedStringBeforeLocLoadedErrorMsg )
				{
					*(LiveStringEntry.DisplayString) = TEXT("");
				}

				LiveStringEntry.bIsLocalized = false;

#if ENABLE_LOC_TESTING
				const bool bShouldLEETIFYUnlocalizedString = FParse::Param(FCommandLine::Get(), TEXT("LEETIFYUnlocalized"));
				if(bShouldLEETIFYUnlocalizedString )
				{
					FInternationalization::Leetify(*(LiveStringEntry.DisplayString));
				}
#endif
			}
		}
	}

	// Add new entries. 
	for(const auto& Tracker : LocalizationEntryTrackers)
	{
		for(const auto& Namespace : Tracker.Namespaces)
		{
			const YString& NamespaceName = Namespace.Key;
			const FLocalizationEntryTracker::FKeysTable& NewKeyTable = Namespace.Value;
			for(const auto& Key : NewKeyTable)
			{
				const YString& KeFName = Key.Key;
				const FLocalizationEntryTracker::FEntryArray& NewEntryArray = Key.Value;
				const FLocalizationEntryTracker::FEntry& NewEntry = NewEntryArray[0];

				FDisplayStringLookupTable::FKeysTable& LiveKeyTable = DisplayStringLookupTable.NamespacesTable.FindOrAdd(NamespaceName);
				FDisplayStringLookupTable::FDisplayStringEntry* const LiveStringEntry = LiveKeyTable.Find(KeFName);
				// Note: Anything we find in the table has already been updated above.
				if( !LiveStringEntry )
				{
					FDisplayStringLookupTable::FDisplayStringEntry NewLiveEntry(
						true,													/*bIsLocalized*/
						NewEntry.LocResID,
						NewEntry.SourceStringHash,								/*SourceStringHash*/
						MakeShareable( new YString(NewEntry.LocalizedString) )	/*String*/
						);
					LiveKeyTable.Add( KeFName, NewLiveEntry );

					NamespaceKeyLookupTable.Add(NewLiveEntry.DisplayString, FNamespaceKeyEntry(NamespaceName, KeFName));
				}
			}
		}
	}

	DirtyTextRevision();
}

void FTextLocalizationManager::DirtyLocalRevisionForDisplayString(const FTextDisplayStringRef& InDisplayString)
{
	uint16* FoundLocalRevision = LocalTextRevisions.Find(InDisplayString);
	if (FoundLocalRevision)
	{
		while (++(*FoundLocalRevision) == 0) {} // Zero is special, don't allow an overflow to stay at zero
	}
	else
	{
		LocalTextRevisions.Add(InDisplayString, 1);
	}
}

void FTextLocalizationManager::DirtyTextRevision()
{
	while (++TextRevisionCounter == 0) {} // Zero is special, don't allow an overflow to stay at zero
	LocalTextRevisions.Empty();
	OnTextRevisionChangedEvent.Broadcast();
}
