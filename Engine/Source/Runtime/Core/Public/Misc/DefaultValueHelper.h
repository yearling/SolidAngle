// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Containers/SolidAngleString.h"
#include "Misc/Timespan.h"

//
//	FDefaultValueHelper
//

class CORE_API FDefaultValueHelper
{
	/** Something like TEXT macro for chars is needed */
	static TCHAR TS(const TCHAR* Sign);

	static bool IsWhitespace(TCHAR Char);

	/** 
	 *	advances Pos to first non-whitespace symbol
	 *	returns if some non-Whitespace sign remains in string 
	 */
	static bool Trim(int32& Pos, const YString& Source);

	/** returns address of the first char in the string */
	static const TCHAR* StartOf(const YString& Source);

	/** returns address of the last (empty) char in the string */
	static const TCHAR* EndOf(const YString& Source);

	/** 
	 *	advances Pos to first non-whitespace symbol
	 *	returns if some non-Whitespace sign remains in string 
	 */
	static bool Trim(const TCHAR* & Start, const TCHAR* End );

	/**
	 *	@param Start - address of the first symbol of the checked string
	 *  @param End - address of the first symbol after of the checked string (also address of the terminating empty symbol)
	 */
	static bool IsStringValidFloat(const TCHAR* Start, const TCHAR* End);

	/** return if the string can be parse to an int. OutBase is the base of the integer (8, 10, 16)*/
	static bool IsStringValidInteger(const TCHAR* Start, const TCHAR* End, int32& OutBase);
	static bool IsStringValidInteger(const TCHAR* Start, const TCHAR* End);

public:

	static YString GetUnqualifiedEnumValue(const YString& Source);

	static bool HasWhitespaces(const YString& Source);

	static YString RemoveWhitespaces(const YString& Source);

	/** Shell parameters list: " TypeName ( A a, B b ) " -> "A a, B b" */
	static bool GetParameters(const YString& Source, const YString& TypeName, YString& OutForm);

	/** returns if given strings are equal, ignores initial and final white spaces in Source */
	static bool Is(const YString& Source, const TCHAR* CompareStr);

	/**   
	 * source forms:	TypeName( TEXT ("ABC") ), TEXT("ABC"), TypeName("ABC"), "ABC"
	 * output form:		ABC
	 */
	static bool StringFromCppString(const YString& Source, const YString& TypeName, YString& OutForm);

	/*
	 *	Following functions accept c++ style representations of numbers.
	 *  e.g. 13.5e-2f for float or -0x123 for int
	 */
	static bool IsStringValidInteger(const YString& Source);

	static bool IsStringValidFloat(const YString& Source);

	/** accepted form: " %f, %f, %f" */
	static bool IsStringValidVector(const YString& Source);

	/** accepted form: " %f, %f, %f" */
	static bool IsStringValidRotator(const YString& Source);

	/** accepted form: " %f, %f, %f " or " %f, %f, %f, %f " (alpha is optional)  */
	static bool IsStringValidLinearColor(const YString& Source);


	/**
	 * Converts a string into a int32.
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output integer
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseInt(const YString& Source, int32& OutVal);

	/**
	 * Converts a string into a int64.
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output integer
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseInt64(const YString& Source, int64& OutVal);

	/**
	 * Converts a string into a float.
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output float
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseFloat(const YString& Source, float& OutVal);

	/**
	 * Converts a string into a double.
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output double
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseDouble(const YString& Source, double& OutVal);

	/**
	 * Converts a string into a YVector. Accepted form: " %f, %f, %f "
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output vector
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseVector(const YString& Source, FVector& OutVal);

	/**
	 * Converts a string into a YVector. Accepted form: " %f, %f "
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output vector2D
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseVector2D(const YString& Source, YVector2D& OutVal);

	/**
	 * Converts a string into a YRotator. Accepted form: " %f, %f, %f "
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output rotator
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseRotator(const YString& Source, FRotator& OutVal);

	/**
	 * Converts a string into a YLinearColor. 
	 * Accepted forms: " %f, %f, %f " or " %f, %f, %f, %f " (alpha is optional).
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output color
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseLinearColor(const YString& Source, FLinearColor& OutVal);

	/**
	 * Converts a string into a YLinearColor. 
	 * Accepted forms: " %d, %d, %d " or " %d, %d, %d, %d " (alpha is optional).
	 *
	 * @param Source the input string to try to convert
	 * @param OutVal the output color
	 *
	 * @return true if the conversion happened, false otherwise
	 */
	static bool ParseColor(const YString& Source, FColor& OutVal);
};
