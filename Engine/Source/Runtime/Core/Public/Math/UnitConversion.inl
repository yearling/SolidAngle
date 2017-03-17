// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/** Inline file for UnitConversion.h to separate the implementation from the header */


#include "CoreTypes.h"
#include "CoreFwd.h"

struct YMath;
struct YUnitConversion;
enum class EUnit : uint8;
enum class EUnitType;
template<typename NumericType> struct YNumericUnit;
template<typename OptionalType> struct TOptional;
template<typename ValueType, typename ErrorType> class TValueOrError;

namespace UnitConversion
{
	/** Find the common quantization factor for the specified distance unit. Quantizes to Meters. */
	CORE_API double DistanceUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified angular unit. Quantizes to Degrees. */
	CORE_API double AngleUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified speed unit. Quantizes to km/h. */
	CORE_API double SpeedUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified temperature unit. Quantizes to Kelvin. */
	CORE_API double TemperatureUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified mass unit. Quantizes to Grams. */
	CORE_API double MassUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified force unit. Quantizes to Newtons. */
	CORE_API double ForceUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified frequency unit. Quantizes to KHz. */
	CORE_API double FrequencyUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified data size unit. Quantizes to MB. */
	CORE_API double DataSizeUnificationFactor(EUnit From);
	/** Find the common quantization factor for the specified time unit. Quantizes to hours. */
	CORE_API double TimeUnificationFactor(EUnit From);

	/** Attempt to parse an expression into a numeric unit */
	CORE_API TValueOrError<YNumericUnit<double>, FText> TryParseExpression(const TCHAR* InExpression, EUnit DefaultUnit, const YNumericUnit<double>& InExistingValue);

	/** Structure used to define the factor required to get from one unit type to the next. */
	struct FQuantizationInfo
	{
		/** The unit to which this factor applies */
		EUnit Units;
		/** The factor by which to multiply to get to the next unit in this range */
		float Factor;

		/** Constructor */
		FQuantizationInfo(EUnit InUnit, float InFactor) : Units(InUnit), Factor(InFactor) {}
	};

	/** Find the quantization bounds for the specified unit, if any */
	CORE_API TOptional<const TArray<FQuantizationInfo>*> GetQuantizationBounds(EUnit Unit);

}	// namespace UnitConversion

/** Convert the specified number from one unit to another. Does nothing if the units are incompatible. */
template<typename T>
T YUnitConversion::Convert(T InValue, EUnit From, EUnit To)
{
	using namespace UnitConversion;

	if (!AreUnitsCompatible(From, To))
	{
		return InValue;
	}
	else if (From == EUnit::Unspecified || To == EUnit::Unspecified)
	{
		return InValue;
	}

	switch(YUnitConversion::GetUnitType(From))
	{
		case EUnitType::Distance:			return InValue * DistanceUnificationFactor(From)		* (1.0 / DistanceUnificationFactor(To));
		case EUnitType::Angle:				return InValue * AngleUnificationFactor(From) 			* (1.0 / AngleUnificationFactor(To));
		case EUnitType::Speed:				return InValue * SpeedUnificationFactor(From) 			* (1.0 / SpeedUnificationFactor(To));
		case EUnitType::Mass:				return InValue * MassUnificationFactor(From) 			* (1.0 / MassUnificationFactor(To));
		case EUnitType::Force:				return InValue * ForceUnificationFactor(From) 			* (1.0 / ForceUnificationFactor(To));
		case EUnitType::Frequency:			return InValue * FrequencyUnificationFactor(From) 		* (1.0 / FrequencyUnificationFactor(To));
		case EUnitType::DataSize:			return InValue * DataSizeUnificationFactor(From) 		* (1.0 / DataSizeUnificationFactor(To));
		case EUnitType::LuminousFlux:		return InValue;
		case EUnitType::Time:				return InValue * TimeUnificationFactor(From) 			* (1.0 / TimeUnificationFactor(To));			
		// Temperature conversion is not just a simple multiplication, so needs special treatment
		case EUnitType::Temperature:
		{
			double NewValue = InValue;
			// Put it into kelvin
			switch (From)
			{
				case EUnit::Celsius:			NewValue = NewValue + 273.15;					break;
				case EUnit::Farenheit:			NewValue = (NewValue + 459.67) * 5.f/9.f;		break;
				default: 																		break;
			}
			// And out again
			switch (To)
			{
				case EUnit::Celsius:			return NewValue - 273.15;
				case EUnit::Farenheit:			return NewValue * 9.f/5.f - 459.67;
				default: 						return NewValue;
			}
		}

		default:							return InValue;
	}
}

