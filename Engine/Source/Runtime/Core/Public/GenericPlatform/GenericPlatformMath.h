#pragma once
//目前不太清楚UE是如何引进标准库的<math.h>的，这里直接加上了
#include <math.h>
#include <stdlib.h>

struct YGenericPlatformMath
{
	// Convert a float to an integer with truncation towards zero.
	// this usually a slow operation implement by ANSI C++,Depend on platform.
	static CONSTEXPR FORCEINLINE int32 TruncToInt(float F)
	{
		return (int32)F;
	}

	// Convert a float to a float with truncation towards zero.
	static CONSTEXPR FORCEINLINE float TruncToFloat(float F)
	{
		return (float)TruncToInt(F);
	}

	// Convert a float to the nearest less or equal integer.
	static FORCEINLINE int32	FloorToInt(float F)
	{
		return TruncToInt(floorf(F));
	}

	static FORCEINLINE float	FloorToFloat(float F)
	{
		return floorf(F);
	}

	static FORCEINLINE double	FloorToDouble(double F)
	{
		return floor(F);
	}

	// Convert a float to the nearest integer. Rounds up when the fraction is .5
	static FORCEINLINE int32	RoundToInt(float F)
	{
		return FloorToInt(F + 0.5f);
	}

	static FORCEINLINE float	RoundToFloat(float F)
	{
		return FloorToFloat(F + 0.5f);
	}

	static FORCEINLINE double	RoundToDouble(float F)
	{
		return FloorToDouble(F + 0.5f);
	}

	// Convert a float to the nearest geater or equal integer
	static FORCEINLINE int32	CeilToInt(float F)
	{
		return TruncToInt(ceilf(F));
	}

	static FORCEINLINE float	CeilToFloat(float F)
	{
		return ceilf(F);
	}

	static FORCEINLINE double	CeilToDouble(float F)
	{
		return ceil(F);
	}

	// Returns signed fractional part of a float
	// !!NOTE by zyx,如果输入参数是负的,返回值，ue注释是a float between >=0 and <1 for nonnegtive input,a float between >=-1 and <0 for negative input
	static FORCEINLINE float	Fractional(float Value)
	{
		return Value - TruncToFloat(Value);
	}

	// Returns the fractional part of a float,输入是负数时注意
	// return a float between >=0 and <1,
	static FORCEINLINE float	Frac(float Value)
	{
		return Value - FloorToFloat(Value);
	}

	// Breaks the given value into an integral and a fractional part
	static FORCEINLINE float	Modf(const float InValue, float* OutIntPart)
	{
		return modff(InValue, OutIntPart);
	}

	static FORCEINLINE double	Modf(const double InValue, double* OutIntPart)
	{
		return modf(InValue, OutIntPart);
	}

	// Returns e^Value
	static FORCEINLINE float	Exp(float Value) { return expf(Value); }
	// Returns 2^Value
	static FORCEINLINE float	Exp2(float Value) { return powf(2.f,Value); }
	static FORCEINLINE float	Loge(float Value) { return logf(Value); }
	static FORCEINLINE float	LogX(float Base,float Value) { return Loge(Value)/Loge(Base); }
	// 1.0 / loge(2) = 1.4426950f
	static FORCEINLINE float	Log2(float Value) { return Loge(Value)*1.4426950f; }

	// Returns the floating-point remainder of X / Y
	// Warning: Always returns remainder toward 0, not toward the smaller multiple of Y.
	// So for example Fmod(2.8f, 2) gives .8f as you would expect, however, Fmod(-2.8f, 2) gives - .8f, NOT 1.2f
	// Use Floor instead when snapping positions that can be negative to a grid
	static FORCEINLINE float	Fmod(float X, float Y)
	{
		if (fabsf(Y) <= 1.e-8f)
		{
			//FmodReportError(X, Y);
			return 0.f;
		}
		const float Quotient = TruncToFloat(X / Y);
		float IntPortion = Y*Quotient;
		// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
		// For example Fmod(55.8, 9.3) would result in a very small negative value!
		if (fabsf(IntPortion) > fabsf(X))
		{
			IntPortion = X;
		}
		const float	Result = X - IntPortion;
		return Result;
	}

