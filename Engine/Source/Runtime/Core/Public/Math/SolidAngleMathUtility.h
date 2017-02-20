#pragma once

#include "HAL/Platform.h"
#include "HAL/PlatformMath.h"

#ifndef ENABLE_NAN_DIAGNOSTIC
#define ENABLE_NAN_DIAGNOSTIC 0
#endif

// Forward declarations.
struct  YVector;
struct  YVector4;
struct  YPlane;
struct  YBox;
struct  YRotator;
struct  YMatrix;
struct  YQuat;
struct  YTwoVectors;
struct  YTransform;
class	YSphere;
struct	YVector2D;
struct	YLinearColor;

#undef  PI
#define PI 						(3.1415926535897932f)
#define SMALL_NUMBER			(1.e-8f)
#define KINDA_SMALL_NUMBER		(1.e-4f)
#define BIG_NUMBER				(3.4e+38f)
#define EULERS_NUMBER			(2.71828182845904523536f)

// Copied from float.h
#define MAX_FLT					3.402823466e+38F

// Aux constants.
#define INV_PI					(0.31830988618f)
#define HALF_PI					(1.57079632679f)

// Magic numbers for numerical precision.
#define DELTA					(0.00001f)

/**
* Lengths of normalized vectors (These are half their maximum values
* to assure that dot products with normalized vectors don't overflow).
*/
#define FLOAT_NORMAL_THRESH				(0.0001f)

//
// Magic numbers for numerical precision.
//
#define THRESH_POINT_ON_PLANE			(0.10f)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20f)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.00002f)	/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015f)	/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002f)	/* Two normal points are same if within this distance */
/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25f)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01f)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.0001f)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_NORMALS_ARE_PARALLEL		(0.999845f)	/* Two unit vectors are parallel if abs(A dot B) is greater than or equal to this. This is roughly cosine(1.0 degrees). */
#define THRESH_NORMALS_ARE_ORTHOGONAL	(0.017455f)	/* Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less than or equal this. This is roughly cosine(89.0 degrees). */

#define THRESH_VECTOR_NORMALIZED		(0.01f)		/** Allowed error for a normalized vector (against squared magnitude) */
#define THRESH_QUAT_NORMALIZED			(0.01f)		/** Allowed error for a normalized quaternion (against squared magnitude) */

// Structure for all math helper functions, inherits from platform math to pick up platform-specific implementations
struct YMath :public YPlatformMath
{
	//Random Number Functions

	// Return a random number in [0,A)
	static FORCEINLINE int32	RandHelper(int32 A)
	{
		// Note that on some platforms RAND_MAX is a large number so we cannot do ((rand()/(RAND_MAX+1)) * A)
		// or else we may include the upper bound results, which should be excluded.
		return A > 0 ? Min(TruncToInt(FRand() * A), A - 1) : 0;
	}

	// Return a random number in [Min,Max]
	static FORCEINLINE int32	RandRange(int32 Min, int32 Max)
	{
		const int32 Range = (Max - Min) + 1;
		return Min + RandHelper(Range);
	}

	// Return a random float in [Min,Max]
	static FORCEINLINE float	RandRange(float InMin, float InMax)
	{
		return FRandRange(InMin, InMax);
	}
	
	// Return a random float in [Min,Max]
	static FORCEINLINE float	FRandRange(float InMin, float InMax)
	{
		return InMin + (InMax - InMin)* FRand();
	}

	static FORCEINLINE bool		RandBool()
	{
		return (RandRange(0, 1) == 1) ? true : false;
	}

	// Return a uniformly distributed random unit length vector = point on the unit sphere surface.
	static YVector				VRand();

	// Return a random unit vector, uniformly distributed, within the specified cone.
	// ConeHalfAngleRad is the half-angle of cone, in radians. Returns a normalized vector.
	static CORE_API YVector		VRandCone(YVector const& Dir, float ConeHalfAngleRad);

	// This is a version of VrandCone that handles "squished" cones, i.e. with different angle limits in the Y and Z axes.
	static CORE_API YVector		VRandCone(YVector const &Dir, float HorizontalConeHalfAngleRad, float VerticalConeHalfRad);

	// Returns a random point within the passed in bounding box.
	static CORE_API YVector		RandPointInBox(const YBox& Box);

	// Give a direction vector and a surface normal. return the vector reflected across the surface normal.
	// Produces a result like shining a laser at a mirror.
	// Direction: Direction Vector the ray is coming form.
	// SurfaceNormal: A normal of the surface the ray should be reflected on.
	// Returns Reflected vector
	static CORE_API YVector		GetReflectionVector(const YVector& Direction, const YVector & SurfaceNormal);

	// Predicates

	// Checks if value is within a range, exclusive on MaxValue[Min,Max)
	template< class T >
	static FORCEINLINE bool		IsWithin(const T& TestValue, const T& MinValue, const U& MaxValue)
	{
		return ((MinValue <= TestValue) && (TestValue < MaxValue));
	}

	// Checks if value is within a range, inclusive on MaxValue[Min,Max]
	template< class T >
	static FORCEINLINE bool		IsWithinInclusive(const T& TestValue, const T& MinValue, const T& MaxValue)
	{
		return ((MinValue <= TestValue) && (TestValue <= MaxValue));
	}