template<typename T>
YNumericUnit<T> YUnitConversion::QuantizeUnitsToBestFit(T Value, EUnit Units)
{
	auto OptionalBounds = UnitConversion::GetQuantizationBounds(Units);
	if (!OptionalBounds.IsSet())
	{
		return YNumericUnit<T>(Value, Units);
	}

	const auto& Bounds = *OptionalBounds.GetValue();

	const int32 CurrentUnitIndex = (uint8)Units - (uint8)Bounds[0].Units;

	EUnit NewUnits = Units;
	double NewValue = Value;

	if (YMath::Abs(NewValue) > 1)
	{
		// Large number? Try larger units
		for (int32 Index = CurrentUnitIndex; Index < Bounds.Num(); ++Index)
		{
			if (Bounds[Index].Factor == 0)
			{
				break;
			}

			const auto Tmp = NewValue / Bounds[Index].Factor;

			if (YMath::Abs(Tmp) < 1)
			{
				break;
			}

			NewValue = Tmp;
			NewUnits = Bounds[Index + 1].Units;
		}
	}
	else if (NewValue != 0)
	{
		// Small number? Try smaller units
		for (int32 Index = CurrentUnitIndex - 1; Index >= 0; --Index)
		{
			NewValue *= Bounds[Index].Factor;
			NewUnits = Bounds[Index].Units;

			if (YMath::Abs(NewValue) > 1)
			{
				break;
			}
		}
	}

	return YNumericUnit<T>(NewValue, NewUnits);
}

template<typename T>
EUnit YUnitConversion::CalculateDisplayUnit(T Value, EUnit InUnits)
{
	if (InUnits == EUnit::Unspecified)
	{
		return EUnit::Unspecified;
	}

	const TArray<EUnit>& DisplayUnits = Settings().GetDisplayUnits(GetUnitType(InUnits));
	if (DisplayUnits.Num() == 0)
	{
		return QuantizeUnitsToBestFit(Value, InUnits).Units;
	}
	else if (DisplayUnits.Num() == 1)
	{
		return DisplayUnits[0];
	}

	// If the value we were given was 0, change it to something we can actually work with
	if (Value == 0)
	{
		Value = 1;
	}

	int32 BestIndex = 0;
	for (int32 Index = 0; Index < DisplayUnits.Num() - 1; ++Index)
	{
		double This = Convert(Value, InUnits, DisplayUnits[Index]);
		double Next = Convert(Value, InUnits, DisplayUnits[Index + 1]);

		if (YMath::Abs(YMath::LogX(10.0f, This)) < YMath::Abs(YMath::LogX(10.0f, Next)))
		{
			BestIndex = Index;
		}
		else
		{
			BestIndex = Index + 1;
		}
	}

	return DisplayUnits[BestIndex];
}

template<typename NumericType>
YNumericUnit<NumericType>::YNumericUnit()
	: Units(EUnit::Unspecified)
{}

template<typename NumericType>
YNumericUnit<NumericType>::YNumericUnit(const NumericType& InValue, EUnit InUnits)
	: Value(InValue), Units(InUnits)
{}

template<typename NumericType>
YNumericUnit<NumericType>::YNumericUnit(const YNumericUnit& Other)
	: Units(EUnit::Unspecified)
{
	(*this) = Other;
}

template<typename NumericType>
YNumericUnit<NumericType>& YNumericUnit<NumericType>::operator=(const YNumericUnit& Other)
{
	CopyValueWithConversion(Other);
	return *this;
}

/** Templated Copy construction/assignment from differing numeric types. Relies on implicit conversion of the two numeric types. */
template<typename NumericType> template<typename OtherType>
YNumericUnit<NumericType>::YNumericUnit(const YNumericUnit<OtherType>& Other)
{
	(*this) = Other;
}

template<typename NumericType> template<typename OtherType>
YNumericUnit<NumericType>& YNumericUnit<NumericType>::operator=(const YNumericUnit<OtherType>& Other)
{
	CopyValueWithConversion(Other);
	return *this;
}

/** Convert this quantity to a different unit */
template<typename NumericType>
TOptional<YNumericUnit<NumericType>> YNumericUnit<NumericType>::ConvertTo(EUnit ToUnits) const
{
	if (Units == EUnit::Unspecified)
	{
		return YNumericUnit(Value, ToUnits);
	}
	else if (YUnitConversion::AreUnitsCompatible(Units, ToUnits))
	{
		return YNumericUnit<NumericType>(YUnitConversion::Convert(Value, Units, ToUnits), ToUnits);
	}

	return TOptional<YNumericUnit<NumericType>>();
}

template<typename NumericType>
YNumericUnit<NumericType> YNumericUnit<NumericType>::QuantizeUnitsToBestFit() const
{
	return YUnitConversion::QuantizeUnitsToBestFit(Value, Units);
}

template<typename NumericType>
TValueOrError<YNumericUnit<NumericType>, FText> YNumericUnit<NumericType>::TryParseExpression(const TCHAR* InExpression, EUnit InDefaultUnit, const YNumericUnit<NumericType>& InExistingValue)
{
	TValueOrError<YNumericUnit<double>, FText> Result = UnitConversion::TryParseExpression(InExpression, InDefaultUnit, InExistingValue);
	if (Result.IsValid())
	{
		const auto& Value = Result.GetValue();
		return MakeValue(YNumericUnit<NumericType>((NumericType)Value.Value, Value.Units));
	}

	return MakeError(Result.GetError());
}

