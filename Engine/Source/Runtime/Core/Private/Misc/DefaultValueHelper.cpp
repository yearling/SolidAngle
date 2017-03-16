// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/DefaultValueHelper.h"
#include "Math/Color.h"
#include "Math/Vector2D.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"


bool FDefaultValueHelper::Is(const YString& Source, const TCHAR* CompareStr)
{
	check(NULL != CompareStr);

	int32 Pos = 0;
	if( !Trim(Pos, Source) )
	{
		return false;
	}

	if( Source.Find(CompareStr, ESearchCase::CaseSensitive) != Pos )
	{
		return false;
	}
	Pos += FCString::Strlen(CompareStr);

	if( Trim(Pos, Source) )
	{
		return false;
	}

	return true;
}


YString FDefaultValueHelper::RemoveWhitespaces(const YString& Source)
{
	YString Result;
	if( !Source.IsEmpty() )
	{
		Result.Reserve( Source.Len() );
		for(int Pos = 0; Pos < Source.Len(); ++Pos)
		{
			if( !IsWhitespace( Source[Pos] ) )
			{
				Result.AppendChar( Source[Pos] );
			}
		}
	}
	return Result;
}


const TCHAR* FDefaultValueHelper::EndOf(const YString& Source)
{
	check(!Source.IsEmpty());
	return *Source + Source.Len();
}


const TCHAR* FDefaultValueHelper::StartOf(const YString& Source)
{
	check(!Source.IsEmpty());
	return *Source;
}


TCHAR FDefaultValueHelper::TS(const TCHAR* Sign)
{
	check(NULL != Sign);
	check(0 != Sign[0]);
	return Sign[0];
}


bool FDefaultValueHelper::IsWhitespace(TCHAR Char)
{
	return FChar::IsWhitespace(Char)
		|| FChar::IsLinebreak(Char)
		|| ( TS(TEXT("\r")) == Char );
}


bool FDefaultValueHelper::Trim(int32& Pos, const YString& Source)
{
	for(; Pos < Source.Len() && IsWhitespace(Source[Pos]); ++Pos) { }
	return (Pos < Source.Len());
}


bool FDefaultValueHelper::Trim(const TCHAR* & Start, const TCHAR* End )
{
	check(NULL != Start);
	check(NULL != End);
	for(; ( Start < End ) && IsWhitespace( *Start ); ++Start ) { }
	return Start < End;
}


YString FDefaultValueHelper::GetUnqualifiedEnumValue(const YString& Source)
{
	auto SeparatorPosition = Source.Find(YString("::"), ESearchCase::CaseSensitive);
	if (SeparatorPosition == INDEX_NONE)
	{
		return Source;
	}

	return Source.RightChop(SeparatorPosition + 2);
}


bool FDefaultValueHelper::HasWhitespaces(const YString& Source)
{
	for(int Pos = 0; Pos < Source.Len(); ++Pos)
	{
		if( IsWhitespace( Source[Pos] ) )
		{
			return true;
		}
	}
	return false;
}


bool FDefaultValueHelper::GetParameters(const YString& Source, const YString& TypeName, YString& OutForm)
{
	int32 Pos = 0;

	if( !Trim(Pos, Source) )
	{
		return false;
	}

	// find the beginning of actual val after "TypeName ( "
	if( Source.Find(TypeName, ESearchCase::CaseSensitive) != Pos )
	{
		return false;
	}
	Pos += TypeName.Len();
	if( !Trim(Pos, Source) )
	{
		return false;
	}
	if( TS(TEXT("(")) != Source[Pos++] )
	{
		return false;
	}
	if( !Trim(Pos, Source) )
	{
		return false;
	}
	const int32 StartPos = Pos;

	int32 EndPos = -1, PendingParentheses = 1;
	// find the end of the actual string before " ) "
	for(Pos = Source.Len() - 1; Pos > StartPos; --Pos )
	{
		if( TS(TEXT(")")) == Source[Pos]) 
		{
			PendingParentheses--;
		}
		else if(!IsWhitespace(Source[Pos]))
		{
			EndPos = Pos+1;
			break;
		}
	}

	if(EndPos < 0 || 0 != PendingParentheses)
	{
		return false;
	}

	OutForm = Source.Mid(StartPos, EndPos - StartPos);
	return true;
}


