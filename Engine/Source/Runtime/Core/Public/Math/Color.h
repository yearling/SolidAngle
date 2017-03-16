#pragma once
#include "Math/MathUtility.h"
// !!FIXME by zyx
//#include "Misc/Parse.h"
//#include "Serialization/Archive.h"

struct YColor;
struct YVector;
class YFloat16Color;

// Enum for the different kinds of gamma spaces we expect to need to convert from / to.
enum class EGammaSpace
{
	/** No gamma correction is applied to this space, the incoming colors are assumed to already be in linear space. */
	Linear,
	/** A simplified sRGB gamma correction is applied, pow(1/2.2). */
	Pow22,
	/** Use the standard sRGB conversion. */
	sRGB,
};

/**
* A linear, 32-bit/component floating point RGBA color.
*/
struct YLinearColor
{
	float	
		R,
		G,
		B,
		A;

	/** Static lookup table used for YColor -> YLinearColor conversion. Pow(2.2) */
	static float Pow22OneOver255Table[256];

	/** Static lookup table used for YColor -> YLinearColor conversion. sRGB */
	static float sRGBToLinearTable[256];

	FORCEINLINE					YLinearColor() {}
	FORCEINLINE explicit		YLinearColor(EForceInit)
		:R(0), G(0), B(0), A(0) {}
	FORCEINLINE					YLinearColor(float InR, float InG, float InB, float InA = 1.0f)
		: R(InR), G(InG), B(InB), A(InA) {}

	/**
	* Converts an YColor which is assumed to be in sRGB space, into linear color space.
	* @param Color The sRGB color that needs to be converted into linear space.
	*/
	CORE_API					YLinearColor(const YColor& Color);
	CORE_API					YLinearColor(const YVector& Vector);
	CORE_API explicit			YLinearColor(const YFloat16Color& C);

	// Serializer.
	// !!FIXME by zyx
	//friend YArchive& operator<<(YArchive& Ar, YLinearColor& Color)
	//{
	//	return Ar << Color.R << Color.G << Color.B << Color.A;
	//}
	//bool Serialize(YArchive& Ar)
	//{
	//	Ar << *this;
	//	return true;
	//}

	//Conversions
	/**
	* Converts an YColor coming from an observed sRGB output, into a linear color.
	* @param Color The sRGB color that needs to be converted into linear space.
	*/
	CORE_API static YLinearColor FromSRGBColor(const YColor& Color);

	/**
	* Converts an YColor coming from an observed Pow(1/2.2) output, into a linear color.
	* @param Color The Pow(1/2.2) color that needs to be converted into linear space.
	*/
	CORE_API static YLinearColor FromPow22Color(const YColor& Color);

	FORCEINLINE float&			Component(int32 Index)
	{
		return (&R)[Index];
	}

	FORCEINLINE const float&	Component(int32 Index) const
	{
		return (&R)[Index];
	}

	FORCEINLINE YLinearColor	operator+(const YLinearColor& ColorB) const
	{
		return YLinearColor(
			this->R + ColorB.R,
			this->G + ColorB.G,
			this->B + ColorB.B,
			this->A + ColorB.A
		);
	}

	FORCEINLINE YLinearColor&	operator+=(const YLinearColor& ColorB)
	{
		R += ColorB.R;
		G += ColorB.G;
		B += ColorB.B;
		A += ColorB.A;
		return *this;
	}

	FORCEINLINE YLinearColor	operator-(const YLinearColor& ColorB) const
	{
		return YLinearColor(
			this->R - ColorB.R,
			this->G - ColorB.G,
			this->B - ColorB.B,
			this->A - ColorB.A
		);
	}

	FORCEINLINE YLinearColor&	operator-=(const YLinearColor& ColorB)
	{
		R -= ColorB.R;
		G -= ColorB.G;
		B -= ColorB.B;
		A -= ColorB.A;
		return *this;
	}

	FORCEINLINE YLinearColor	operator*(const YLinearColor& ColorB) const
	{
		return YLinearColor(
			this->R * ColorB.R,
			this->G * ColorB.G,
			this->B * ColorB.B,
			this->A * ColorB.A
		);
	}

	FORCEINLINE YLinearColor&	operator*=(const YLinearColor& ColorB)
	{
		R *= ColorB.R;
		G *= ColorB.G;
		B *= ColorB.B;
		A *= ColorB.A;
		return *this;
	}