template<typename NumericType>
TOptional<YNumericUnit<NumericType>> YNumericUnit<NumericType>::TryParseString(const TCHAR* InSource)
{
	TOptional<YNumericUnit<NumericType>> Result;
	if (!InSource || !*InSource)
	{
		return Result;
	}

	const TCHAR* NumberEnd = nullptr;
	if (!ExtractNumberBoundary(InSource, NumberEnd))
	{
		return Result;
	}

	NumericType NewValue;
	Lex::FromString(NewValue, InSource);

	// Now parse the units
	while(FChar::IsWhitespace(*NumberEnd)) ++NumberEnd;

	if (*NumberEnd == '\0')
	{
		// No units
		Result.Emplace(NewValue);
	}
	else
	{
		// If the string specifies units, they must map to something that exists for this function to succeed
		auto NewUnits = YUnitConversion::UnitFromString(NumberEnd);
		if (NewUnits)
		{
			Result.Emplace(NewValue, NewUnits.GetValue());
		}
	}

	return Result;
}

/** Copy another unit into this one, taking account of its units, and applying necessary conversion */
template<typename NumericType> template<typename OtherType>
void YNumericUnit<NumericType>::CopyValueWithConversion(const YNumericUnit<OtherType>& Other)
{
	if (Units != EUnit::Unspecified && Other.Units != EUnit::Unspecified)
	{
		if (Units == Other.Units)
		{
			Value = Other.Value;
		}
		else if (YUnitConversion::AreUnitsCompatible(Units, Other.Units))
		{
			Value = YUnitConversion::Convert(Other.Value, Other.Units, Units);
		}
		else
		{
			// Invalid conversion - assignment invalid
		}
	}
	else
	{
		// If our units haven't been specified, we take on the units of the rhs
		if (Units == EUnit::Unspecified)
		{
			// This is the only time we ever change units. Const_cast is 'acceptible' here since the units haven't been specified yet.
			const_cast<EUnit&>(Units) = Other.Units;
		}

		Value = Other.Value;
	}
}

template<typename NumericType>
bool YNumericUnit<NumericType>::ExtractNumberBoundary(const TCHAR* Start, const TCHAR*& End)
{
	while(FChar::IsWhitespace(*Start)) ++Start;

	End = Start;
	if (*End == '-' || *End == '+')
	{
		End++;
	}

	bool bHasDot = false;
	while (FChar::IsDigit(*End) || *End == '.')
	{
		if (*End == '.')
		{
			if (bHasDot)
			{
				return false;
			}
			bHasDot = true;
		}
		++End;
	}

	return true;
}

/** Global arithmetic operators for number types. Deals with conversion from related units correctly. */
template<typename NumericType, typename OtherType>
bool operator==(const YNumericUnit<NumericType>& LHS, const YNumericUnit<OtherType>& RHS)
{
	if (LHS.Units != EUnit::Unspecified && RHS.Units != EUnit::Unspecified)
	{
		if (LHS.Units == RHS.Units)
		{
			return LHS.Value == RHS.Value;
		}
		else if (YUnitConversion::AreUnitsCompatible(LHS.Units, RHS.Units))
		{
			return LHS.Value == YUnitConversion::Convert(RHS.Value, RHS.Units, LHS.Units);
		}
		else
		{
			// Invalid conversion
			return false;
		}
	}
	else
	{
		return LHS.Value == RHS.Value;
	}
}

template<typename NumericType, typename OtherType>
bool operator!=(const YNumericUnit<NumericType>& LHS, const YNumericUnit<OtherType>& RHS)
{
	return !(LHS == RHS);
}


template <typename NumericType>
struct TNumericLimits<YNumericUnit<NumericType>> : public TNumericLimits<NumericType>
{ };

namespace Lex
{
	template<typename T>
	YString ToString(const YNumericUnit<T>& NumericUnit)
	{
		YString String = ToString(NumericUnit.Value);
		String += TEXT(" ");
		String += YUnitConversion::GetUnitDisplayString(NumericUnit.Units);

		return String;
	}

	template<typename T>
	YString ToSanitizedString(const YNumericUnit<T>& NumericUnit)
	{
		YString String = ToSanitizedString(NumericUnit.Value);
		String += TEXT(" ");
		String += YUnitConversion::GetUnitDisplayString(NumericUnit.Units);

		return String;
	}

	template<typename T>
	void FromString(YNumericUnit<T>& OutValue, const TCHAR* String)
	{
		auto Parsed = YNumericUnit<T>::TryParseString(String);
		if (Parsed)
		{
			OutValue = Parsed.GetValue();
		}
	}
	
	template<typename T>
	bool TryParseString(YNumericUnit<T>& OutValue, const TCHAR* String)
	{
		auto Parsed = YNumericUnit<T>::TryParseString(String);
		if (Parsed)
		{
			OutValue = Parsed.GetValue();
			return true;
		}

		return false;
	}
}