////////////////////////////////////////////////////////

bool FDefaultValueHelper::IsStringValidInteger(const TCHAR* Start, const TCHAR* End, int32& OutBase)
{
	bool bAllowHex = false;
	bool bADigitFound = false;

	if( !Trim(Start, End) )
	{
		return false;
	}

	if( ( TS(TEXT("-")) == *Start ) || ( TS(TEXT("+")) == *Start ) )
	{
		Start++;
	}

	if( !Trim(Start, End) )
	{
		return false;
	}

	if( TS(TEXT("0")) == *Start )
	{
		Start++;
		bADigitFound = true;
		if( ( TS(TEXT("x")) == *Start ) || ( TS(TEXT("X")) == *Start ) )
		{
			Start++;
			bAllowHex = true;
			bADigitFound = false;
			OutBase = 16;
		}
		else
		{
			OutBase = 8;
		}
	}
	else
	{
		OutBase = 10;
	}

	if( bAllowHex )
	{
		for(; ( Start < End ) && FChar::IsHexDigit( *Start ); ++Start)
		{
			bADigitFound = true;
		}
	}
	else
	{
		for(; ( Start < End ) && FChar::IsDigit( *Start ); ++Start)
		{
			bADigitFound = true;
		}
	}

	return ( !Trim(Start, End) ) && bADigitFound;
}


bool FDefaultValueHelper::IsStringValidInteger(const TCHAR* Start, const TCHAR* End)
{
	int32 Base;
	return IsStringValidInteger(Start, End, /*out*/ Base);
}
	
	
bool FDefaultValueHelper::IsStringValidInteger(const YString& Source)
{
	if(!Source.IsEmpty())
	{
		int32 Base;
		return IsStringValidInteger(StartOf(Source), EndOf(Source), Base);
	}
	return false;
}


bool FDefaultValueHelper::IsStringValidFloat(const TCHAR* Start, const TCHAR* End)
{
	if( !Trim(Start, End) )
	{
		return false;
	}

	if( ( TS(TEXT("-")) == *Start ) || ( TS(TEXT("+")) == *Start ) )
	{
		Start++;
	}
	if( !Trim(Start, End) )
	{
		return false;
	}

	for(; ( Start < End ) && FChar::IsDigit( *Start ); ++Start ) { }
	if( TS(TEXT(".")) == *Start )
	{
		Start++;
	}

	for(; ( Start < End ) && FChar::IsDigit( *Start ); ++Start ) { }

	if( ( TS(TEXT("e")) == *Start ) || ( TS(TEXT("E")) == *Start ) )
	{
		Start++;
		if( ( TS(TEXT("-")) == *Start ) || ( TS(TEXT("+")) == *Start ) )
		{
			Start++;
		}
	}
	for(; ( Start < End ) && FChar::IsDigit( *Start ); ++Start) { }
	if( ( TS(TEXT("f")) == *Start ) || ( TS(TEXT("F")) == *Start ) )
	{
		Start++;
	}

	return !Trim(Start, End);
}


bool FDefaultValueHelper::IsStringValidFloat(const YString& Source)
{
	if(!Source.IsEmpty())
	{
		return IsStringValidFloat( StartOf(Source), EndOf(Source) );
	}
	return false;
}


bool FDefaultValueHelper::IsStringValidVector(const YString& Source)
{
	const TCHAR* Start = StartOf(Source);
	const TCHAR* FirstComma = FCString::Strstr( Start, TEXT(",") );
	if(!FirstComma)
	{
		return false;
	}

	const TCHAR* SecondComma = FCString::Strstr( FirstComma + 1,  TEXT(",") );
	if(!SecondComma)
	{
		return false;
	}

	// there must be exactly 2 commas in the string
	if( FCString::Strstr( SecondComma + 1, TEXT(",") ) )
	{
		return false;
	}

	const TCHAR* End = EndOf(Source);
	if(	!IsStringValidFloat( Start, FirstComma ) ||
		!IsStringValidFloat( FirstComma + 1, SecondComma ) ||
		!IsStringValidFloat( SecondComma + 1, End ) )
	{
		return false;
	}
	return true;
}