	static FORCEINLINE float	Sin(float Value) { return sinf(Value); }
	static FORCEINLINE float	Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float	Cos(float Value) { return cosf(Value); }
	static FORCEINLINE float	Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float	Tan(float Value) { return tanf(Value); }
	static FORCEINLINE float	Atan(float Value) { return atanf(Value); }
	static CORE_API    float	Atan2(float Y, float X);
	static FORCEINLINE float	Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE float	Pow(float Base, float Exponent) { return powf(Base, Exponent); }

	// Compute a fully  accurate inverse square root
	static FORCEINLINE float	InvSqrt(float F)
	{
		return 1.f / sqrtf(F);
	}

	// Compute a faster but less accurate inverse squre root
	static FORCEINLINE float	InvSqrtEst(float F)
	{
		return InvSqrt(F);
	}
	// 指数域全为1&&尾数域不等于0
	static FORCEINLINE bool		IsNaN(float F)
	{
		return ((*(uint32*)&F) & 0x7FFFFFFF) > (uint32)0x7F800000;
	}
	// 指数域全为1 &&尾域全为0 为Inf
	static FORCEINLINE bool		IsFinite(float F)
	{
		return ((*(uint32*)&F) & 0x7FFFFFFF) != 0x7F800000;
	}
	static FORCEINLINE bool		IsNegativeFloat(float F)
	{
		return ((*(uint32*)&F) >= (uint32)0x80000000); // Detects sign bit.
	}

	static FORCEINLINE bool		IsNegativeDouble(double F)
	{
		return ((*(uint64*)&F) >= (uint64)0x8000000000000000); // Detects sign bit.
	}

	// Return a random integer [0,RAND_MAX]
	static FORCEINLINE int32	Rand() { return rand(); }

	// Seeds global random number functions Rand() and FRand()
	static FORCEINLINE void		RandInit(int32 Seed) { srand(Seed); }

	static FORCEINLINE float	FRand() { return Rand() / (float)RAND_MAX; }

	static CORE_API void		SRandInit(int32 Seed);

	static CORE_API int32		GetRandSeed();
	// Returns a seeded random float in the range[0, 1), using the seed from SRandInit()
	static CORE_API float		SRand();

	// Computes the base 2 logarithm for an integer value that is greater than 0
	static FORCEINLINE uint32	FloorLog2(uint32 Value)
	{
		// see http://codinggorilla.domemtech.com/?p=81 or http://en.wikipedia.org/wiki/Binary_logarithm but modified to return 0 for a input value of 0
		uint32 pos = 0;
		if (Value >= 1 << 16) { Value >>= 16; pos += 16; }
		if (Value >= 1 << 8) { Value >>= 8; pos += 8; }
		if (Value >= 1 << 4) { Value >>= 4; pos += 4; }
		if (Value >= 1 << 2) { Value >>= 2; pos += 2; }
		if (Value >= 1 << 1) { pos += 1; }
		return (Value == 0) ? 0 : pos;
	}

	// Counts the number of leading zeros in the bit representation of the value
	static FORCEINLINE uint32	CountLeadingZeros(uint32 Value)
	{
		if (Value == 0) return 32;
		return 31 - FloorLog2(Value);
	}

	// Counts the number of trailing zeros in the bit representation of the value
	static FORCEINLINE uint32	CountTrailingZeros(uint32 Value)
	{
		if (Value == 0)
		{
			return 32;
		}
		uint32 Result = 0;
		while ((Value & 1) == 0)
		{
			Value >>= 1;
			++Result;
		}
		return Result;
	}

	// Returns smallest N such that (1<<N)>=Arg.
	// !!Note by zyx, copy from UE, Don't known why
	static FORCEINLINE uint32	CeilLogTwo(uint32 Value)
	{
		int32 Bitmask = ((int32)(CountLeadingZeros(Value) << 26)) >> 31;
		return (32 - CountLeadingZeros(Value - 1)) & (~Bitmask);
	}

	// Rounds up to the next highest power of two
	static FORCEINLINE uint32	RoundUpToPowerOfTwo(uint32 Value)
	{
		return 1 << CeilLogTwo(Value);
	}

	static FORCEINLINE uint32	MortonCode2(uint32 x)
	{
		x &= 0x0000ffff;
		x = (x ^ (x << 8)) & 0x00ff00ff;
		x = (x ^ (x << 4)) & 0x0f0f0f0f;
		x = (x ^ (x << 2)) & 0x33333333;
		x = (x ^ (x << 1)) & 0x55555555;
		return x;
	}

