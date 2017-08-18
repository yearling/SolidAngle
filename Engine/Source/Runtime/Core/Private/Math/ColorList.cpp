// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	ColorList.cpp: List of common colors implementation.
=============================================================================*/

#include "Math/ColorList.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogColorList, Log, All);

/** Global instance of color list helper class. */
YColorList GColorList;

const FColor& YColorList::GetFColorBFName( const TCHAR* ColorName ) const
{
	const FColor* Color = ColorsMap.FindRef( ColorName );

	if( Color != NULL )
	{
		return *Color;
	}
	
	return White;
}

const FColor& YColorList::GetFColorByIndex( int32 ColorIndex ) const
{
	if( ColorsLookup.IsValidIndex( ColorIndex ) == true )
	{
		return *ColorsLookup[ ColorIndex ];
	}
	
	return White;
}

const FLinearColor YColorList::GetFLinearColorBFName( const TCHAR* ColorName ) const
{
	const FColor* Color = ColorsMap.FindRef( ColorName );

	if( Color != NULL )
	{
		return FLinearColor( *Color );
	}

	return FLinearColor::White;
}

bool YColorList::IsValidColorName( const TCHAR* ColorName ) const
{
	const FColor* Color = ColorsMap.FindRef( ColorName );
	return Color != NULL ? true : false;
}


int32 YColorList::GetColorIndex( const TCHAR* ColorName ) const
{
	const FColor& Color = GetFColorBFName( ColorName );
	int32 ColorIndex = 0;
	ColorsLookup.Find( &Color, ColorIndex );
	return ColorIndex;
}

const YString& YColorList::GetColorNameByIndex( int32 ColorIndex ) const
{
	static const YString BadIndex( TEXT( "BadIndex" ) );

	if( ColorsLookup.IsValidIndex( ColorIndex ) == true )
	{
		const FColor& Color = *ColorsLookup[ ColorIndex ];
		const YString& ColorName = *ColorsMap.FindKey( &Color );
		return ColorName;
	}

	return BadIndex;
}