bool FDefaultValueHelper::IsStringValidRotator(const YString& Source)
{
	return IsStringValidVector(Source);
}


bool FDefaultValueHelper::IsStringValidLinearColor(const YString& Source)
{
	if(Source.IsEmpty())
	{
		return false;
	}

	const TCHAR* Start = StartOf(Source);
	const TCHAR* FirstComma = FCString::Strstr( Start, TEXT(",") );
	if(!FirstComma)
	{
		return false;
	}

	const TCHAR* SecondComma = FCString::Strstr( FirstComma + 1,  TEXT(",") );
	if(!SecondComma)
	{
		return false;
	}

	const TCHAR* ThirdComma = FCString::Strstr( SecondComma + 1,  TEXT(",") );
	const TCHAR* End = EndOf(Source);
	if( ( NULL != ThirdComma ) && !IsStringValidFloat( ThirdComma + 1, End ) )
	{
		return false;
	}

	if(	!IsStringValidFloat( Start, FirstComma ) ||
		!IsStringValidFloat( FirstComma + 1, SecondComma ) ||
		!IsStringValidFloat( SecondComma + 1, ( NULL != ThirdComma ) ? ThirdComma : End ) )
	{
		return false;
	}

	return true;
}


bool FDefaultValueHelper::StringFromCppString(const YString& Source, const YString& TypeName, YString& OutForm)
{
	int32 Pos = 0, PendingParentheses = 0;

	if( !Trim(Pos, Source) )
	{
		return false;
	}

	// remove "TypeName ( "
	if (Source.Find(TypeName, ESearchCase::CaseSensitive) == Pos)
	{
		Pos += TypeName.Len();
		if( !Trim(Pos, Source) )
		{
			return false;
		}

		if (Source.Find(YString("::"), ESearchCase::CaseSensitive) == Pos)
		{
			Pos += 2;

			if (!Trim(Pos, Source))
			{
				return false;
			}

			const YString AllowedFunctionName(TEXT("FromString"));
			if (Source.Find(AllowedFunctionName, ESearchCase::CaseSensitive, ESearchDir::FromStart, Pos) == Pos)
			{
				Pos += AllowedFunctionName.Len();
			}
			else
			{
				return false;
			}

			if (!Trim(Pos, Source))
			{
				return false;
			}
		}

		if( TS(TEXT("(")) != Source[Pos++] )
		{
			return false;
		}
		PendingParentheses++;
		if( !Trim(Pos, Source) )
		{
			return false;
		}		
	}

	// remove "TEXT ( "
	const TCHAR* TextStr = TEXT("TEXT");
	if (Source.Find(TextStr, ESearchCase::CaseSensitive) == Pos)
	{
		Pos += FCString::Strlen(TextStr);
		if( !Trim(Pos, Source) )
		{
			return false;
		}
		if( TS(TEXT("(")) != Source[Pos++] )
		{
			return false;
		}
		PendingParentheses++;
		if( !Trim(Pos, Source) )
		{
			return false;
		}
	}

	// ensure the beginning of actual string is found
	if( TS(TEXT("\"")) != Source[Pos++] )
	{
		return false;
	}
	const int32 StartPos = Pos;
	int32 EndPos = -1;

	// find end of the actual string
	for(; Pos < Source.Len(); ++Pos)
	{
		if( ( TS(TEXT("\"")) == Source[Pos] ) && ( TS(TEXT("\\")) != Source[Pos-1] ) )
		{
			EndPos = Pos;
			Pos++;
			break;
		}
	}

	for(; Pos < Source.Len(); ++Pos)
	{
		if( TS(TEXT(")")) == Source[Pos] ) 
		{
			PendingParentheses--;
		}
		else if(!IsWhitespace(Source[Pos]))
		{
			return false;
		}
	}

	if(EndPos < 0 || 0 != PendingParentheses)
	{
		return false;
	}

	OutForm = Source.Mid(StartPos, EndPos - StartPos);
	return true;
}