	FORCEINLINE YLinearColor	operator*(float Scalar) const
	{
		return YLinearColor(
			this->R * Scalar,
			this->G * Scalar,
			this->B * Scalar,
			this->A * Scalar
		);
	}
	
	FORCEINLINE YLinearColor&	operator*=(float Scalar)
	{
		R *= Scalar;
		G *= Scalar;
		B *= Scalar;
		A *= Scalar;
		return *this;
	}

	FORCEINLINE YLinearColor	operator/(const YLinearColor& ColorB) const
	{
		return YLinearColor(
			this->R / ColorB.R,
			this->G / ColorB.G,
			this->B / ColorB.B,
			this->A / ColorB.A
		);
	}

	FORCEINLINE YLinearColor&	operator/=(const YLinearColor& ColorB)
	{
		R /= ColorB.R;
		G /= ColorB.G;
		B /= ColorB.B;
		A /= ColorB.A;
		return *this;
	}

	FORCEINLINE YLinearColor	GetClamped(float InMin = 0.0f, float InMax = 1.0f) const
	{
		YLinearColor Ret;

		Ret.R = YMath::Clamp(R, InMin, InMax);
		Ret.G = YMath::Clamp(G, InMin, InMax);
		Ret.B = YMath::Clamp(B, InMin, InMax);
		Ret.A = YMath::Clamp(A, InMin, InMax);

		return Ret;
	}

	FORCEINLINE bool operator==(const YLinearColor& ColorB) const
	{
		return this->R == ColorB.R && this->G == ColorB.G && this->B == ColorB.B && this->A == ColorB.A;
	}
	FORCEINLINE bool operator!=(const YLinearColor& Other) const
	{
		return this->R != Other.R || this->G != Other.G || this->B != Other.B || this->A != Other.A;
	}

	FORCEINLINE bool			Equals(const YLinearColor& ColorB, float Tolerance = KINDA_SMALL_NUMBER) const
	{
		return YMath::Abs(this->R - ColorB.R) < Tolerance && YMath::Abs(this->G - ColorB.G) < Tolerance && YMath::Abs(this->B - ColorB.B) < Tolerance && YMath::Abs(this->A - ColorB.A) < Tolerance;
	}

	CORE_API YLinearColor		CopyWithNewOpacity(float NewOpacicty) const
	{
		YLinearColor NewCopy = *this;
		NewCopy.A = NewOpacicty;
		return NewCopy;
	}

	/**
	* Converts byte hue-saturation-brightness to floating point red-green-blue.
	*/
	static CORE_API YLinearColor FGetHSV(uint8 H, uint8 S, uint8 V);

	/**
	* Makes a random but quite nice color.
	*/
	static CORE_API YLinearColor MakeRandomColor();

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static CORE_API YLinearColor MakeFromColorTemperature(float Temp);

	/**
	* Euclidean distance between two points.
	*/
	static inline float Dist(const YLinearColor &V1, const YLinearColor &V2)
	{
		return YMath::Sqrt(YMath::Square(V2.R - V1.R) + YMath::Square(V2.G - V1.G) + YMath::Square(V2.B - V1.B) + YMath::Square(V2.A - V1.A));
	}

	/**
	* Generates a list of sample points on a Bezier curve defined by 2 points.
	*
	* @param	ControlPoints	Array of 4 Linear Colors (vert1, controlpoint1, controlpoint2, vert2).
	* @param	NumPoints		Number of samples.
	* @param	OutPoints		Receives the output samples.
	* @return					Path length.
	*/
	//!!FIXME by zyx
	//static CORE_API float		EvaluateBezier(const YLinearColor* ControlPoints, int32 NumPoints, TArray<YLinearColor>& OutPoints);

	/** Converts a linear space RGB color to an HSV color */
	CORE_API YLinearColor		LinearRGBToHSV() const;

	/** Converts an HSV color to a linear space RGB color */
	CORE_API YLinearColor		HSVToLinearRGB() const;

