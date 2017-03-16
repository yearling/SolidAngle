// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Misc/Exec.h"
#include "Containers/Array.h"
#include "Containers/SolidAngleString.h"
#include "Containers/Map.h"
#include "Math/Color.h"

class CORE_API FColorList
	: public YExec
{
public:
	typedef TMap< YString, const YColor* > TColorsMap;
	typedef TArray< const YColor* > TColorsLookup;

	// Common colors.	
	static const YColor White;
	static const YColor Red;
	static const YColor Green;
	static const YColor Blue;
	static const YColor Magenta;
	static const YColor Cyan;
	static const YColor Yellow;
	static const YColor Black;
	static const YColor Aquamarine;
	static const YColor BakerChocolate;
	static const YColor BlueViolet;
	static const YColor Brass;
	static const YColor BrightGold;
	static const YColor Brown;
	static const YColor Bronze;
	static const YColor BronzeII;
	static const YColor CadetBlue;
	static const YColor CoolCopper;
	static const YColor Copper;
	static const YColor Coral;
	static const YColor CornFlowerBlue;
	static const YColor DarkBrown;
	static const YColor DarkGreen;
	static const YColor DarkGreenCopper;
	static const YColor DarkOliveGreen;
	static const YColor DarkOrchid;
	static const YColor DarkPurple;
	static const YColor DarkSlateBlue;
	static const YColor DarkSlateGrey;
	static const YColor DarkTan;
	static const YColor DarkTurquoise;
	static const YColor DarkWood;
	static const YColor DimGrey;
	static const YColor DustyRose;
	static const YColor Feldspar;
	static const YColor Firebrick;
	static const YColor ForestGreen;
	static const YColor Gold;
	static const YColor Goldenrod;
	static const YColor Grey;
	static const YColor GreenCopper;
	static const YColor GreenYellow;
	static const YColor HunterGreen;
	static const YColor IndianRed;
	static const YColor Khaki;
	static const YColor LightBlue;
	static const YColor LightGrey;
	static const YColor LightSteelBlue;
	static const YColor LightWood;
	static const YColor LimeGreen;
	static const YColor MandarianOrange;
	static const YColor Maroon;
	static const YColor MediumAquamarine;
	static const YColor MediumBlue;
	static const YColor MediumForestGreen;
	static const YColor MediumGoldenrod;
	static const YColor MediumOrchid;
	static const YColor MediumSeaGreen;
	static const YColor MediumSlateBlue;
	static const YColor MediumSpringGreen;
	static const YColor MediumTurquoise;
	static const YColor MediumVioletRed;
	static const YColor MediumWood;
	static const YColor MidnightBlue;
	static const YColor NavyBlue;
	static const YColor NeonBlue;
	static const YColor NeonPink;
	static const YColor NewMidnightBlue;
	static const YColor NewTan;
	static const YColor OldGold;
	static const YColor Orange;
	static const YColor OrangeRed;
	static const YColor Orchid;
	static const YColor PaleGreen;
	static const YColor Pink;
	static const YColor Plum;
	static const YColor Quartz;
	static const YColor RichBlue;
	static const YColor Salmon;
	static const YColor Scarlet;
	static const YColor SeaGreen;
	static const YColor SemiSweetChocolate;
	static const YColor Sienna;
	static const YColor Silver;
	static const YColor SkyBlue;
	static const YColor SlateBlue;
	static const YColor SpicyPink;
	static const YColor SpringGreen;
	static const YColor SteelBlue;
	static const YColor SummerSky;
	static const YColor Tan;
	static const YColor Thistle;
	static const YColor Turquoise;
	static const YColor VeryDarkBrown;
	static const YColor VeryLightGrey;
	static const YColor Violet;
	static const YColor VioletRed;
	static const YColor Wheat;
	static const YColor YellowGreen;

	/** Initializes list of common colors. */
	void CreateColorMap();

	/** Returns a color based on ColorName if not found returs White. */
	const YColor& GetFColorByName(const TCHAR* ColorName) const;

	/** Returns a linear color based on ColorName if not found returs White. */
	const YLinearColor GetFLinearColorByName(const TCHAR* ColorName) const;

	/** Returns true if color is valid common colors, returns false otherwise. */
	bool IsValidColorName(const TCHAR* ColorName) const;

	/** Returns index of color. */
	int32 GetColorIndex(const TCHAR* ColorName) const;

	/** Returns a color based on index. If index is invalid, returns White. */
	const YColor& GetFColorByIndex(int32 ColorIndex) const;

	/** Resturn color's name based on index. If index is invalid, returns BadIndex. */
	const YString& GetColorNameByIndex(int32 ColorIndex) const;

	/** Returns the number of colors. */
	int32 GetColorsNum() const
	{
		return ColorsMap.Num();
	}

	/** Prints to log all colors information. */
	void LogColors();

protected:
	void InitializeColor(const TCHAR* ColorName, const YColor* ColorPtr, int32& CurrentIndex);

	/** List of common colors. */
	TColorsMap ColorsMap;

	/** Array of colors for fast lookup when using index. */
	TColorsLookup ColorsLookup;
};


extern CORE_API FColorList GColorList;