	// Reverses MortonCode2. Compacts every other bit to the right. 
	static FORCEINLINE uint32	ReverseMortonCode2(uint32 x)
	{
		x &= 0x55555555;
		x = (x ^ (x >> 1)) & 0x33333333;
		x = (x ^ (x >> 2)) & 0x0f0f0f0f;
		x = (x ^ (x >> 4)) & 0x00ff00ff;
		x = (x ^ (x >> 8)) & 0x0000ffff;
		return x;
	}

	// Spreads bits to every 3rd. 
	static FORCEINLINE uint32	MortonCode3(uint32 x)
	{
		x &= 0x000003ff;
		x = (x ^ (x << 16)) & 0xff0000ff;
		x = (x ^ (x << 8)) & 0x0300f00f;
		x = (x ^ (x << 4)) & 0x030c30c3;
		x = (x ^ (x << 2)) & 0x09249249;
		return x;
	}

	// Reverses MortonCode3. Compacts every 3rd bit to the right. 
	static FORCEINLINE uint32	ReverseMortonCode3(uint32 x)
	{
		x &= 0x09249249;
		x = (x ^ (x >> 2)) & 0x030c30c3;
		x = (x ^ (x >> 4)) & 0x0300f00f;
		x = (x ^ (x >> 8)) & 0xff0000ff;
		x = (x ^ (x >> 16)) & 0x000003ff;
		return x;
	}

	/**
	* Returns value based on comparand. The main purpose of this function is to avoid
	* branching based on floating point comparison which can be avoided via compiler
	* intrinsics.
	*
	* Please note that we don't define what happens in the case of NaNs as there might
	* be platform specific differences.
	*
	* @param	Comparand		Comparand the results are based on
	* @param	ValueGEZero		Return value if Comparand >= 0
	* @param	ValueLTZero		Return value if Comparand < 0
	*
	* @return	ValueGEZero if Comparand >= 0, ValueLTZero otherwise
	*/
	static CONSTEXPR FORCEINLINE float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	/**
	* Returns value based on comparand. The main purpose of this function is to avoid
	* branching based on floating point comparison which can be avoided via compiler
	* intrinsics.
	*
	* Please note that we don't define what happens in the case of NaNs as there might
	* be platform specific differences.
	*
	* @param	Comparand		Comparand the results are based on
	* @param	ValueGEZero		Return value if Comparand >= 0
	* @param	ValueLTZero		Return value if Comparand < 0
	*
	* @return	ValueGEZero if Comparand >= 0, ValueLTZero otherwise
	*/
	static CONSTEXPR FORCEINLINE double FloatSelect(double Comparand, double ValueGEZero, double ValueLTZero)
	{
		return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
	}

	// Compute absolute value in a generic way
	template< class T >
	static CONSTEXPR FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	// Returns 1, 0, or -1 depending on relation of T to 0
	template< class T >
	static CONSTEXPR FORCEINLINE T Sign(const T A)
	{
		return (A > (T)0) ? (T)1 : ( (A< (T)0)? (T)-1 : (T) 0);
	}

	// Returns higher value in a generic way
	template< class T >
	static CONSTEXPR FORCEINLINE T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	// Returns lower value in a generic way
	template< class T> 
	static CONSTEXPR FORCEINLINE T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}

	// MIn of Array
	// !!FIX ME by zyx
	//template< class T>
	//static FORCEINLINE T Min(const TArray<T>& Values, int32* MinIndex = nullptr);

	// Max of Array
	// !!FIX ME by zyx
	//template< class T >
	//static FORCEINLINE T Max(const TArray<T>& Values, int32* MaxIndex = nullptr);
#if WITH_DEV_AUTOMATION_TESTS
	/** Test some of the tricky functions above **/
	static void AutoTest();
#endif
private:
	//  Error reporting for Fmod. Not inlined to avoid compilation issues and avoid all the checks and error reporting at all callsites. 
	static CORE_API void FmodReportError(float X, float Y);
};

template<>
FORCEINLINE float YGenericPlatformMath::Abs(const float A)
{
	return fabsf(A);
}