	/**
	* Linearly interpolates between two colors by the specified progress amount.  The interpolation is performed in HSV color space
	* taking the shortest path to the new color's hue.  This can give better results than YLinearColor::Lerp(), but is much more expensive.
	* The incoming colors are in RGB space, and the output color will be RGB.  The alpha value will also be interpolated.
	*
	* @param	From		The color and alpha to interpolate from as linear RGBA
	* @param	To			The color and alpha to interpolate to as linear RGBA
	* @param	Progress	Scalar interpolation amount (usually between 0.0 and 1.0 inclusive)
	* @return	The interpolated color in linear RGB space along with the interpolated alpha value
	*/
	static CORE_API YLinearColor LerpUsingHSV(const YLinearColor& From, const YLinearColor& To, const float Progress);

	/** Quantizes the linear color and returns the result as a YColor.  This bypasses the SRGB conversion. */
	CORE_API YColor				Quantize() const;

	/** Quantizes the linear color and returns the result as a YColor with optional sRGB conversion and quality as goal. */
	CORE_API YColor				ToYColor(const bool bSRGB) const;

	/**
	* Returns a desaturated color, with 0 meaning no desaturation and 1 == full desaturation
	*
	* @param	Desaturation	Desaturation factor in range [0..1]
	* @return	Desaturated color
	*/
	CORE_API YLinearColor		Desaturate(float Desaturation) const;

	/** Computes the perceptually weighted luminance value of a color. */
	CORE_API float				ComputeLuminance() const;

	/**
	* Returns the maximum value in this color structure
	*
	* @return The maximum color channel value
	*/
	FORCEINLINE float			GetMax() const
	{
		return YMath::Max(YMath::Max(YMath::Max(R, G), B), A);
	}

	/** useful to detect if a light contribution needs to be rendered */
	bool IsAlmostBlack() const
	{
		return YMath::Square(R) < DELTA && YMath::Square(G) < DELTA && YMath::Square(B) < DELTA;
	}

	/**
	* Returns the minimum value in this color structure
	*
	* @return The minimum color channel value
	*/
	FORCEINLINE float GetMin() const
	{
		return YMath::Min(YMath::Min(YMath::Min(R, G), B), A);
	}

	FORCEINLINE float GetLuminance() const
	{
		return R * 0.3f + G * 0.59f + B * 0.11f;
	}

	//!!FIXME by zyx
	//YString ToString() const
	//{
	//	return YString::Printf(TEXT("(R=%f,G=%f,B=%f,A=%f)"), R, G, B, A);
	//}

	/**
	* Initialize this Color based on an YString. The String is expected to contain R=, G=, B=, A=.
	* The YLinearColor will be bogus when InitFromString returns false.
	*
	* @param InSourceString YString containing the color values.
	* @return true if the R,G,B values were read successfully; false otherwise.
	*/

	//!!FIXME by zyx
	//bool						InitFromString(const YString& InSourceString)
	//{
	//	R = G = B = 0.f;
	//	A = 1.f;

	//	// The initialization is only successful if the R, G, and B values can all be parsed from the string
	//	const bool bSuccessful = FParse::Value(*InSourceString, TEXT("R="), R) && FParse::Value(*InSourceString, TEXT("G="), G) && FParse::Value(*InSourceString, TEXT("B="), B);

	//	// Alpha is optional, so don't factor in its presence (or lack thereof) in determining initialization success
	//	FParse::Value(*InSourceString, TEXT("A="), A);

	//	return bSuccessful;
	//}

	// Common colors.	
	static CORE_API const YLinearColor White;
	static CORE_API const YLinearColor Gray;
	static CORE_API const YLinearColor Black;
	static CORE_API const YLinearColor Transparent;
	static CORE_API const YLinearColor Red;
	static CORE_API const YLinearColor Green;
	static CORE_API const YLinearColor Blue;
	static CORE_API const YLinearColor Yellow;
};

FORCEINLINE YLinearColor operator*(float Scalar, const YLinearColor& Color)
{
	return Color.operator*(Scalar);
}

struct YColor
{
public:
	// Variables.
#if PLATFORM_LITTLE_ENDIAN
#if _MSC_VER
	// Win32 x86
	union { struct { uint8 B, G, R, A; }; uint32 AlignmentDummy; };
#else
	// Linux x86, etc
	uint8 B GCC_ALIGN(4);
	uint8 G, R, A;
#endif
#else // PLATFORM_LITTLE_ENDIAN
	union { struct { uint8 A, R, G, B; }; uint32 AlignmentDummy; };
#endif

	uint32&						DWColor(void) { return *((uint32*)this); }
	const uint32&				DWColor(void) const { return *((uint32*)this); }

