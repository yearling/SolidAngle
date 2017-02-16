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

};