	// Checks if two floating point numbers are nearly equal
	static FORCEINLINE bool		IsNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<float>(A - B) <= ErrorTolerance;
	}
	
	// Checks if two floating point numbers are nearly equal
	static FORCEINLINE bool		IsNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER)
	{
		return Abs<double>(A - B) <= ErrorTolerance;
	}

	// Checks if a float point number is nearly zero
	static FORCEINLINE bool		IsNearlyZero(float Value, float ErrorTolerence = SMALL_NUMBER)
	{
		return Abs<float>(Value) <= ErrorTolerence;
	}

	// Checks if a float point number is nearly zero
	static FORCEINLINE bool		IsNearlyZero(double Value, double ErrorTolerence = SMALL_NUMBER)
	{
		return Abs<double>(Value) <= ErrorTolerence;
	}

	// Checks whether a number is a power of two
	template <typename T>
	static FORCEINLINE bool		IsPowerOfTwo(T Value)
	{
		return ((Value & (Value - 1)) == (T)0);
	}

	// Math Operations
	// Returns highest of 3 Values
	template < class T >
	static FORCEINLINE T		Max3(const T A, const T B, const T C)
	{
		return Max(Max(A, B), C);
	}

	// Returns lowest of 3 Values
	template < class T >
	static FORCEINLINE T		Min3(const T A, const T B, const T C)
	{
		return Min(Min(A, B), C);
	}

	template< class T>
	static FORCEINLINE T		Square(const T A)
	{
		return A*A;
	}

	template< class T>
	static FORCEINLINE T		Clamp(const T X, const T Min, const T Max)
	{
		return X < Min ? Min : X < Max ? X : Max;
	}

	// Snaps a value to the nearest grid multiple
	static FORCEINLINE float	GridSnap(float Location, float Grid)
	{
		if (Grid == 0.0f)
		{
			return Location;
		}
		else
		{
			return FloorToFloat((Location + 0.5f* Grid) / Grid)*Grid;
		}
	}

	// Snaps a value to the nearest grid multiple
	static FORCEINLINE float	Gridsnap(double Location,double Grid)
	{
		if (Grid == 0.0f)
		{
			return Location;
		}
		else
		{
			return FloorToFloat((Location + 0.5* Grid) / Grid)*Grid;
		}
	}

	template< class T>
	static FORCEINLINE T		DivideAndRoundUp(T Dividend, T Divisor)
	{
		return (Dividend + Dividend - 1) / Divisor;
	}

	template< class T>
	static FORCEINLINE T		DivideAndRoundDown(T Dividend, T Divisor)
	{
		return Dividend / Divisor;
	}

	// Compute the base 2 logarithm of the specified value
	static FORCEINLINE float	log2(float Value)
	{
		static const float LogToLog2 = 1.f / Loge(2.0f);
		return Loge(Value)* LogToLog2;
	}

	// Compute the sine and cosine of a scalar float
	static FORCEINLINE void		SinCos(float* ScalarSin, float* ScalarCos, float Value)
	{
		// Map Value to y in [ -pi,pi], x = 2*pi*quotient + remainder
		float quotient = (INV_PI * 0.5f)*Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient-0.5f));
		}
		float y = Value - (2.0f *PI)*quotient;
		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y*y;
		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign*p;
	}

	// 不明觉厉的魔数
	// Note:  We use FASTASIN_HALF_PI instead of HALF_PI inside of FastASin(), since it was the value that accompanied the minimax coefficients below.
	// It is important to use exactly the same value in all places inside this function to ensure that FastASin(0.0f) == 0.0f.
	// For comparison:
	//		HALF_PI				== 1.57079632679f == 0x3fC90FDB
	//		FASTASIN_HALF_PI	== 1.5707963050f  == 0x3fC90FDA
#define FASTASIN_HALF_PI (1.5707963050f)
	/**
	* Computes the ASin of a scalar float.
	*
	* @param Value  input angle
	* @return ASin of Value
	*/
	static FORCEINLINE float FastAsin(float Value)
	{
		// Clamp input to [-1,1].
		bool nonnegative = (Value >= 0.0f);
		float x = YMath::Abs(Value);
		float omx = 1.0f - x;
		if (omx < 0.0f)
		{
			omx = 0.0f;
		}
		float root = YMath::Sqrt(omx);
		// 7-degree minimax approximation
		float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
		result *= root;  // acos(|x|)
						 // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
		return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
	}
