#pragma once
#include "Containers/ContainersFwd.h"

/** An argument supplied to YString::Format */
struct CORE_API YStringFormatArg
{
	enum EType { Int, UInt, Double, String, StringLiteral };

	/** The type of this arg */
	EType Type;

	/* todo: convert this to a TVariant */
	union
	{
		/** Value as integer */
		int64 IntValue;
		/** Value as uint */
		uint64 UIntValue;
		/** Value as double */
		double DoubleValue;
		/** Value as a string literal */
		const TCHAR* StringLiteralValue;
	};

	/** Value as an YString */
	YString StringValue;

	YStringFormatArg(const int32 Value);
	YStringFormatArg(const uint32 Value);
	YStringFormatArg(const int64 Value);
	YStringFormatArg(const uint64 Value);
	YStringFormatArg(const float Value);
	YStringFormatArg(const double Value);
	YStringFormatArg(YString Value);
	YStringFormatArg(const TCHAR* Value);

	/** Copyable */
	YStringFormatArg(const YStringFormatArg& RHS);

private:

	/** Not default constructible */
	YStringFormatArg();
};