void YColorList::CreateColorMap()
{
	int32 Index = 0;
	// Black color must be first.
	InitializeColor( TEXT("black"),				&YColorList::Black, Index );
	InitializeColor( TEXT("aquamarine"),		&YColorList::Aquamarine, Index );
	InitializeColor( TEXT("bakerchocolate"),	&YColorList::BakerChocolate, Index );
	InitializeColor( TEXT("blue"),				&YColorList::Blue, Index );
	InitializeColor( TEXT("blueviolet"),		&YColorList::BlueViolet, Index );
	InitializeColor( TEXT("brass"),				&YColorList::Brass, Index );
	InitializeColor( TEXT("brightgold"),		&YColorList::BrightGold, Index );
	InitializeColor( TEXT("brown"),				&YColorList::Brown, Index );
	InitializeColor( TEXT("bronze"),			&YColorList::Bronze, Index );
	InitializeColor( TEXT("bronzeii"),			&YColorList::BronzeII, Index );
	InitializeColor( TEXT("cadetblue"),			&YColorList::CadetBlue, Index );
	InitializeColor( TEXT("coolcopper"),		&YColorList::CoolCopper, Index );
	InitializeColor( TEXT("copper"),			&YColorList::Copper, Index );
	InitializeColor( TEXT("coral"),				&YColorList::Coral, Index );
	InitializeColor( TEXT("cornflowerblue"),	&YColorList::CornFlowerBlue, Index );
	InitializeColor( TEXT("cyan"),				&YColorList::Cyan, Index );
	InitializeColor( TEXT("darkbrown"),			&YColorList::DarkBrown, Index );
	InitializeColor( TEXT("darkgreen"),			&YColorList::DarkGreen, Index );
	InitializeColor( TEXT("darkgreencopper"),	&YColorList::DarkGreenCopper, Index );
	InitializeColor( TEXT("darkolivegreen"),	&YColorList::DarkOliveGreen, Index );
	InitializeColor( TEXT("darkorchid"),		&YColorList::DarkOrchid, Index );
	InitializeColor( TEXT("darkpurple"),		&YColorList::DarkPurple, Index );
	InitializeColor( TEXT("darkslateblue"),		&YColorList::DarkSlateBlue, Index );
	InitializeColor( TEXT("darkslategrey"),		&YColorList::DarkSlateGrey, Index );
	InitializeColor( TEXT("darktan"),			&YColorList::DarkTan, Index );
	InitializeColor( TEXT("darkturquoise"),		&YColorList::DarkTurquoise, Index );
	InitializeColor( TEXT("darkwood"),			&YColorList::DarkWood, Index );
	InitializeColor( TEXT("dimgrey"),			&YColorList::DimGrey, Index );
	InitializeColor( TEXT("dustyrose"),			&YColorList::DustyRose, Index );
	InitializeColor( TEXT("feldspar"),			&YColorList::Feldspar, Index );
	InitializeColor( TEXT("firebrick"),			&YColorList::Firebrick, Index );
	InitializeColor( TEXT("forestgreen"),		&YColorList::ForestGreen, Index );
	InitializeColor( TEXT("gold"),				&YColorList::Gold, Index );
	InitializeColor( TEXT("goldenrod"),			&YColorList::Goldenrod, Index );
	InitializeColor( TEXT("green"),				&YColorList::Green, Index );
	InitializeColor( TEXT("greencopper"),		&YColorList::GreenCopper, Index );
	InitializeColor( TEXT("greenyellow"),		&YColorList::GreenYellow, Index );
	InitializeColor( TEXT("grey"),				&YColorList::Grey, Index );
	InitializeColor( TEXT("huntergreen"),		&YColorList::HunterGreen, Index );
	InitializeColor( TEXT("indianred"),			&YColorList::IndianRed, Index );
	InitializeColor( TEXT("khaki"),				&YColorList::Khaki, Index );
	InitializeColor( TEXT("lightblue"),			&YColorList::LightBlue, Index );
	InitializeColor( TEXT("lightgrey"),			&YColorList::LightGrey, Index );
	InitializeColor( TEXT("lightsteelblue"),	&YColorList::LightSteelBlue, Index );
	InitializeColor( TEXT("lightwood"),			&YColorList::LightWood, Index );
	InitializeColor( TEXT("limegreen"),			&YColorList::LimeGreen, Index );
	InitializeColor( TEXT("magenta"),			&YColorList::Magenta, Index );
	InitializeColor( TEXT("mandarianorange"),	&YColorList::MandarianOrange, Index );
	InitializeColor( TEXT("maroon"),			&YColorList::Maroon, Index );
	InitializeColor( TEXT("mediumaquamarine"),	&YColorList::MediumAquamarine, Index );
	InitializeColor( TEXT("mediumblue"),		&YColorList::MediumBlue, Index );
	InitializeColor( TEXT("mediumforestgreen"), &YColorList::MediumForestGreen, Index );
	InitializeColor( TEXT("mediumgoldenrod"),	&YColorList::MediumGoldenrod, Index );
	InitializeColor( TEXT("mediumorchid"),		&YColorList::MediumOrchid, Index );
	InitializeColor( TEXT("mediumseagreen"),	&YColorList::MediumSeaGreen, Index );
	InitializeColor( TEXT("mediumslateblue"),	&YColorList::MediumSlateBlue, Index );
	InitializeColor( TEXT("mediumspringgreen"), &YColorList::MediumSpringGreen, Index );
	InitializeColor( TEXT("mediumturquoise"),	&YColorList::MediumTurquoise, Index );
	InitializeColor( TEXT("mediumvioletred"),	&YColorList::MediumVioletRed, Index );
	InitializeColor( TEXT("mediumwood"),		&YColorList::MediumWood, Index );
	InitializeColor( TEXT("midnightblue"),		&YColorList::MidnightBlue, Index );
	InitializeColor( TEXT("navyblue"),			&YColorList::NavyBlue, Index );
	InitializeColor( TEXT("neonblue"),			&YColorList::NeonBlue, Index );
	InitializeColor( TEXT("neonpink"),			&YColorList::NeonPink, Index );
	InitializeColor( TEXT("newmidnightblue"),	&YColorList::NewMidnightBlue, Index );
	InitializeColor( TEXT("newtan"),			&YColorList::NewTan, Index );
	InitializeColor( TEXT("oldgold"),			&YColorList::OldGold, Index );
	InitializeColor( TEXT("orange"),			&YColorList::Orange, Index );
	InitializeColor( TEXT("orangered"),			&YColorList::OrangeRed, Index );
	InitializeColor( TEXT("orchid"),			&YColorList::Orchid, Index );
	InitializeColor( TEXT("palegreen"),			&YColorList::PaleGreen, Index );
	InitializeColor( TEXT("pink"),				&YColorList::Pink, Index );
	InitializeColor( TEXT("plum"),				&YColorList::Plum, Index );
	InitializeColor( TEXT("quartz"),			&YColorList::Quartz, Index );
	InitializeColor( TEXT("red"),				&YColorList::Red, Index );
	InitializeColor( TEXT("richblue"),			&YColorList::RichBlue, Index );
	InitializeColor( TEXT("salmon"),			&YColorList::Salmon, Index );
	InitializeColor( TEXT("scarlet"),			&YColorList::Scarlet, Index );
	InitializeColor( TEXT("seagreen"),			&YColorList::SeaGreen, Index );
	InitializeColor( TEXT("semisweetchocolate"), &YColorList::SemiSweetChocolate, Index );
	InitializeColor( TEXT("sienna"),			&YColorList::Sienna, Index );
	InitializeColor( TEXT("silver"),			&YColorList::Silver, Index );
	InitializeColor( TEXT("skyblue"),			&YColorList::SkyBlue, Index );
	InitializeColor( TEXT("slateblue"),			&YColorList::SlateBlue, Index );
	InitializeColor( TEXT("spicypink"),			&YColorList::SpicyPink, Index );
	InitializeColor( TEXT("springgreen"),		&YColorList::SpringGreen, Index );
	InitializeColor( TEXT("steelblue"),			&YColorList::SteelBlue, Index );
	InitializeColor( TEXT("summersky"),			&YColorList::SummerSky, Index );
	InitializeColor( TEXT("tan"),				&YColorList::Tan, Index );
	InitializeColor( TEXT("thistle"),			&YColorList::Thistle, Index );
	InitializeColor( TEXT("turquoise"),			&YColorList::Turquoise, Index );
	InitializeColor( TEXT("verydarkbrown"),		&YColorList::VeryDarkBrown, Index );
	InitializeColor( TEXT("verylightgrey"),		&YColorList::VeryLightGrey, Index );
	InitializeColor( TEXT("violet"),			&YColorList::Violet, Index );
	InitializeColor( TEXT("violetred"),			&YColorList::VioletRed, Index );
	InitializeColor( TEXT("wheat"),				&YColorList::Wheat, Index );
	InitializeColor( TEXT("white"),				&YColorList::White, Index );
	InitializeColor( TEXT("yellow"),			&YColorList::Yellow, Index );
	InitializeColor( TEXT("yellowgreen"),		&YColorList::YellowGreen, Index );

	ColorsLookup.Shrink();
}

