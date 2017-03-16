// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/SolidAngleTemplate.h"
#include "Misc/Crc.h"
#include "Containers/SolidAngleString.h"
#include "Containers/Set.h"
#include "SObject/NameTypes.h"
#include "Misc/Guid.h"

struct ECustomVersionSerializationFormat
{
	enum Type
	{
		Unknown,
		Guids,
		Enums,
		Optimized,

		// Add new versions above this comment
		CustomVersion_Automatic_Plus_One,
		Latest = CustomVersion_Automatic_Plus_One - 1
	};
};


/**
 * Structure to hold unique custom key with its version.
 */
struct CORE_API FCustomVersion
{
	friend class FCustomVersionContainer;

	/** Unique custom key. */
	YGuid Key;

	/** Custom version. */
	int32 Version;

	/** Number of times this GUID has been registered */
	int32 ReferenceCount;

	/** Constructor. */
	FORCEINLINE FCustomVersion()
	{
	}

	/** Helper constructor. */
	FORCEINLINE FCustomVersion(YGuid InKey, int32 InVersion, YName InFriendlyName)
	: Key           (InKey)
	, Version       (InVersion)
	, ReferenceCount(1)
	, FriendlyName  (InFriendlyName)
	{
	}

	/** Equality comparison operator for Key */
	FORCEINLINE bool operator==(YGuid InKey) const
	{
		return Key == InKey;
	}

	/** Inequality comparison operator for Key */
	FORCEINLINE bool operator!=(YGuid InKey) const
	{
		return Key != InKey;
	}

	CORE_API friend YArchive& operator<<(YArchive& Ar, FCustomVersion& Version);

	/** Gets the friendly name for error messages or whatever */
	const YName GetFriendlyName() const;

private:

	/** Friendly name for error messages or whatever. Lazy initialized for serialized versions */
	mutable YName FriendlyName;
};

struct FCustomVersionKeyFuncs : BaseKeyFuncs<FCustomVersion, YGuid, false>
{
	static FORCEINLINE const YGuid& GetSetKey(const FCustomVersion& Element)
	{
		return Element.Key;
	}
	static FORCEINLINE bool Matches(YGuid A, YGuid B)
	{
		return A == B;
	}
	static FORCEINLINE uint32 GetKeyHash(YGuid Key)
	{
		return FCrc::MemCrc_DEPRECATED(&Key, sizeof(YGuid));
	}
};

typedef TSet<FCustomVersion, FCustomVersionKeyFuncs> FCustomVersionSet;

class CORE_API FCustomVersionRegistration;


/**
 * Container for all available/serialized custom versions.
 */
class CORE_API FCustomVersionContainer
{
	friend class FCustomVersionRegistration;

public:
	/** Gets available custom versions in this container. */
	FORCEINLINE const FCustomVersionSet& GetAllVersions() const
	{
		return Versions;
	}

	/**
	 * Gets a custom version from the container.
	 *
	 * @param CustomKey Custom key for which to retrieve the version.
	 * @return The FCustomVersion for the specified custom key, or nullptr if the key doesn't exist in the container.
	 */
	const FCustomVersion* GetVersion(YGuid CustomKey) const;

	/**
	* Gets a custom version friendly name from the container.
	*
	* @param CustomKey Custom key for which to retrieve the version.
	* @return The friendly name for the specified custom key, or NAME_None if the key doesn't exist in the container.
	*/
	const YName GetFriendlyName(YGuid CustomKey) const;

	/**
	 * Sets a specific custom version in the container.
	 *
	 * @param CustomKey Custom key for which to retrieve the version.
	 * @param Version The version number for the specified custom key
	 * @param FriendlyName A friendly name to assign to this version
	 */
	void SetVersion(YGuid CustomKey, int32 Version, YName FriendlyName);

	/** Serialization. */
	void Serialize(YArchive& Ar, ECustomVersionSerializationFormat::Type Format = ECustomVersionSerializationFormat::Latest);

	/**
	 * Gets a singleton with the registered versions.
	 *
	 * @return The registered version container.
	 */
	static const FCustomVersionContainer& GetRegistered();

	/**
	 * Empties the custom version container.
	 */
	void Empty();

	/** Return a string representation of custom versions. Used for debug. */
	YString ToString(const YString& Indent) const;

private:

	/** Private implementation getter */
	static FCustomVersionContainer& GetInstance();

	/** Array containing custom versions. */
	FCustomVersionSet Versions;
};


/**
 * This class will cause the registration of a custom version number and key with the global
 * FCustomVersionContainer when instantiated, and unregister when destroyed.  It should be
 * instantiated as a global variable somewhere in the module which needs a custom version number.
 */
class FCustomVersionRegistration : FNoncopyable
{
public:

	FCustomVersionRegistration(YGuid InKey, int32 Version, YName InFriendlyName);
	~FCustomVersionRegistration();

private:

	YGuid Key;
};