////////////////////////////////////////////////////////

bool FDefaultValueHelper::ParseVector(const YString& Source, YVector& OutVal)
{
	const bool bHasWhitespace = HasWhitespaces(Source);
	const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
	const YString& ProperSource = bHasWhitespace ? NoWhitespace : Source;
	if(ProperSource.IsEmpty())
	{
		return false;
	}

	const TCHAR* Start = StartOf(ProperSource);
	const TCHAR* FirstComma = FCString::Strstr( Start, TEXT(",") );
	if(!FirstComma)
	{
		return false;
	}

	const TCHAR* SecondComma = FCString::Strstr( FirstComma + 1,  TEXT(",") );
	if(!SecondComma)
	{
		return false;
	}

	// there must be exactly 2 commas in the string
	if( FCString::Strstr( SecondComma + 1, TEXT(",") ) )
	{
		return false;
	}

	const TCHAR* End = EndOf(ProperSource);
	if(	!IsStringValidFloat( Start, FirstComma ) ||
		!IsStringValidFloat( FirstComma + 1, SecondComma ) ||
		!IsStringValidFloat( SecondComma + 1, End ) )
	{
		return false;
	}
	
	OutVal = YVector( 
		FCString::Atof(Start),
		FCString::Atof(FirstComma + 1),
		FCString::Atof(SecondComma + 1) );
	return true;
}


bool FDefaultValueHelper::ParseVector2D(const YString& Source, YVector2D& OutVal)
{
	const bool bHasWhitespace = HasWhitespaces(Source);
	const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
	const YString& ProperSource = bHasWhitespace ? NoWhitespace : Source;
	if(ProperSource.IsEmpty())
	{
		return false;
	}

	const TCHAR* Start = StartOf(ProperSource);
	const TCHAR* FirstComma = FCString::Strstr( Start, TEXT(",") );
	if(!FirstComma)
	{
		return false;
	}

	const TCHAR* End = EndOf(ProperSource);
	if(	!IsStringValidFloat( Start, FirstComma ) ||
		!IsStringValidFloat( FirstComma + 1, End ) )
	{
		return false;
	}

	OutVal = YVector2D( 
		FCString::Atof(Start),
		FCString::Atof(FirstComma + 1) );
	return true;
}


bool FDefaultValueHelper::ParseRotator(const YString& Source, YRotator& OutVal)
{
	YVector Vector;
	if( ParseVector( Source, Vector ) )
	{
		OutVal = YRotator(Vector.X, Vector.Y, Vector.Z);
		return true;
	}
	return false;
}


bool FDefaultValueHelper::ParseInt(const YString& Source, int32& OutVal)
{
	int32 Base;
	if( !Source.IsEmpty() && IsStringValidInteger( StartOf(Source), EndOf(Source), Base ) )
	{
		const bool bHasWhitespace = HasWhitespaces(Source);
		const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
		OutVal = FCString::Strtoi( bHasWhitespace ? *NoWhitespace : *Source , NULL, Base );
		return true;
	}
	return false;
}


bool FDefaultValueHelper::ParseInt64(const YString& Source, int64& OutVal)
{
	int32 Base;
	if( !Source.IsEmpty() && IsStringValidInteger( StartOf(Source), EndOf(Source), Base ) )
	{
		const bool bHasWhitespace = HasWhitespaces(Source);
		const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
		OutVal = FCString::Strtoui64( bHasWhitespace ? *NoWhitespace : *Source , NULL, Base );
		return true;
	}
	return false;
}


