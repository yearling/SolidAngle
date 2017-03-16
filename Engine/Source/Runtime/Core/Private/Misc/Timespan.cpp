// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/Timespan.h"
#include "Templates/TypeHash.h"
#include "Containers/SolidAngleString.h"
#include "SObject/PropertyPortFlags.h"

/* YTimespan interface
 *****************************************************************************/

bool YTimespan::ExportTextItem(YString& ValueStr, YTimespan const& DefaultValue, SObject* Parent, int32 PortFlags, SObject* ExportRootScope) const
{
	if (0 != (PortFlags & EPropertyPortFlags::PPF_ExportCpp))
	{
		ValueStr += YString::Printf(TEXT("YTimespan(0x%016X)"), Ticks);
		return true;
	}

	ValueStr += ToString(TEXT("%N%d.%h:%m:%s.%f"));

	return true;
}


bool YTimespan::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, SObject* Parent, YOutputDevice* ErrorText)
{
	// @todo gmp: implement YTimespan::ImportTextItem
	return false;
}


bool YTimespan::Serialize(YArchive& Ar)
{
	Ar << *this;

	return true;
}


YString YTimespan::ToString() const
{
	if (GetDays() == 0)
	{
		return ToString(TEXT("%n%h:%m:%s.%f"));
	}

	return ToString(TEXT("%n%d.%h:%m:%s.%f"));
}


YString YTimespan::ToString(const TCHAR* Format) const
{
	YString Result;

	while (*Format != TCHAR('\0'))
	{
		if ((*Format == TCHAR('%')) && (*++Format != TCHAR('\0')))
		{
			switch (*Format)
			{
			case TCHAR('n'): if (Ticks < 0) Result += TCHAR('-'); break;
			case TCHAR('N'): Result += (Ticks < 0) ? TCHAR('-') : TCHAR('+'); break;
			case TCHAR('d'): Result += YString::Printf(TEXT("%i"), YMath::Abs(GetDays())); break;
			case TCHAR('h'): Result += YString::Printf(TEXT("%02i"), YMath::Abs(GetHours())); break;
			case TCHAR('m'): Result += YString::Printf(TEXT("%02i"), YMath::Abs(GetMinutes())); break;
			case TCHAR('s'): Result += YString::Printf(TEXT("%02i"), YMath::Abs(GetSeconds())); break;
			case TCHAR('f'): Result += YString::Printf(TEXT("%03i"), YMath::Abs(GetMilliseconds())); break;
			case TCHAR('D'): Result += YString::Printf(TEXT("%f"), YMath::Abs(GetTotalDays())); break;
			case TCHAR('H'): Result += YString::Printf(TEXT("%f"), YMath::Abs(GetTotalHours())); break;
			case TCHAR('M'): Result += YString::Printf(TEXT("%f"), YMath::Abs(GetTotalMinutes())); break;
			case TCHAR('S'): Result += YString::Printf(TEXT("%f"), YMath::Abs(GetTotalSeconds())); break;
			case TCHAR('F'): Result += YString::Printf(TEXT("%f"), YMath::Abs(GetTotalMilliseconds())); break;

			default:

				Result += *Format;
			}
		}
		else
		{
			Result += *Format;
		}

		++Format;
	}

	return Result;
}


/* YTimespan static interface
 *****************************************************************************/

YTimespan YTimespan::FromDays(double Days)
{
	check((Days >= MinValue().GetTotalDays()) && (Days <= MaxValue().GetTotalDays()));

	return YTimespan(Days * ETimespan::TicksPerDay);
}


YTimespan YTimespan::FromHours(double Hours)
{
	check((Hours >= MinValue().GetTotalHours()) && (Hours <= MaxValue().GetTotalHours()));

	return YTimespan(Hours * ETimespan::TicksPerHour);
}


YTimespan YTimespan::FromMicroseconds(double Microseconds)
{
	check((Microseconds >= MinValue().GetTotalMicroseconds()) && (Microseconds <= MaxValue().GetTotalMicroseconds()));

	return YTimespan(Microseconds * ETimespan::TicksPerMicrosecond);
}


YTimespan YTimespan::FromMilliseconds(double Milliseconds)
{
	check((Milliseconds >= MinValue().GetTotalMilliseconds()) && (Milliseconds <= MaxValue().GetTotalMilliseconds()));

	return YTimespan(Milliseconds * ETimespan::TicksPerMillisecond);
}


YTimespan YTimespan::FromMinutes(double Minutes)
{
	check((Minutes >= MinValue().GetTotalMinutes()) && (Minutes <= MaxValue().GetTotalMinutes()));

	return YTimespan(Minutes * ETimespan::TicksPerMinute);
}


YTimespan YTimespan::FromSeconds(double Seconds)
{
	check((Seconds >= MinValue().GetTotalSeconds()) && (Seconds <= MaxValue().GetTotalSeconds()));

	return YTimespan(Seconds * ETimespan::TicksPerSecond);
}


bool YTimespan::Parse(const YString& TimespanString, YTimespan& OutTimespan)
{
	// @todo gmp: implement stricter YTimespan parsing; this implementation is too forgiving.
	YString TokenString = TimespanString.Replace(TEXT("."), TEXT(":"));
	TokenString.ReplaceInline(TEXT(","), TEXT(":"));

	bool Negative = TokenString.StartsWith(TEXT("-"));
	TokenString.ReplaceInline(TEXT("-"), TEXT(":"), ESearchCase::CaseSensitive);

	TArray<YString> Tokens;
	TokenString.ParseIntoArray(Tokens, TEXT(":"), true);

	if (Tokens.Num() == 4)
	{
		Tokens.Insert(TEXT("0"), 0);
	}

	if (Tokens.Num() != 5)
	{
		return false;
	}

	OutTimespan.Assign(FCString::Atoi(*Tokens[0]), FCString::Atoi(*Tokens[1]), FCString::Atoi(*Tokens[2]), FCString::Atoi(*Tokens[3]), FCString::Atoi(*Tokens[4]), 0);

	if (Negative)
	{
		OutTimespan.Ticks *= -1;
	}

	return true;
}


/* YTimespan friend functions
 *****************************************************************************/

YArchive& operator<<(YArchive& Ar, YTimespan& Timespan)
{
	return Ar << Timespan.Ticks;
}


uint32 GetTypeHash(const YTimespan& Timespan)
{
	return GetTypeHash(Timespan.Ticks);
}


/* YTimespan implementation
 *****************************************************************************/

void YTimespan::Assign(int32 Days, int32 Hours, int32 Minutes, int32 Seconds, int32 Milliseconds, int32 Microseconds)
{
	int64 TotalTicks = ETimespan::TicksPerMicrosecond * (1000 * (1000 * (60 * 60 * 24 * (int64)Days + 60 * 60 * (int64)Hours + 60 * (int64)Minutes + (int64)Seconds) + (int64)Milliseconds) + (int64)Microseconds);
	check((TotalTicks >= MinValue().GetTicks()) && (TotalTicks <= MaxValue().GetTicks()));

	Ticks = TotalTicks;
}