	// Constructors.
	FORCEINLINE					YColor() {}
	FORCEINLINE explicit		YColor(EForceInit)
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = G = B = A = 0;
	}
	FORCEINLINE					YColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = InR;
		G = InG;
		B = InB;
		A = InA;
	}

	FORCEINLINE explicit		YColor(uint32 InColor)
	{
		DWColor() = InColor;
	}

	// Serializer.
	//!!FIXME by zyx
	//friend YArchive& operator<< (YArchive &Ar, YColor &Color)
	//{
	//	return Ar << Color.DWColor();
	//}

	//bool Serialize(YArchive& Ar)
	//{
	//	Ar << *this;
	//	return true;
	//}

	// Operators.
	FORCEINLINE bool operator==(const YColor &C) const
	{
		return DWColor() == C.DWColor();
	}

	FORCEINLINE bool operator!=(const YColor& C) const
	{
		return DWColor() != C.DWColor();
	}

	FORCEINLINE void operator+=(const YColor& C)
	{
		R = (uint8)YMath::Min((int32)R + (int32)C.R, 255);
		G = (uint8)YMath::Min((int32)G + (int32)C.G, 255);
		B = (uint8)YMath::Min((int32)B + (int32)C.B, 255);
		A = (uint8)YMath::Min((int32)A + (int32)C.A, 255);
	}

	CORE_API YLinearColor		FromRGBE() const;

	/**
	* Creates a color value from the given hexadecimal string.
	*
	* Supported formats are: RGB, RRGGBB, RRGGBBAA, #RGB, #RRGGBB, #RRGGBBAA
	*
	* @param HexString - The hexadecimal string.
	* @return The corresponding color value.
	* @see ToHex
	*/
	//!!FIXME by zyx
	//static CORE_API YColor		FromHex(const YString& HexString);

	/**
	* Makes a random but quite nice color.
	*/
	static CORE_API YColor		MakeRandomColor();

	/**
	* Makes a color red->green with the passed in scalar (e.g. 0 is red, 1 is green)
	*/
	static CORE_API YColor		MakeRedToGreenColorFromScalar(float Scalar);

	/**
	* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
	*/
	static CORE_API YColor		MakeFromColorTemperature(float Temp);

	/**
	*	@return a new YColor based of this color with the new alpha value.
	*	Usage: const YColor& MyColor = FColorList::Green.WithAlpha(128);
	*/
	YColor						WithAlpha(uint8 Alpha) const
	{
		return YColor(R, G, B, Alpha);
	}

	/**
	* Reinterprets the color as a linear color.
	*
	* @return The linear color representation.
	*/
	FORCEINLINE YLinearColor	ReinterpretAsLinear() const
	{
		return YLinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
	}

	/**
	* Converts this color value to a hexadecimal string.
	*
	* The format of the string is RRGGBBAA.
	*
	* @return Hexadecimal string.
	* @see FromHex, ToString
	*/
	//!!FIXME by zyx
	//FORCEINLINE YString ToHex() const
	//{
	//	return YString::Printf(TEXT("%02X%02X%02X%02X"), R, G, B, A);
	//}

	/**
	* Converts this color value to a string.
	*
	* @return The string representation.
	* @see ToHex
	*/
	//!!FIXME by zyx
	//FORCEINLINE YString ToString() const
	//{
	//	return YString::Printf(TEXT("(R=%i,G=%i,B=%i,A=%i)"), R, G, B, A);
	//}

	/**
	* Initialize this Color based on an YString. The String is expected to contain R=, G=, B=, A=.
	* The YColor will be bogus when InitFromString returns false.
	*
	* @param	InSourceString	YString containing the color values.
	* @return true if the R,G,B values were read successfully; false otherwise.
	*/
	//!!FIXME by zyx
	//bool InitFromString(const YString& InSourceString)
	//{
	//	R = G = B = 0;
	//	A = 255;

	//	// The initialization is only successful if the R, G, and B values can all be parsed from the string
	//	const bool bSuccessful = FParse::Value(*InSourceString, TEXT("R="), R) && FParse::Value(*InSourceString, TEXT("G="), G) && FParse::Value(*InSourceString, TEXT("B="), B);

	//	// Alpha is optional, so don't factor in its presence (or lack thereof) in determining initialization success
	//	FParse::Value(*InSourceString, TEXT("A="), A);

	//	return bSuccessful;
	//}

	/**
	* Gets the color in a packed uint32 format packed in the order ARGB.
	*/
	FORCEINLINE uint32			ToPackedARGB() const
	{
		return (A << 24) | (R << 16) | (G << 8) | (B << 0);
	}

	/**
	* Gets the color in a packed uint32 format packed in the order ABGR.
	*/
	FORCEINLINE uint32			ToPackedABGR() const
	{
		return (A << 24) | (B << 16) | (G << 8) | (R << 0);
	}

	/**
	* Gets the color in a packed uint32 format packed in the order RGBA.
	*/
	FORCEINLINE uint32			ToPackedRGBA() const
	{
		return (R << 24) | (G << 16) | (B << 8) | (A << 0);
	}

	/**
	* Gets the color in a packed uint32 format packed in the order BGRA.
	*/
	FORCEINLINE uint32			ToPackedBGRA() const
	{
		return (B << 24) | (G << 16) | (R << 8) | (A << 0);
	}

	/** Some pre-inited colors, useful for debug code */
	static CORE_API const YColor White;
	static CORE_API const YColor Black;
	static CORE_API const YColor Transparent;
	static CORE_API const YColor Red;
	static CORE_API const YColor Green;
	static CORE_API const YColor Blue;
	static CORE_API const YColor Yellow;
	static CORE_API const YColor Cyan;
	static CORE_API const YColor Magenta;
	static CORE_API const YColor Orange;
	static CORE_API const YColor Purple;
	static CORE_API const YColor Turquoise;
	static CORE_API const YColor Silver;
	static CORE_API const YColor Emerald;