bool FDefaultValueHelper::ParseFloat(const YString& Source, float& OutVal)
{
	if( IsStringValidFloat( Source ) )
	{
		const bool bHasWhitespace = HasWhitespaces(Source);
		const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
		OutVal =  FCString::Atof( bHasWhitespace ? *NoWhitespace : *Source );
		return true;
	}
	return false;
}


bool FDefaultValueHelper::ParseDouble(const YString& Source, double& OutVal)
{
	if( IsStringValidFloat( Source ) )
	{
		const bool bHasWhitespace = HasWhitespaces(Source);
		const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
		OutVal =  FCString::Atod( bHasWhitespace ? *NoWhitespace : *Source );
		return true;
	}
	return false;
}


bool FDefaultValueHelper::ParseLinearColor(const YString& Source, YLinearColor& OutVal)
{
	const bool bHasWhitespace = HasWhitespaces(Source);
	const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
	const YString& ProperSource = bHasWhitespace ? NoWhitespace : Source;
	if(ProperSource.IsEmpty())
	{
		return false;
	}

	const TCHAR* Start = StartOf(ProperSource);
	const TCHAR* FirstComma = FCString::Strstr( Start, TEXT(",") );
	if(!FirstComma)
	{
		return false;
	}

	const TCHAR* SecondComma = FCString::Strstr( FirstComma + 1,  TEXT(",") );
	if(!SecondComma)
	{
		return false;
	}

	const TCHAR* ThirdComma = FCString::Strstr( SecondComma + 1,  TEXT(",") );
	const TCHAR* End = EndOf(ProperSource);
	if( ( NULL != ThirdComma ) && !IsStringValidFloat( ThirdComma + 1, End ) )
	{
		return false;
	}
	
	if(	!IsStringValidFloat( Start, FirstComma ) ||
		!IsStringValidFloat( FirstComma + 1, SecondComma ) ||
		!IsStringValidFloat( SecondComma + 1, ( NULL != ThirdComma ) ? ThirdComma : End ) )
	{
		return false;
	}

	const float Alpha = ( NULL != ThirdComma ) ? FCString::Atof( ThirdComma + 1 ) : 1.0f;
	OutVal = YLinearColor( 
		FCString::Atof( Start ), 
		FCString::Atof( FirstComma + 1 ), 
		FCString::Atof( SecondComma + 1 ),
		Alpha );
	
	return true;
}

bool FDefaultValueHelper::ParseColor(const YString& Source, YColor& OutVal)
{
	const bool bHasWhitespace = HasWhitespaces(Source);
	const YString NoWhitespace = bHasWhitespace ? RemoveWhitespaces(Source) : YString();
	const YString& ProperSource = bHasWhitespace ? NoWhitespace : Source;
	if(ProperSource.IsEmpty())
	{
		return false;
	}

	const TCHAR* Start = StartOf(ProperSource);
	const TCHAR* FirstComma = FCString::Strstr( Start, TEXT(",") );
	if(!FirstComma)
	{
		return false;
	}

	const TCHAR* SecondComma = FCString::Strstr( FirstComma + 1,  TEXT(",") );
	if(!SecondComma)
	{
		return false;
	}

	const TCHAR* ThirdComma = FCString::Strstr( SecondComma + 1,  TEXT(",") );
	const TCHAR* End = EndOf(ProperSource);
	if( ( NULL != ThirdComma ) && !IsStringValidInteger( ThirdComma + 1, End ) )
	{
		return false;
	}
	
	if(	!IsStringValidInteger( Start, FirstComma ) ||
		!IsStringValidInteger( FirstComma + 1, SecondComma ) ||
		!IsStringValidInteger( SecondComma + 1, ( NULL != ThirdComma ) ? ThirdComma : End ) )
	{
		return false;
	}

	const uint8 Alpha = ( NULL != ThirdComma ) ? FCString::Atoi( ThirdComma + 1 ) : 255;
	OutVal = YColor( 
		FCString::Atoi( Start ), 
		FCString::Atoi( FirstComma + 1 ), 
		FCString::Atoi( SecondComma + 1 ),
		Alpha );
	
	return true;
}