void YColorList::InitializeColor( const TCHAR* ColorName, const FColor* ColorPtr, int32& CurrentIndex )
{
	ColorsMap.Add( ColorName, ColorPtr );
	ColorsLookup.Add( ColorPtr );

	CurrentIndex ++;
}

void YColorList::LogColors()
{
	for( TColorsMap::TIterator It(ColorsMap); It; ++It )
	{
		const FColor* ColorPtr = It.Value();
		const YString& ColorName = It.Key();

		int32 ColorIndex = 0;
		ColorsLookup.Find( ColorPtr, ColorIndex );

		UE_LOG(LogColorList, Log,  TEXT( "%3i - %32s -> %s" ), ColorIndex, *ColorName, *ColorPtr->ToString() );
	}
}

// Common colors declarations.
const FColor YColorList::White            ( 255, 255, 255, 255 );
const FColor YColorList::Red              ( 255,   0,   0, 255 );
const FColor YColorList::Green            (   0, 255,   0, 255 );
const FColor YColorList::Blue             (   0,   0, 255, 255 );
const FColor YColorList::Magenta          ( 255,   0, 255, 255 );
const FColor YColorList::Cyan             (   0, 255, 255, 255 );
const FColor YColorList::Yellow           ( 255, 255,   0, 255 );
const FColor YColorList::Black            (   0,   0,   0, 255 );
const FColor YColorList::Aquamarine       ( 112, 219, 147, 255 );
const FColor YColorList::BakerChocolate   (  92,  51,  23, 255 );
const FColor YColorList::BlueViolet       ( 159,  95, 159, 255 );
const FColor YColorList::Brass            ( 181, 166,  66, 255 );
const FColor YColorList::BrightGold       ( 217, 217,  25, 255 );
const FColor YColorList::Brown            ( 166,  42,  42, 255 );
const FColor YColorList::Bronze           ( 140, 120,  83, 255 );
const FColor YColorList::BronzeII         ( 166, 125,  61, 255 );
const FColor YColorList::CadetBlue        (  95, 159, 159, 255 );
const FColor YColorList::CoolCopper       ( 217, 135,  25, 255 );
const FColor YColorList::Copper           ( 184, 115,  51, 255 );
const FColor YColorList::Coral            ( 255, 127,   0, 255 );
const FColor YColorList::CornFlowerBlue   (  66,  66, 111, 255 );
const FColor YColorList::DarkBrown        (  92,  64,  51, 255 );
const FColor YColorList::DarkGreen        (  47,  79,  47, 255 );
const FColor YColorList::DarkGreenCopper  (  74, 118, 110, 255 );
const FColor YColorList::DarkOliveGreen   (  79,  79,  47, 255 );
const FColor YColorList::DarkOrchid       ( 153,  50, 205, 255 );
const FColor YColorList::DarkPurple       ( 135,  31, 120, 255 );
const FColor YColorList::DarkSlateBlue    ( 107,  35, 142, 255 );
const FColor YColorList::DarkSlateGrey    (  47,  79,  79, 255 );
const FColor YColorList::DarkTan          ( 151, 105,  79, 255 );
const FColor YColorList::DarkTurquoise    ( 112, 147, 219, 255 );
const FColor YColorList::DarkWood         ( 133,  94,  66, 255 );
const FColor YColorList::DimGrey          (  84,  84,  84, 255 );
const FColor YColorList::DustyRose        ( 133,  99,  99, 255 );
const FColor YColorList::Feldspar         ( 209, 146, 117, 255 );
const FColor YColorList::Firebrick        ( 142,  35,  35, 255 );
const FColor YColorList::ForestGreen      (  35, 142,  35, 255 );
const FColor YColorList::Gold             ( 205, 127,  50, 255 );
const FColor YColorList::Goldenrod        ( 219, 219, 112, 255 );
const FColor YColorList::Grey             ( 192, 192, 192, 255 );
const FColor YColorList::GreenCopper      (  82, 127, 118, 255 );
const FColor YColorList::GreenYellow      ( 147, 219, 112, 255 );
const FColor YColorList::HunterGreen      (  33,  94,  33, 255 );
const FColor YColorList::IndianRed        (  78,  47,  47, 255 );
const FColor YColorList::Khaki            ( 159, 159,  95, 255 );
const FColor YColorList::LightBlue        ( 192, 217, 217, 255 );
const FColor YColorList::LightGrey        ( 168, 168, 168, 255 );
const FColor YColorList::LightSteelBlue   ( 143, 143, 189, 255 );
const FColor YColorList::LightWood        ( 233, 194, 166, 255 );
const FColor YColorList::LimeGreen        (  50, 205,  50, 255 );
const FColor YColorList::MandarianOrange  ( 228, 120,  51, 255 );
const FColor YColorList::Maroon           ( 142,  35, 107, 255 );
const FColor YColorList::MediumAquamarine (  50, 205, 153, 255 );
const FColor YColorList::MediumBlue       (  50,  50, 205, 255 );
const FColor YColorList::MediumForestGreen( 107, 142,  35, 255 );
const FColor YColorList::MediumGoldenrod  ( 234, 234, 174, 255 );
const FColor YColorList::MediumOrchid     ( 147, 112, 219, 255 );
const FColor YColorList::MediumSeaGreen   (  66, 111,  66, 255 );
const FColor YColorList::MediumSlateBlue  ( 127,   0, 255, 255 );
const FColor YColorList::MediumSpringGreen( 127, 255,   0, 255 );
const FColor YColorList::MediumTurquoise  ( 112, 219, 219, 255 );
const FColor YColorList::MediumVioletRed  ( 219, 112, 147, 255 );
const FColor YColorList::MediumWood       ( 166, 128, 100, 255 );
const FColor YColorList::MidnightBlue     (  47,  47,  79, 255 );
const FColor YColorList::NavyBlue         (  35,  35, 142, 255 );
const FColor YColorList::NeonBlue         (  77,  77, 255, 255 );
const FColor YColorList::NeonPink         ( 255, 110, 199, 255 );
const FColor YColorList::NewMidnightBlue  (   0,   0, 156, 255 );
const FColor YColorList::NewTan           ( 235, 199, 158, 255 );
const FColor YColorList::OldGold          ( 207, 181,  59, 255 );
const FColor YColorList::Orange           ( 255, 127,   0, 255 );
const FColor YColorList::OrangeRed        ( 255,  36,   0, 255 );
const FColor YColorList::Orchid           ( 219, 112, 219, 255 );
const FColor YColorList::PaleGreen        ( 143, 188, 143, 255 );
const FColor YColorList::Pink             ( 188, 143, 143, 255 );
const FColor YColorList::Plum             ( 234, 173, 234, 255 );
const FColor YColorList::Quartz           ( 217, 217, 243, 255 );
const FColor YColorList::RichBlue         (  89,  89, 171, 255 );
const FColor YColorList::Salmon           ( 111,  66,  66, 255 );
const FColor YColorList::Scarlet          ( 140,  23,  23, 255 );
const FColor YColorList::SeaGreen         (  35, 142, 104, 255 );
const FColor YColorList::SemiSweetChocolate(107,  66,  38, 255 );
const FColor YColorList::Sienna           ( 142, 107,  35, 255 );
const FColor YColorList::Silver           ( 230, 232, 250, 255 );
const FColor YColorList::SkyBlue          (  50, 153, 204, 255 );
const FColor YColorList::SlateBlue        (   0, 127, 255, 255 );
const FColor YColorList::SpicyPink        ( 255,  28, 174, 255 );
const FColor YColorList::SpringGreen      (   0, 255, 127, 255 );
const FColor YColorList::SteelBlue        (  35, 107, 142, 255 );
const FColor YColorList::SummerSky        (  56, 176, 222, 255 );
const FColor YColorList::Tan              ( 219, 147, 112, 255 );
const FColor YColorList::Thistle          ( 216, 191, 216, 255 );
const FColor YColorList::Turquoise        ( 173, 234, 234, 255 );
const FColor YColorList::VeryDarkBrown    (  92,  64,  51, 255 );
const FColor YColorList::VeryLightGrey    ( 205, 205, 205, 255 );
const FColor YColorList::Violet           (  79,  47,  79, 255 );
const FColor YColorList::VioletRed        ( 204,  50, 153, 255 );
const FColor YColorList::Wheat            ( 216, 216, 191, 255 );
const FColor YColorList::YellowGreen      ( 153, 204,  50, 255 );