private:
	/**
	* Please use .ToYColor(true) on YLinearColor if you wish to convert from YLinearColor to YColor,
	* with proper sRGB conversion applied.
	*
	* Note: Do not implement or make public.  We don't want people needlessly and implicitly converting between
	* YLinearColor and YColor.  It's not a free conversion.
	*/
	explicit YColor(const YLinearColor& LinearColor);
};

FORCEINLINE uint32				GetTypeHash(const YColor& Color)
{
	return Color.DWColor();
}

//!!FIXME by zyx
//FORCEINLINE uint32 GetTypeHash(const YLinearColor& LinearColor)
//{
//	// Note: this assumes there's no padding in YLinearColor that could contain uncompared data.
//	return FCrc::MemCrc_DEPRECATED(&LinearColor, sizeof(YLinearColor));
//}
//

/** Computes a brightness and a fixed point color from a floating point color. */
extern CORE_API void			ComputeAndFixedColorAndIntensity(const YLinearColor& InLinearColor, YColor& OutColor, float& OutIntensity);

// These act like a POD
//!!FIXME by zyx
//template <> struct TIsPODType<YColor> { enum { Value = true }; };
//template <> struct TIsPODType<YLinearColor> { enum { Value = true }; };

/**
* Helper struct for a 16 bit 565 color of a DXT1/3/5 block.
*/
struct FDXTColor565
{
	/** Blue component, 5 bit. */
	uint16 B : 5;

	/** Green component, 6 bit. */
	uint16 G : 6;

	/** Red component, 5 bit */
	uint16 R : 5;
};


/**
* Helper struct for a 16 bit 565 color of a DXT1/3/5 block.
*/
struct FDXTColor16
{
	union
	{
		/** 565 Color */
		FDXTColor565 Color565;
		/** 16 bit entity representation for easy access. */
		uint16 Value;
	};
};


/**
* Structure encompassing single DXT1 block.
*/
struct FDXT1
{
	/** Color 0/1 */
	union
	{
		FDXTColor16 Color[2];
		uint32 Colors;
	};
	/** Indices controlling how to blend colors. */
	uint32 Indices;
};


/**
* Structure encompassing single DXT5 block
*/
struct FDXT5
{
	/** Alpha component of DXT5 */
	uint8	Alpha[8];
	/** DXT1 color component. */
	FDXT1	DXT1;
};


// Make DXT helpers act like PODs
//!!FIXME by zyx
//template <> struct TIsPODType<FDXT1> { enum { Value = true }; };
//template <> struct TIsPODType<FDXT5> { enum { Value = true }; };
//template <> struct TIsPODType<FDXTColor16> { enum { Value = true }; };
//template <> struct TIsPODType<FDXTColor565> { enum { Value = true }; };