#undef FASTASIN_HALF_PI

	// Conversion Functions
	// Converts radians to degrees
	template<class T>
	static FORCEINLINE auto		RadiansToDegrees(T const & RadVal) ->decltype(RadVal* (180.f / PI))
	{
		return RadVal*(180.f / PI);
	}

	// Converts degrees to radians
	template<class T>
	static FORCEINLINE auto		DegreesToRadians(T const& DegVal) -> decltype(DegVal * (PI / 180.f))
	{
		return DegVal * (PI / 180.f);
	}

	//Clamps an arbitrary angle to be between the given angles.  Will clamp to nearest boundary.
	static float CORE_API		ClampAngle(float AngleDegrees, float MinAngleDegrees, float MaxAngleDegrees);

	//Find the smallest angle between two headings (in degrees)
	static FORCEINLINE float	FindDeltaAngleDegrees(float A1, float A2)
	{
		float Delta = A2 - A1;
		// If Change is larger than 180
		if(Delta > 180.0f)
		{
			Delta = Delta - 360.0f;
		}
		else if(Delta < -180.0f)
		{
			Delta = Delta + 360.0f;
		}
		// Return delta in [-180,180] range
		return Delta;
	}
	
	// Find the smallest angle between two headings( in radians)
	static FORCEINLINE float	FindDeltaAngleRadians(float A1, float A2)
	{
		// Find the difference
		float Delta = A2 - A1;
		if(Delta > PI)
		{
			Delta = Delta - (PI*2.0f);
		}
		else if(Delta < -PI)
		{
			Delta = Delta + (PI*2.0f);
		}
		// Return delta in [-PI,PI] range
		return Delta;
	}

	// Given a heading which may be outside the +/- PI range, 'unwind' it back into that range.
	static FORCEINLINE float	UnwindRadians(float A)
	{
		while (A > PI)
		{
			A -= ((float)PI * 2.0f);
		}
		
		while (A < -PI)
		{
			A += ((float)PI *2.0f);
		}

		return A;
	}

	// Given a heading which may be outside of the +/- 180 degree,'unwind'it back into that range
	static FORCEINLINE float	UnwindDegree(float A)
	{
		while (A > 180.0f)
		{
			A -= 360.0f;
		}

		while (A < -180.0f)
		{
			A += 360.0f;
		}

		return A;
	}

	// Given two angles in degrees, 'wind' the rotation in Angle1 so that it avoids >180 degree flips.
	// Good for winding rotations previously expressed as quaternions into a euler - angle representation.
	// Angle 0 : the first angle that we wind relative to.
	// Angle 1 : the second angle that we may wind relative to the first. 
	static CORE_API void		WindRelativeAnglesDegrees(float InAngle0, float& InOutAngle1);

	// Returns a new rotation component value
	// InCurrent is the current rotation value
	// InDesired is the desired rotation value
	// InDeltaRate is the rotation amount to apply
	// return a new rotation component value
	static CORE_API float		FixedTurn(float InCurrent, float InDesired, float InDeltaRate);

	// Converts given Cartesian coordinate pair to Polar coordinate system.
	static FORCEINLINE void		CartesianToPolar(const float X, const float Y, float &OutRad, float &OutAng)
	{
		OutRad = Sqrt(Square(X) + Square(Y));
		OutAng = Atan2(Y, X);
	}

	// Converts given Cartesian coordinate pair to Polar coordinate system.
	static FORCEINLINE void		CartesianToPolar(const YVector2D InCart, const YVector2D& OutPolar);
	 
	// Converts given Polar coordinate pair to Cartesian coordinate system
	static FORCEINLINE void		PolarToCartesion(const float Rad, const float Ang, float& OutX, float& OutY)
	{
		OutX = Rad* Cos(Ang);
		OutY = Rad* Cos(Rad);
	}

	// Converts given Polar coordinate pair to Cartesian coordinate system
	static FORCEINLINE void		PolarToCartesion(const YVector2D InPolar, YVector2D& OutCart);

	/**
	* Calculates the dotted distance of vector 'Direction' to coordinate system O(AxisX,AxisY,AxisZ).
	*
	* Orientation: (consider 'O' the first person view of the player, and 'Direction' a vector pointing to an enemy)
	* - positive azimuth means enemy is on the right of crosshair. (negative means left).
	* - positive elevation means enemy is on top of crosshair, negative means below.
	*
	* @Note: 'Azimuth' (.X) sign is changed to represent left/right and not front/behind. front/behind is the funtion's return value.
	*
	* @param	OutDotDist	.X = 'Direction' dot AxisX relative to plane (AxisX,AxisZ). (== Cos(Azimuth))
	*						.Y = 'Direction' dot AxisX relative to plane (AxisX,AxisY). (== Sin(Elevation))
	* @param	Direction	direction of target.
	* @param	AxisX		X component of reference system.
	* @param	AxisY		Y component of reference system.
	* @param	AxisZ		Z component of reference system.
	*
	* @return	true if 'Direction' is facing AxisX (Direction dot AxisX >= 0.f)
	*/
	static CORE_API bool		GetDotDistance(YVector& outDotDist, const YVector& Direction, const YVector& AxisX, const YVector& AxisY, const YVector& AxisZ);

	/**
	* Returns Azimuth and Elevation of vector 'Direction' in coordinate system O(AxisX,AxisY,AxisZ).
	*
	* Orientation: (consider 'O' the first person view of the player, and 'Direction' a vector pointing to an enemy)
	* - positive azimuth means enemy is on the right of crosshair. (negative means left).
	* - positive elevation means enemy is on top of crosshair, negative means below.
	*
	* @param	Direction		Direction of target.
	* @param	AxisX			X component of reference system.
	* @param	AxisY			Y component of reference system.
	* @param	AxisZ			Z component of reference system.
	*
	* @return	YVector2D	X = Azimuth angle (in radians) (-PI, +PI)
	*						Y = Elevation angle (in radians) (-PI/2, +PI/2)
	*/
	static CORE_API YVector2D	GetAzimuthAndElevation(const YVector& Direction, const YVector &AxisX, const YVector& AxisY, const YVector& AxisZ);

	// Interpolation Functions

	// Calculates the percentage along a line from MinValue to MaxValue that Value is.
	static FORCEINLINE float	GetRangePct(float MinValue, float MaxValue, float Value)
	{
		return (Value - MinValue) / (MaxValue - MinValue);
	}

	// Calculates the percentage along a line from MinValue to MaxValue that Value is, range is represented by YVector2
	static float				GetRangePct(YVector2D const & Range, float Value);

	// Basically a Vector2D Version of Lerp
	static float				GetRangeValue(YVector2D const& Range, float Pct);

	// For the given Value clamped to the [Input:Range] inclusive, returns the corresponding percentage in [Output:Range] Inclusive.
	static FORCEINLINE float	GetMappedRangeValueClamped(const YVector2D& InputRange, const YVector2D& OutputRange, const float Value)
	{
		const float ClampedPct = Clamp<float>(GetRangePct(InputRange, Value), 0.0f, 1.0f);
		return GetRangeValue(OutputRange, ClampedPct);
	}

	// Performs a linear interpolation between two values, Alpha ranges from 0-1
	template< class T, class U>
	static FORCEINLINE T		Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}

	// Performs a linear interpolation between two values, Alpha ranges from 0-1. Handles full numeric range of T 
	template<class T>
	static FORCEINLINE T		LerpStable(const T& A, const T& B, double Alpha)
	{
		return (T)((A*(1.0 - Alpha)) + (B*Alpha));
	}

	// Performs a linear interpolation between two values, Alpha ranges from 0-1. Handles full numeric range of T 
	template<class T>
	static FORCEINLINE T		LerpStable(const T& A, const T& B, float Alpha)
	{
		return (T)((A*(1.0f - Alpha)) + (B*Alpha));
	}

	// Performs a 2D linear interpolation between four values, FracX, FracY ranges form 0~1
	template< class T, class U>
	static FORCEINLINE T		BiLerp(const T& P00, const T& P10, const T& P01, const T& p11, const U& FracX, const U& FracY)
	{
		return Lerp(Lerp(P00, P10, FracX), Lerp(P01, P11, FracX), FracY);
	}

	// Performs a cubic interpolation
	// P :end Points
	// T :tangent directions at end points
	// A : distance along spline
	template< class T, class U>
	static FORCEINLINE T		CubicInterp(const T& P0, const T& T0, const T& P1, const T& T1, const U& Alpha)
	{
		const float A2 = Alpha*Alpha;
		const float A3 = A2*Alpha;
		return (T)(((2 * A3) - (3 * A2) + 1) * P0) + ((A3 - (2 * A2) + A) * T0) + ((A3 - A2) * T1) + (((-2 * A3) + (3 * A2)) * P1);
	}

	/**
	* Performs a first derivative cubic interpolation
	*
	* @param  P - end points
	* @param  T - tangent directions at end points
	* @param  Alpha - distance along spline
	*
	* @return  Interpolated value
	*/
	template< class T, class U >
	static FORCEINLINE T		CubicInterpDerivative(const T& P0, const T& T0, const T& P1, const T& T1, const U& A)
	{
		T a = 6.f*P0 + 3.f*T0 + 3.f*T1 - 6.f*P1;
		T b = -6.f*P0 - 4.f*T0 - 2.f*T1 + 6.f*P1;
		T c = T0;

		const float A2 = A  * A;

		return (a * A2) + (b * A) + c;
	}

	/**
	* Performs a second derivative cubic interpolation
	*
	* @param  P - end points
	* @param  T - tangent directions at end points
	* @param  Alpha - distance along spline
	*
	* @return  Interpolated value
	*/
	template< class T, class U >
	static FORCEINLINE T		CubicInterpSecondDerivative(const T& P0, const T& T0, const T& P1, const T& T1, const U& A)
	{
		T a = 12.f*P0 + 6.f*T0 + 6.f*T1 - 12.f*P1;
		T b = -6.f*P0 - 4.f*T0 - 2.f*T1 + 6.f*P1;

		return (a * A) + b;
	}
	
	// !!Note by zyx, 带In的都是凹的曲线，带Out的都是凸的曲线

	//Interpolate between A and B, applying an ease in function. Exp controls the degree of the curve.
	template< class T>
	static FORCEINLINE T		InterEaseIn(const T& A, const T& B, float Alpha, float Exp)
	{
		float const ModifiedAlpha = Pow(Alpha, Exp);
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolate between A and B, applying an ease out function. Exp controls the degree of the curve.
	template<class T>
	static FORCEINLINE T		InterpEaseOut(const T& A, const T& B, float Alpha, float Exp)
	{
		const float ModifiedAlpha = 1.0f - Pow(1.0f - Alpha, Exp);
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolate between A and B, applying an ease in/out function. ExpControls the degree of the curve
	template<class T>
	static FORCEINLINE T		InterpEaseInOut(const T& A, const T& B, float Alpha, float Exp)
	{
		return Lerp<T>(A, B, (Alpha < 0.5f) ?
			InterpEaseIn(0.f, 1.f, Alpha * 2.f, Exp) * 0.5f :
			InterpEaseOut(0.f, 1.f, Alpha * 2.f - 1.f, Exp) * 0.5f + 0.5f);
	}

	// Interpolate between A and B, applying a step function
	// 感觉这个函数有问题，除NumIntervals没看懂，觉得应该是除StempsAsFloat
	template< class T>
	static FORCEINLINE	T		InterStep(const T& A, const T& B, float Alpha, int32 Steps)
	{
		if (Steps <= 1 || Alpha <= 0)
		{
			return A;
		}
		else if (Alpha >= 1)
		{
			return B;
		}

		const float StempsAsFloat = static_cast<float>(Steps);
		const float NumIntervals = StempsAsFloat - 1.0f;
		const float ModifiedAlpha = FloorToFloat(Alpha*StempsAsFloat) / NumIntervals;
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying a sinusoidal in function
	template< class T>
	static FORCEINLINE T		InterpSinIn(const T& A, const T& B, float Alpha)
	{
		float const ModifiedAlpha = -1.0f *Cos(Alpha*HALF_PI) + 1.0f;
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying a sinusoidal out function

	template< class T> 
	static FORCEINLINE T		InterSinOut(const T& A, const T& B, float Alpha)
	{
		float const ModifiedAlpha = Sin(Alpha * HALF_PI);
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying a sinusoial in/out funciton
	template<class T>
	static FORCEINLINE T		InterpSinOut(const T& A, const T& B, float Alpha)
	{
		return Lerp<T>(A, B, (Alpha < 0.5f) ?
			InterpSinIn(0.f, 1.f, Alpha * 2.f) * 0.5f :
			InterpSinOut(0.f, 1.f, Alpha * 2.f - 1.f) * 0.5f + 0.5f);
	}

	// Interpolation between A and B, applying an exponential in function.
	template<class T>
	static FORCEINLINE T		InterpExpoIn(const T& A, const T& B, float Alpha)
	{
		float const ModifiedAlpha = (Alpha == 0.0f) ? 0.0f : Pow(2.0f, 10.0f *(Alpha - 1.0f));
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying an exponential out function.
	template<class T>
	static FORCEINLINE T		InterpExpoOut(const T& A, const T& B, float Alpha)
	{
		float const ModifiedAlpha = (Alpha == 1.0f) ? 1.0f : -Pow(2.0f, -10.0f * Alpha) + 1.0f;
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying an exponential in/out function.
	template< class T>
	static FORCEINLINE T		InterpExoInOut(const T& A, const T& B, float Alpha)
	{
		return Lerp<T>(A, B, (Alpha < 0.5f) ?
			InterpExpoIn(0.f, 1.f, Alpha * 2.f) * 0.5f :
			InterpExpoOut(0.f, 1.f, Alpha * 2.f - 1.f) * 0.5f + 0.5f);
	}

	// Interpolation between A and B, applying a circular in function. 
	template< class T >
	static FORCEINLINE T		InterpCircularIn(const T& A, const T& B, float Alpha)
	{
		float const ModifiedAlpha = -1.f * (Sqrt(1.f - Alpha * Alpha) - 1.f);
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying a circular out function. 
	template< class T >
	static FORCEINLINE T		InterpCircularOut(const T& A, const T& B, float Alpha)
	{
		Alpha -= 1.f;
		float const ModifiedAlpha = Sqrt(1.f - Alpha  * Alpha);
		return Lerp<T>(A, B, ModifiedAlpha);
	}

	// Interpolation between A and B, applying a circular in/out function.
	template< class T >
	static FORCEINLINE T		InterpCircularInOut(const T& A, const T& B, float Alpha)
	{
		return Lerp<T>(A, B, (Alpha < 0.5f) ?
			InterpCircularIn(0.f, 1.f, Alpha * 2.f) * 0.5f :
			InterpCircularOut(0.f, 1.f, Alpha * 2.f - 1.f) * 0.5f + 0.5f);
	}

	// Rotator specific interpolation
	template<class T>
	static YRotator				Lerp(const YRotator& A, const YRotator& B, const T& Alpha);

	// Quat-specific interpolation
	template<class T>
	static YQuat				Lerp(const YQuat& A, const YQuat& B, const T& Alpha);
	template<class T>
	static YQuat				BiLerp(const YQuat& P00, const YQuat& P10, const YQuat& P01, const YQuat& P11, float FracX, float FracY);

	// In the case of quaternions, we use a bezier like approach.
	// T - Actual 'control' orientations
	template<class T>
	static YQuat				CubicInterp(const YQuat& P0, const YQuat& T0, const YQuat& P1, const YQuat& T1, const T& A);

	/*
	*	Cubic Catmull-Rom Spline interpolation. Based on http://www.cemyuksel.com/research/catmullrom_param/catmullrom.pdf
	*	Curves are guaranteed to pass through the control points and are easily chained together.
	*	Equation supports abitrary parameterization. eg. Uniform=0,1,2,3 ; chordal= |Pn - Pn-1| ; centripetal = |Pn - Pn-1|^0.5
	*	P0 - The control point preceding the interpolation range.
	*	P1 - The control point starting the interpolation range.
	*	P2 - The control point ending the interpolation range.
	*	P3 - The control point following the interpolation range.
	*	T0-3 - The interpolation parameters for the corresponding control points.
	*	T - The interpolation factor in the range 0 to 1. 0 returns P1. 1 returns P2.
	*/
	template< class U > 
	static U					CubicCRSplineInterp(const U& P0, const U& P1, const U& P2, const U& P3, const float T0, const float T1, const float T2, const float T3, const float T);

	/* Same as CubicCRSplineInterp but with additional saftey checks. If the checks fail P1 is returned. **/
	template< class U > 
	static U					CubicCRSplineInterpSafe(const U& P0, const U& P1, const U& P2, const U& P3, const float T0, const float T1, const float T2, const float T3, const float T);

	//Special-case interpolation

	// Interpolate float from Current to Target with constant step
	static CORE_API float		VInterpConstantTo(const float& Current, const float& Target, float DeltaTime, float InterpSpeed);

	// Interpolate float from Current to Target. Scaled by distance to Target, so it has a strong start speed and ease out.
	static CORE_API float		VInterpTo(const float& Current, const float& Target, float DeltaTime, float InterpSpeed);


	// Interpolate a normal vector Current to Target, by interpolating the angle between those vectors with constant step
	static CORE_API YVector		VInterpNormalRotationTo(const YVector& Current, const YVector& Target, float DeltaTime, float RotationSpeedDegrees);

	// Interpolate vector from Current to Target with constant step
	static CORE_API YVector		VInterpConstantTo(const YVector& Current, const YVector& Target, float DeltaTime, float InterpSpeed);

	// Interpolate vector from Current to Target. Scaled by distance to Target, so it has a strong start speed and ease out.
	static CORE_API YVector		VInterpTo(const YVector& Current, const YVector& Target, float DeltaTime, float InterpSpeed);

	// Interpolate vector2D from Current to Target with constant step
	static CORE_API YVector2D	Vector2DInterpConstantTo(const YVector2D& Current, const YVector2D& Target, float DeltaTime, float InterpSpeed);

	// Interpolate vector2D from Current to Target. Scaled by distance to Target, so it has a strong start speed and ease out.
	static CORE_API YVector2D	VInterpTo(const YVector2D& Current, const YVector2D& Target, float DeltaTime, float InterpSpeed);

	// Interpolate rotator from Current to Target with constant step
	static CORE_API YRotator	VInterpConstantTo(const YRotator& Current, const YRotator& Target, float DeltaTime, float InterpSpeed);

	// Interpolate rotator from Current to Target. Scaled by distance to Target, so it has a strong start speed and ease out.
	static CORE_API YRotator	VInterpTo(const YRotator& Current, const YRotator& Target, float DeltaTime, float InterpSpeed);

	// Interpolate Linear Color from Current to Target. Scaled by distance to Target, so it has a strong start speed and ease out.
	static CORE_API YLinearColor	VInterpTo(const YRotator& Current, const YRotator& Target, float DeltaTime, float InterpSpeed);

	// Simple function to creat a pulsating scalar value
	// InCurrentTime:			Current absolute time
	// InPulsesPerSecond :		How Many full pules per second?
	// InPhase:					Optional Phase amount, between 0.0 and 1.0 (to sychronize pulses)
	// Return:					Pulsating Value (0.0~1.0)
	static FORCEINLINE float	MakePulsatingValue(const double InCurrentTime, const float InPulsesPerSecond, const float InPhase = 0.0f);

		// Geomerty intersection

		//b Find the intersection of an infinite line(defined by two point) and a plane.
		// Assumes that the line and plane do indeed intersect; you mush make sure they 
		// are NOT parallel before calling

	static YVector				LinePlaneIntersection(const YVector& Point1, const YVector& Ponint2, const YVector& PlaneOrigin, const YVector& PlaneNormal);
	static YVector				LinePlaneIntersection(const YVector& Point1, const YVector& Point2, const YPlane& Plane);

	// InOutScissorRect should be set to View.ViewRect before the call
	// return 0: light is not visible,
	// return 1: use scissor rect,
	// return 2: no scissor rect needed.
	static CORE_API uint32		ComputeProjectedSphereSissoreRect(struct YIntRect& InOutSissorRect, YVector SphereOrigin, float Radius, YVector ViewOrigin, const YMatrix& ViewMatrix, const YMatrix& ProjMatrix);

	// Determine if a plane and an AABB intersect
	// p:						the plane to test
	// AABB:					the AABB to text
	// return :					if collision occurs 
	static CORE_API bool		PlaneAABBIntersection(const YPlane& P, const YBox& AABB);

	// Perform a sphere vs box intersection test using Arvo's algorithm
	// UE的实现比较慢，有一种更快的方法，可以对比一下
	static bool					SphereAABBIntersection(const YVector& SphereCenter, const float RadiusSquared, const YBox& AABB);

	// Convert a sphere into a point plus radius squared for the test above
	static bool					SphereAABBIntersection(const YSphere& Sphere, const YBox& AABB);

	// Determines whether a point is inside a box
	static bool					PointBoxIntersection(const YVector& Point, const YBox& Box);

	// Determines whether a line intersects a box
	static bool					LineBoxIntersection(const YBox& Box, const YVector& Start, const YVector& End, const YVector& Direction);
	// Determines whether a line intersects a box. This overload avoid the need to do the reciprocal every time.
	static bool					LineBoxIntersection(const YBox& Box, const YVector& Start, const YVector& End, const YVector& Direction, const YVector& OneOverDirection);

	// Swept-Box vs Box test
	static CORE_API bool		LineExtentBoxIntersection(const YBox& inBox, const YVector& Start, const YVector& End, const YVector& Extent, YVector& HitLocation, YVector& HitNormal, float & HitTime);
	
	// Determins whether a line intersects a sphere
	static bool					LineSphereIntersection(const YVector& Start, const YVector& Dir, float Length, const YVector& Origin, float Radius);

	// Assume the cone tip is at 0,0,0 (means the SphereCenter is relative to the cone tip)
	// Return true: one and sphere do intersect
	// Return false: otherwise
	static CORE_API bool		SphereConeIntersection(const YVector& SphereCenter, float SphereRadius, const YVector& ConeAxis, float ConeAngleSin, float ConeAngleCos);

	// Find the point on line segment form LineStart to LineEnd which is closet to Point
	static CORE_API YVector		ClosetPointOnLine(const YVector& LineStart, const YVector& LineEnd, const YVector& Point);

	// Compute intersection point of three planes. Return 1 if Valid, 0 if infinite.
	static bool					IntersectPlant3(YVector& I, const YPlane& P1, const YPlane& P2, const YPlane& P3);

	// Compute intersection point and direction of line joining two planes
	// Return 1 if valid, 0 if infinite
	static bool					IntersectPlant2(YVector& I, YVector& D, const YPlane& P1, const YPlane& P2);

	// Calculates the distance of a given Point in world space to a give line,
	// defined by the vector coupe(Origin, Direction)
	// Point:					Point to check distance to Axis
	// Direction:				unit vector indicating the direction to check against
	// Origin:					point of reference used to calculate distance
	// OutClosestPoint:			optional point that represents the closest point project onto Axis
	// Return :					distance of Point from line defined by (Origin, Direction)
	static CORE_API float		PointDistToLine(const YVector& Point, const YVector& Line, const YVector& Origin, YVector &OutClosestPoint);
	static CORE_API float		PointDistToLine(const YVector& Point, const YVector& LIne, const YVector& Origin);

	// Return closest point on a segment to a given point
	// The idea is to project point on line formed by segment.
	// Then we see if the closest point on the line is outside of segment or inside.
	// Point:					point for which we find the closest point on the segment.
	// StartPoint:				StartPoint of Segment.
	// EndPoint:				EndPont of Segment.
	// Return:					point on the segment defined by (StartPoint, EndPoint) that is closest to Point.
	static CORE_API YVector		ClosestPointOnSegment(const YVector& Point, const YVector& StartPoint, const YVector& EndPoint);

	// YVector2D version of ClosestPointOnSegment.
	// Returns closest point on a segment to a given 2D point.
	// The idea is to project point on line formed by segment.
	// Then we see if the closest point on the line is outside of segment or inseide.
	// Point:					point for which we find the closest point on the segment.
	// StartPoint:				StartPoint of Segment.
	// EndPoint:				EndPoint of Segment.
	// Return:					point on the segment defined by (StartPoint, EndPoint) that is closest to Point.
	static CORE_API YVector2D	ClosestPointOnSegment(const YVector2D& Point, const YVector2D & StartPint, const YVector2D &EndPoint);

	// Return distance from a point to the closet point on a segment.
	// Point:					point to check distance for
	// StartPoint:				StartPoint of Segment
	// EndPoint:				EndPoint of Segment
	// Return:					closest distance from Point to segment defined by (StartPoint, EndPoint)
	static CORE_API float		PointDistToSegment(const YVector& Point, const YVector &StartPoint, const YVector &EndPoint);

	// Return square of the distance from a point to the closest point on a segemnt.
	// Point:					point to check distance for
	// StartPoint:				StartPoint of Segment
	// EndPoint:				EndPoint of Segment
	// Return:					square of closest distance from Point to segment defined by (StartPoint, EndPoint)
	static CORE_API float		PointDistToSegmentSquared(const YVector& Point, const YVector &StartPoint, const YVector &EndPoint);

	// Find closest points between 2 Segments
	// (A1, B1)					defines the first segment.
	// (A2, B2)					defines the second segment.
	// OutP1					Closest point on segment 1 to segment 2.
	// OutP2					Closest point on segment 2 to segment 1.
	static CORE_API void		SegmentDistToSegment(YVector A1, YVector B1, YVector A2, YVector B2, YVector& OutP1, YVector& OutP2);

	// Find closest points between 2 Segments
	// (A1, B1)					defines the first segment.
	// (A2, B2)					defines the second segment.
	// OutP1					Closest point on segment 1 to segment 2.
	// OutP2					Closest point on segment 2 to segment 1.
	static CORE_API void		SegmentDistToSegmentSafe(YVector A1, YVector B1, YVector A2, YVector B2, YVector& OutP1, YVector& OutP2);

	// returns the time (t) of the intersection of the passed segment and a plane (could be <0 or >1)
	// StartPoint:				Start point of segment
	// EndPoint:				End point of segment
	// Plane:					plane to intersect with
	static CORE_API float		GetTForSegmentPlaneIntersect(const YVector& StartPoint, const YVector& EndPoint, const YVector& Plane);

	// Returns true if this is an intersection between the segment specified by StartPoint, and EndPoint
	// And the Plane on which polygon Plane lies. If there is an intersection, the point is placed in out_IntersectionPoint
	// StartPoint:				start point of segment
	// EndPoint:				end point of segment
	// Plane:					plane to intersect with
	// OutIntersectionPoint:	out var for the point on the segment that intersects the mesh(if any)
	// Return true:				if intersection occured.
	static CORE_API float		SegmentIntersection2D(const YVector& SegmentStartA, const YVector& SegmentEndA, const YVector& SegmentStartB, const YVector& SegmentEndB, YVector& outIntersectionPoint);

	// Return closest point on a triangle to a point
	// The idea is to identify the halfplanes that the point is
	// in relative to each triangle segment "plane"
	// Point:					point to check distance for
	// A,B,C					counter clockwise ordering of points defining a triangle
	static CORE_API YVector		ClosestPointOnTriangleToPoint(const YVector& Point, const YVector& A, const YVector& B, const YVector& C);

	// Find closest point on a Sphere to a Line.
	// When line intersects Sphere, then closest point to LineOrigin is returned.
	// SphereOrigin				Origin of Sphere
	// SphereRadius				Radius of Sphere
	// LineOrigin				Origin of line
	// LineDir					Direction of line. Needs to be normalized!!
	// OutClosestPoint			Closest point on sphere to given line.
	static CORE_API void		SphereDistToLine(YVector SphereOrigin, float SphereRaius, YVector LineDirNormalized, YVector& OutClosestPoint);

	// Calculates whether a Point is within a cone segment, and also what percentage within the cone(100 % is along the center line, whereas 0 % is along the edge)
	// Point:					The Point in question
	// ConeStartPoint:			the beginning of the cone(with the smallest radius)
	// ConeLine:				the line out from the start point that ends at the largest radius point of the cone
	// radiusAtStart:			the radius at the ConeStartPoint(0 for a 'proper' cone)
	// radiusAtEnd:				the largest radius of the cone
	// percentageOut:			output variable the holds how much within the cone the point is(1 = on center line, 0 = on exact edge or outside cone).
	// return true:				if the point is within the cone, false otherwise.
	static CORE_API void		GetDistanceWithinConeSegment(YVector Point, YVector ConeStartPoint, YVector ConeLine, float RadiusAtStart, float RadiusAtEnd, float & PercentageOut);
	
	// Determines whether a given set of points are coplanar, with a tolerance.Any three points or less are always coplanar.
	// Points:					The set of points to determine coplanarity for.
	// Tolerance:				Larger numbers means more variance is allowed.
	// return:					Whether the points are relatively coplanar, based on the tolerance
	// !!FIXME by zyx, do not have containers now
	//static CORE_API void		PointsAreCoplanner(const YArray<YVector> &Points, const float Tolerance = 0.1f);

	// Converts a floating point number to the nearest integer, equidistant ties go to the value which is closest to an even value : 1.5 becomes 2, 0.5 becomes 0
	// F:						Floating point value to convert
	// return:					The rounded integer
	static CORE_API float		RoundHalfToEvent(float F);
	static CORE_API double		RoundHalfToEvent(double F);

	// Converts a floating point number to an integer which is further from zero, "larger" in absolute value : 0.1 becomes 1, -0.1 becomes - 1
	// F:						Floating point value to convert
	// return:					The rounded integer
	static FORCEINLINE float	RoundFromZero(float F)
	{
		return (F < 0.0f) ? FloorToFloat(F) : CeilToFloat(F);
	}

	static FORCEINLINE double	RoundFromZero(double F)
	{
		return (F < 0.0f) ? FloorToDouble(F) : CeilToDouble(F);
	}

	// Converts a floating point number to an integer which is closer to zero, "smaller" in absolute value : 0.1 becomes 0, -0.1 becomes 0
	// F:						Floating point value to convert
	// return:					The rounded integer

	static FORCEINLINE float	RoundToZero(float F)
	{
		return (F < 0.0f) ? CeilToFloat(F) : FloorToFloat(F);
	}

	static FORCEINLINE double	RoundToZero(double F)
	{
		return (F < 0.0f) ? CeilToDouble(F) : FloorToDouble(F);
	}

	// Converts a floating point number to an integer which is more negative : 0.1 becomes 0, -0.1 becomes - 1
	// F:						Floating point value to convert
	// return:					The rounded integer
	static FORCEINLINE float	RoundToNegativeInfinity(float F)
	{
		return FloorToFloat(F);
	}

	static FORCEINLINE double	RoundToNegativeInfinity(double F)
	{
		return FloorToDouble(F);
	}

	// Converts a floating point number to an integer which is more positive: 0.1 becomes 1, -0.1 becomes 0
	// F:						Floating point value to convert
	// return:					The rounded integer
	static FORCEINLINE float RoundToPositiveInfinity(float F)
	{
		return CeilToFloat(F);
	}

	static FORCEINLINE double RoundToPositiveInfinity(double F)
	{
		return CeilToDouble(F);
	}

	// Formatting functions
	// Formats an integer value into a human readable string(i.e. 12345 becomes "12,345")
	// Val:						The value to use
	// return: FString			The human readable string 
	// !!FIXME by zyx, do not have YString types
	//static CORE_API YString		FormatIntToHumanReadable(int32 Val);

	// Utilities
	// Tests a memory region to see that it's working properly.
	// BaseAddress:				Starting address
	// NumBytes:				Number of bytes to test(will be rounded down to a multiple of 4)
	// return: true				if the memory region passed the test
	static CORE_API bool		MemoryTest(void* BaseAddress, uint32 NumBytes);

	// Evaluates a numerical equation.
	// Operators and precedence: 1 : +-2 : / % 3 : *4 : ^ 5 : &|
	// Unary : -
	// Types : Numbers(0 - 9.), Hex($0 - $f)
	// Grouping : ()
	// Str:						String containing the equation.
	// OutValue:				Pointer to storage for the result.
	// return					1 if successful, 0 if equation fails.
	// !!FIXME by zyx, do not have YString types
	//static CORE_API bool		Eval(YString Str, float& OutValue);

	// Computes the barycentric coordinates for a given point in a triangle - simpler version
	// Point:					point to convert to barycentric coordinates(in plane of ABC)
	// A, B, C:					three non - colinear points defining a triangle in CCW
	//return Vector:			containing the three weights a, b, c such that Point = a*A + b*B + c*C
	//	or Point = A + b*(B - A) + c*(C - A) = (1 - b - c)*A + b*B + c*C
	static CORE_API YVector		GetBaryCentric2D(const YVector& Point, const YVector& A, const YVector& B, const YVector& C);

	// Computes the barycentric coordinates for a given point in a triangle 
	// Point:					point to convert to barycentric coordinates(in plane of ABC)
	// A, B, C:					three non - colinear points defining a triangle in CCW
	//return Vector:			containing the three weights a, b, c such that Point = a*A + b*B + c*C
	//	or Point = A + b*(B - A) + c*(C - A) = (1 - b - c)*A + b*B + c*C
	static CORE_API YVector		ComputeBaryCentric2D(const YVector& Point, const YVector& A, const YVector& B, const YVector& C);

	// Computes the barycentric coordinates for a given point on a tetrahedron(3D)
	// Point:					point to convert to barycentric coordinates
	// A, B, C, D:				four points defining a tetrahedron
	// return Vector:			 containing the four weights a, b, c, d such that Point = a*A + b*B + c*C + d*D
	static CORE_API YVector4	ComputeBaryCentric3D(const YVector& Point, const YVector& A, const YVector& B, const YVector& C, const YVector& D);

	// Returns a smooth Hermite interpolation between 0 and 1 for the value X(where X ranges between A and B)
	// Clamped to 0 for X <= A and 1 for X >= B.
	// A:						Minimum value of X
	// B:						Maximum value of X
	// X:						Parameter
	// return:					Smoothed value between 0 and 1
	static float				SmoothStep(float A, float B, float X)
	{
		if (X < A)
		{
			return 0.0f;
		}
		else if (X >= B)
		{
			return 1.0f;
		}
		const float InterpFraction = (X - A) / (B - A);
		return InterpFraction * InterpFraction * (3.0f - 2.0f * InterpFraction);
	}

	/** 32 bit values where BitFlag[x] == (1<<x) */
	static CORE_API const uint32 BitFlag[32];

	// Get a bit in memory created from bitflags(uint32 Value : 1), used for EngineShowFlags,
	// TestBitFieldFunctions() tests the implementation
	static FORCEINLINE bool		ExtractBoolFromBitField(uint8* Ptr, uint32 Index)
	{
		uint8* BytePtr = Ptr + Index / 8;
		uint8 Mask = 1 << (Index & 0x7);
		return (*BytePtr & Mask) != 0;
	}
};
