// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "Delegates/Delegate.h"
#include "Misc/Optional.h"
#include "Internationalization/Text.h"
#include "Templates/ValueOrError.h"

template<typename NumericType> struct YNumericUnit;

/** Enum *must* be zero-indexed and sequential. Must be grouped by relevance and ordered by magnitude. */
/** Enum *must* match the mirrored enum that exists in CoreUObject/Classes/Object.h for the purposes of UObject reflection */
enum class EUnit : uint8
{
	/** Scalar distance/length units */
	Micrometers, Millimeters, Centimeters, Meters, Kilometers,
	Inches, Feet, Yards, Miles,
	Lightyears,

	/** Angular units */
	Degrees, Radians,

	/** Speed units */
	MetersPerSecond, KilometersPerHour, MilesPerHour,

	/** Temperature units */
	Celsius, Farenheit, Kelvin,

	/** Mass units */
	Micrograms, Milligrams, Grams, Kilograms, MetricTons,
	Ounces, Pounds, Stones,

	/** Force units */
	Newtons, PoundsForce, KilogramsForce,

	/** Frequency units */
	Hertz, Kilohertz, Megahertz, Gigahertz, RevolutionsPerMinute,

	/** Data Size units */
	Bytes, Kilobytes, Megabytes, Gigabytes, Terabytes,

	/** Luminous flux units */
	Lumens,

	/** Time units */
	Milliseconds, Seconds, Minutes, Hours, Days, Months, Years,

	/** Arbitrary multiplier */
	Multiplier,

	/** Symbolic entry, not specifyable on meta data */
	Unspecified
};

/** Enumeration that specifies particular classes of unit */
enum class EUnitType
{
	Distance, Angle, Speed, Temperature, Mass, Force, Frequency, DataSize, LuminousFlux, Time, Arbitrary,

	// Symbolic entry - do not use directly
	NumberOf,
};

template<typename NumericType> struct YNumericUnit;

/** Unit settings accessed globally through FUnitConversion::Settings() */
class CORE_API YUnitSettings
{
public:

	YUnitSettings();

	/** Check whether unit display is globally enabled or disabled */
	bool ShouldDisplayUnits() const;
	void SetShouldDisplayUnits(bool bInGlobalUnitDisplay);
	
	/** Get/Set the specific valid units to display the specified type of unit in */
	const TArray<EUnit>& GetDisplayUnits(EUnitType InType) const;
	void SetDisplayUnits(EUnitType InType, const TArray<EUnit>& Units);
	void SetDisplayUnits(EUnitType InType, EUnit Units);

	/** Returns an event delegate that is executed when a display setting has changed. (GlobalUnitDisplay or DefaultInputUnits) */
	DECLARE_EVENT(YUnitSettings, FDisplaySettingChanged);
	FDisplaySettingChanged& OnDisplaySettingsChanged() { return SettingChangedEvent; }

private:

	/** Global toggle controlling whether we should display units or not */
	bool bGlobalUnitDisplay;

	/** Arrays of units that are valid to display on interfaces */
	TArray<EUnit> DisplayUnits[(uint8)EUnitType::NumberOf + 1];
	
	/** Holds an event delegate that is executed when a display setting has changed. */
	FDisplaySettingChanged SettingChangedEvent;
};

struct CORE_API YUnitConversion
{
	/** Get the global settings for unit conversion/display */
	static YUnitSettings& Settings();

	/** Check whether it is possible to convert a number between the two specified units */
	static bool AreUnitsCompatible(EUnit From, EUnit To);

	/** Check whether a unit is of the specified type */
	static bool IsUnitOfType(EUnit Unit, EUnitType Type);

	/** Get the type of the specified unit */
	static EUnitType GetUnitType(EUnit);

	/** Get the display string for the the specified unit type */
	static const TCHAR* GetUnitDisplayString(EUnit Unit);

	/** Helper function to find a unit from a string (name or display string) */
	static TOptional<EUnit> UnitFromString(const TCHAR* UnitString);

public:

	/** Convert the specified number from one unit to another. Does nothing if the units are incompatible. */
	template<typename T>
	static T Convert(T InValue, EUnit From, EUnit To);

	/** Quantizes this number to the most appropriate unit for user friendly presentation (e.g. 1000m returns 1km). */
	template<typename T>
	static YNumericUnit<T> QuantizeUnitsToBestFit(T Value, EUnit Units);

	/** Quantizes this number to the most appropriate unit for user friendly presentation (e.g. 1000m returns 1km), adhereing to global display settings. */
	template<typename T>
	static EUnit CalculateDisplayUnit(T Value, EUnit InUnits);

};


/**
 * FNumericUnit is a numeric type that wraps the templated type, whilst a specified unit.
 * It handles conversion to/from related units automatically. The units are considered not to contribute to the type's state, and as such should be considered immutable once set.
 */
template<typename NumericType>
struct YNumericUnit
{
	/** The numeric (scalar) value */
	NumericType Value;
	/** The associated units for the value. Can never change once set to anything other than EUnit::Unspecified. */
	const EUnit Units;

	/** Constructors */
	YNumericUnit();
	YNumericUnit(const NumericType& InValue, EUnit InUnits = EUnit::Unspecified);

	/** Copy construction/assignment from the same type */
	YNumericUnit(const YNumericUnit& Other);
	YNumericUnit& operator=(const YNumericUnit& Other);

	/** Templated Copy construction/assignment from differing numeric types. Relies on implicit conversion of the two numeric types. */
	template<typename OtherType> YNumericUnit(const YNumericUnit<OtherType>& Other);
	template<typename OtherType> YNumericUnit& operator=(const YNumericUnit<OtherType>& Other);

	/** Convert this quantity to a different unit */
	TOptional<YNumericUnit<NumericType>> ConvertTo(EUnit ToUnits) const;

public:

	/** Quantizes this number to the most appropriate unit for user friendly presentation (e.g. 1000m returns 1km). */
	YNumericUnit<NumericType> QuantizeUnitsToBestFit() const;

	/** Try and parse an expression into a numeric unit */
	static TValueOrError<YNumericUnit<NumericType>, FText> TryParseExpression(const TCHAR* InExpression, EUnit InDefaultUnit, const YNumericUnit<NumericType>& InExistingValue);

	/** Parse a numeric unit from a string */
	static TOptional<YNumericUnit<NumericType>> TryParseString(const TCHAR* InSource);

private:
	/** Conversion to the numeric type disabled as coupled with implicit construction from NumericType can easily lead to loss of associated units. */
	operator const NumericType&() const;

	/** Copy another unit into this one, taking account of its units, and applying necessary conversion */
	template<typename OtherType>
	void CopyValueWithConversion(const YNumericUnit<OtherType>& Other);

	/** Given a string, skip past whitespace, then any numeric characters. Set End pointer to the end of the last numeric character. */
	static bool ExtractNumberBoundary(const TCHAR* Start, const TCHAR*& End);
};

/** Global arithmetic operators for number types. Deals with conversion from related units correctly. */
template<typename NumericType, typename OtherType>
bool operator==(const YNumericUnit<NumericType>& LHS, const YNumericUnit<OtherType>& RHS);

template<typename NumericType, typename OtherType>
bool operator!=(const YNumericUnit<NumericType>& LHS, const YNumericUnit<OtherType>& RHS);

namespace Lex
{
	template<typename T>
	YString ToString(const YNumericUnit<T>& NumericUnit);

	template<typename T>
	YString ToSanitizedString(const YNumericUnit<T>& NumericUnit);

	template<typename T>
	void FromString(YNumericUnit<T>& OutValue, const TCHAR* String);
	
	template<typename T>
	bool TryParseString(YNumericUnit<T>& OutValue, const TCHAR* String);
}


// Include template definitions
#include "Math/UnitConversion.inl"
