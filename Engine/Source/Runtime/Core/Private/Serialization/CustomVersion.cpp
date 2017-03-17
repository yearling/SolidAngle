// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	CustomVersion.cpp: Unreal custom versioning system.
=============================================================================*/

#include "Serialization/CustomVersion.h"
namespace
{
	FCustomVersion UnusedCustomVersion(YGuid(0, 0, 0, 0xF99D40C1), 0, TEXT("Unused custom version"));

	struct FEnumCustomVersion_DEPRECATED
	{
		uint32 Tag;
		int32  Version;

		FCustomVersion ToCustomVersion() const
		{
			// We'll invent a GUID from three zeroes and the original tag
			return FCustomVersion(YGuid(0, 0, 0, Tag), Version, *YString::Printf(TEXT("EnumTag%u"), Tag));
		}
	};

	YArchive& operator<<(YArchive& Ar, FEnumCustomVersion_DEPRECATED& Version)
	{
		// Serialize keys
		Ar << Version.Tag;
		Ar << Version.Version;

		return Ar;
	}

	struct FGuidCustomVersion_DEPRECATED
	{
		YGuid Key;
		int32 Version;
		YString FriendlyName;

		FCustomVersion ToCustomVersion() const
		{
			// We'll invent a GUID from three zeroes and the original tag
			return FCustomVersion(Key, Version, *FriendlyName);
		}
	};

	YArchive& operator<<(YArchive& Ar, FGuidCustomVersion_DEPRECATED& Version)
	{
		Ar << Version.Key;
		Ar << Version.Version;
		Ar << Version.FriendlyName;
		return Ar;
	}
}

const YName FCustomVersion::GetFriendlyName() const
{
	if (FriendlyName == NAME_None)
	{
		FriendlyName = FCustomVersionContainer::GetRegistered().GetFriendlyName(Key);
	}
	return FriendlyName;
}

const FCustomVersionContainer& FCustomVersionContainer::GetRegistered()
{
	return GetInstance();
}

void FCustomVersionContainer::Empty()
{
	Versions.Empty();
}

YString FCustomVersionContainer::ToString(const YString& Indent) const
{
	YString VersionsAsString;
	for (const FCustomVersion& SomeVersion : Versions)
	{
		VersionsAsString += Indent;
		VersionsAsString += YString::Printf(TEXT("Key=%s  Version=%d  Friendly Name=%s \n"), *SomeVersion.Key.ToString(), SomeVersion.Version, *SomeVersion.GetFriendlyName().ToString() );
	}

	return VersionsAsString;
}

FCustomVersionContainer& FCustomVersionContainer::GetInstance()
{
	static FCustomVersionContainer Singleton;

	return Singleton;
}

YArchive& operator<<(YArchive& Ar, FCustomVersion& Version)
{
	Ar << Version.Key;
	Ar << Version.Version;
	return Ar;
}

void FCustomVersionContainer::Serialize(YArchive& Ar, ECustomVersionSerializationFormat::Type Format)
{
	switch (Format)
	{
		default: check(false);

		case ECustomVersionSerializationFormat::Enums:
		{
			// We should only ever be loading enums.  They should never be saved - they only exist for backward compatibility.
			check(Ar.IsLoading());

			TArray<FEnumCustomVersion_DEPRECATED> OldTags;
			Ar << OldTags;

			Versions.Empty(OldTags.Num());
			for (auto It = OldTags.CreateConstIterator(); It; ++It)
			{
				Versions.Add(It->ToCustomVersion());
			}
		}
		break;

		case ECustomVersionSerializationFormat::Guids:
		{
			// We should only ever be loading old versions.  They should never be saved - they only exist for backward compatibility.
			check(Ar.IsLoading());

			TArray<FGuidCustomVersion_DEPRECATED> VersionArray;
			Ar << VersionArray;
			Versions.Empty(VersionArray.Num());
			for (FGuidCustomVersion_DEPRECATED& OldVer : VersionArray)
			{
				Versions.Add(OldVer.ToCustomVersion());
			}
		}
		break;

		case ECustomVersionSerializationFormat::Optimized:
		{
			Ar << Versions;
		}
		break;
	}
}

const FCustomVersion* FCustomVersionContainer::GetVersion(YGuid Key) const
{
	// A testing tag was written out to a few archives during testing so we need to
	// handle the existence of it to ensure that those archives can still be loaded.
	if (Key == UnusedCustomVersion.Key)
	{
		return &UnusedCustomVersion;
	}

	return Versions.Find(Key);
}

const YName FCustomVersionContainer::GetFriendlyName(YGuid Key) const
{
	YName FriendlyName = NAME_Name;
	const FCustomVersion* CustomVersion = GetVersion(Key);
	if (CustomVersion)
	{
		FriendlyName = CustomVersion->FriendlyName;
	}
	return FriendlyName;
}

void FCustomVersionContainer::SetVersion(YGuid CustomKey, int32 Version, YName FriendlyName)
{
	if (CustomKey == UnusedCustomVersion.Key)
		return;

	if (FCustomVersion* Found = Versions.Find(CustomKey))
	{
		Found->Version      = Version;
		Found->FriendlyName = FriendlyName;
	}
	else
	{
		Versions.Add(FCustomVersion(CustomKey, Version, FriendlyName));
	}
}

FCustomVersionRegistration::FCustomVersionRegistration(YGuid InKey, int32 InVersion, YName InFriendlyName)
{
	FCustomVersionSet& Versions = FCustomVersionContainer::GetInstance().Versions;

	// Check if this tag hasn't already been registered
	if (FCustomVersion* ExistingRegistration = Versions.Find(InKey))
	{
		// We don't allow the registration details to change across registrations - this code path only exists to support hotreload

		// If you hit this then you've probably either:
		// * Changed registration details during hotreload.
		// * Accidentally copy-and-pasted an FCustomVersionRegistration object.
		ensureMsgf(
			ExistingRegistration->Version == InVersion && ExistingRegistration->GetFriendlyName() == InFriendlyName,
			TEXT("Custom version registrations cannot change between hotreloads - \"%s\" version %d is being reregistered as \"%s\" version %d"),
			*ExistingRegistration->GetFriendlyName().ToString(),
			ExistingRegistration->Version,
			InFriendlyName,
			InVersion
		);

		++ExistingRegistration->ReferenceCount;
	}
	else
	{
		Versions.Add(FCustomVersion(InKey, InVersion, InFriendlyName));
	}

	Key = InKey;
}

FCustomVersionRegistration::~FCustomVersionRegistration()
{
	FCustomVersionSet& Versions = FCustomVersionContainer::GetInstance().Versions;

	FCustomVersion* FoundKey = Versions.Find(Key);

	// Ensure this tag has been registered
	check(FoundKey);

	--FoundKey->ReferenceCount;
	if (FoundKey->ReferenceCount == 0)
	{
		Versions.Remove(Key);
	}
}
