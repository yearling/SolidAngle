// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SolidAngleMathUtility.h"
#include "Vector2D.h"

/**
* A vector in 3-D space composed of components (X, Y, Z) with floating point precision.
*/
struct YVector
{
public:

	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

	/** Vector's Z component. */
	float Z;

public:

	/** A zero vector (0,0,0) */
	static CORE_API const YVector ZeroVector;

	/** World up vector (0,0,1) */
	static CORE_API const YVector UpVector;

	/** Unreal forward vector (1,0,0) */
	static CORE_API const YVector ForwardVector;

	/** Unreal right vector (0,1,0) */
	static CORE_API const YVector RightVector;

public:

#if ENABLE_NAN_DIAGNOSTIC
	FORCEINLINE void DiagnosticCheckNaN() const
	{
		if (ContainsNaN())
		{
			logOrEnsureNanError(TEXT("YVector contains NaN: %s"), *ToString());
			*const_cast<YVector*>(this) = ZeroVector;
		}
	}

	FORCEINLINE void DiagnosticCheckNaN(const TCHAR* Message) const
	{
		if (ContainsNaN())
		{
			logOrEnsureNanError(TEXT("%s: YVector contains NaN: %s"), Message, *ToString());
			*const_cast<YVector*>(this) = ZeroVector;
		}
	}
#else
	FORCEINLINE void DiagnosticCheckNaN() const {}
	FORCEINLINE void DiagnosticCheckNaN(const TCHAR* Message) const {}
#endif

	/** Default constructor (no initialization). */
	FORCEINLINE YVector();

	/**
	* Constructor initializing all components to a single float value.
	*
	* @param InF Value to set all components to.
	*/
	explicit FORCEINLINE		YVector(float InF);

	/**
	* Constructor using initial values for each component.
	*
	* @param InX X Coordinate.
	* @param InY Y Coordinate.
	* @param InZ Z Coordinate.
	*/
	FORCEINLINE					YVector(float InX, float InY, float InZ);

	/**
	* Constructs a vector from an YVector2D and Z value.
	*
	* @param V Vector to copy from.
	* @param InZ Z Coordinate.
	*/
	explicit FORCEINLINE		YVector(const YVector2D V, float InZ);

	/**
	* Constructor using the XYZ components from a 4D vector.
	*
	* @param V 4D Vector to copy from.
	*/
	FORCEINLINE					YVector(const YVector4& V);

	/**
	* Constructs a vector from an YLinearColor.
	*
	* @param InColor Color to copy from.
	*/
	explicit					YVector(const YLinearColor& InColor);

	/**
	* Constructs a vector from an YIntVector.
	*
	* @param InVector YIntVector to copy from.
	*/
	explicit					YVector(YIntVector InVector);

	/**
	* Constructs a vector from an YIntPoint.
	*
	* @param A Int Point used to set X and Y coordinates, Z is set to zero.
	*/
	explicit					YVector(YIntPoint A);

	/**
	* Constructor which initializes all components to zero.
	*
	* @param EForceInit Force init enum
	*/
	explicit FORCEINLINE		YVector(EForceInit);

#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
	/**
	* Copy another YVector into this one
	*
	* @param Other The other vector.
	* @return Reference to vector after copy.
	*/
	FORCEINLINE YVector& operator=(const YVector& Other);
#endif

	/**
	* Calculate cross product between this and another vector.
	*
	* @param V The other vector.
	* @return The cross product.
	*/
	FORCEINLINE YVector operator^(const YVector& V) const;

	/**
	* Calculate the cross product of two vectors.
	*
	* @param A The first vector.
	* @param B The second vector.
	* @return The cross product.
	*/
	FORCEINLINE static YVector CrossProduct(const YVector& A, const YVector& B);

	/**
	* Calculate the dot product between this and another vector.
	*
	* @param V The other vector.
	* @return The dot product.
	*/
	FORCEINLINE float operator|(const YVector& V) const;

	/**
	* Calculate the dot product of two vectors.
	*
	* @param A The first vector.
	* @param B The second vector.
	* @return The dot product.
	*/
	FORCEINLINE static float DotProduct(const YVector& A, const YVector& B);

	/**
	* Gets the result of component-wise addition of this and another vector.
	*
	* @param V The vector to add to this.
	* @return The result of vector addition.
	*/
	FORCEINLINE YVector operator+(const YVector& V) const;

	/**
	* Gets the result of component-wise subtraction of this by another vector.
	*
	* @param V The vector to subtract from this.
	* @return The result of vector subtraction.
	*/
	FORCEINLINE YVector operator-(const YVector& V) const;

	/**
	* Gets the result of subtracting from each component of the vector.
	*
	* @param Bias How much to subtract from each component.
	* @return The result of subtraction.
	*/
	FORCEINLINE YVector operator-(float Bias) const;

	/**
	* Gets the result of adding to each component of the vector.
	*
	* @param Bias How much to add to each component.
	* @return The result of addition.
	*/
	FORCEINLINE YVector operator+(float Bias) const;

	/**
	* Gets the result of scaling the vector (multiplying each component by a value).
	*
	* @param Scale What to multiply each component by.
	* @return The result of multiplication.
	*/
	FORCEINLINE YVector operator*(float Scale) const;

	/**
	* Gets the result of dividing each component of the vector by a value.
	*
	* @param Scale What to divide each component by.
	* @return The result of division.
	*/
	YVector operator/(float Scale) const;

	/**
	* Gets the result of component-wise multiplication of this vector by another.
	*
	* @param V The vector to multiply with.
	* @return The result of multiplication.
	*/
	FORCEINLINE YVector operator*(const YVector& V) const;

	/**
	* Gets the result of component-wise division of this vector by another.
	*
	* @param V The vector to divide by.
	* @return The result of division.
	*/
	FORCEINLINE YVector operator/(const YVector& V) const;

	// Binary comparison operators.

	/**
	* Check against another vector for equality.
	*
	* @param V The vector to check against.
	* @return true if the vectors are equal, false otherwise.
	*/
	bool operator==(const YVector& V) const;

	/**
	* Check against another vector for inequality.
	*
	* @param V The vector to check against.
	* @return true if the vectors are not equal, false otherwise.
	*/
	bool operator!=(const YVector& V) const;

	/**
	* Check against another vector for equality, within specified error limits.
	*
	* @param V The vector to check against.
	* @param Tolerance Error tolerance.
	* @return true if the vectors are equal within tolerance limits, false otherwise.
	*/
	bool Equals(const YVector& V, float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether all components of this vector are the same, within a tolerance.
	*
	* @param Tolerance Error tolerance.
	* @return true if the vectors are equal within tolerance limits, false otherwise.
	*/
	bool						AllComponentsEqual(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Get a negated copy of the vector.
	*
	* @return A negated copy of the vector.
	*/
	FORCEINLINE YVector operator-() const;

	/**
	* Adds another vector to this.
	* Uses component-wise addition.
	*
	* @param V Vector to add to this.
	* @return Copy of the vector after addition.
	*/
	FORCEINLINE YVector operator+=(const YVector& V);

	/**
	* Subtracts another vector from this.
	* Uses component-wise subtraction.
	*
	* @param V Vector to subtract from this.
	* @return Copy of the vector after subtraction.
	*/
	FORCEINLINE YVector operator-=(const YVector& V);

	/**
	* Scales the vector.
	*
	* @param Scale Amount to scale this vector by.
	* @return Copy of the vector after scaling.
	*/
	FORCEINLINE YVector operator*=(float Scale);

	/**
	* Divides the vector by a number.
	*
	* @param V What to divide this vector by.
	* @return Copy of the vector after division.
	*/
	YVector operator/=(float V);

	/**
	* Multiplies the vector with another vector, using component-wise multiplication.
	*
	* @param V What to multiply this vector with.
	* @return Copy of the vector after multiplication.
	*/
	YVector operator*=(const YVector& V);

	/**
	* Divides the vector by another vector, using component-wise division.
	*
	* @param V What to divide vector by.
	* @return Copy of the vector after division.
	*/
	YVector operator/=(const YVector& V);

	/**
	* Gets specific component of the vector.
	*
	* @param Index the index of vector component
	* @return reference to component.
	*/
	float& operator[](int32 Index);

	/**
	* Gets specific component of the vector.
	*
	* @param Index the index of vector component
	* @return Copy of the component.
	*/
	float operator[](int32 Index)const;

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component required.
	*
	* @return Reference to the specified component.
	*/
	float&						Component(int32 Index);

	/**
	* Gets a specific component of the vector.
	*
	* @param Index The index of the component required.
	* @return Copy of the specified component.
	*/
	float						Component(int32 Index) const;

public:

	// Simple functions.

	/**
	* Set the values of the vector directly.
	*
	* @param InX New X coordinate.
	* @param InY New Y coordinate.
	* @param InZ New Z coordinate.
	*/
	void						Set(float InX, float InY, float InZ);

	/**
	* Get the maximum value of the vector's components.
	*
	* @return The maximum value of the vector's components.
	*/
	float						GetMax() const;

	/**
	* Get the maximum absolute value of the vector's components.
	*
	* @return The maximum absolute value of the vector's components.
	*/
	float						GetAbsMax() const;

	/**
	* Get the minimum value of the vector's components.
	*
	* @return The minimum value of the vector's components.
	*/
	float						GetMin() const;

	/**
	* Get the minimum absolute value of the vector's components.
	*
	* @return The minimum absolute value of the vector's components.
	*/
	float						GetAbsMin() const;

	/** Gets the component-wise min of two vectors. */
	YVector						ComponentMin(const YVector& Other) const;

	/** Gets the component-wise max of two vectors. */
	YVector						ComponentMax(const YVector& Other) const;

	/**
	* Get a copy of this vector with absolute value of each component.
	*
	* @return A copy of this vector with absolute value of each component.
	*/
	YVector						GetAbs() const;

	/**
	* Get the length (magnitude) of this vector.
	*
	* @return The length of this vector.
	*/
	float						Size() const;

	/**
	* Get the squared length of this vector.
	*
	* @return The squared length of this vector.
	*/
	float						SizeSquared() const;

	/**
	* Get the length of the 2D components of this vector.
	*
	* @return The 2D length of this vector.
	*/
	float						Size2D() const;

	/**
	* Get the squared length of the 2D components of this vector.
	*
	* @return The squared 2D length of this vector.
	*/
	float						SizeSquared2D() const;

	/**
	* Checks whether vector is near to zero within a specified tolerance.
	*
	* @param Tolerance Error tolerance.
	* @return true if the vector is near to zero, false otherwise.
	*/
	bool						IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Checks whether all components of the vector are exactly zero.
	*
	* @return true if the vector is exactly zero, false otherwise.
	*/
	bool						IsZero() const;

	/**
	* Normalize this vector in-place if it is large enough, set it to (0,0,0) otherwise.
	*
	* @param Tolerance Minimum squared length of vector for normalization.
	* @return true if the vector was normalized correctly, false otherwise.
	*/
	bool						Normalize(float Tolerance = SMALL_NUMBER);

	/**
	* Checks whether vector is normalized.
	*
	* @return true if Normalized, false otherwise.
	*/
	bool						IsNormalized() const;

	/**
	* Utility to convert this vector into a unit direction vector and its original length.
	*
	* @param OutDir Reference passed in to store unit direction vector.
	* @param OutLength Reference passed in to store length of the vector.
	*/
	void						ToDirectionAndLength(YVector &OutDir, float &OutLength) const;

	/**
	* Get a copy of the vector as sign only.
	* Each component is set to +1 or -1, with the sign of zero treated as +1.
	*
	* @param A copy of the vector with each component set to +1 or -1
	*/
	FORCEINLINE YVector			GetSignVector() const;

	/**
	* Projects 2D components of vector based on Z.
	*
	* @return Projected version of vector based on Z.
	*/
	YVector						Projection() const;

	/**
	* Calculates normalized version of vector without checking for zero length.
	*
	* @return Normalized version of vector.
	* @see GetSafeNormal()
	*/
	FORCEINLINE YVector			GetUnsafeNormal() const;

	/**
	* Gets a copy of this vector snapped to a grid.
	*
	* @param GridSz Grid dimension.
	* @return A copy of this vector snapped to a grid.
	* @see YMath::GridSnap()
	*/
	YVector						GridSnap(const float& GridSz) const;

	/**
	* Get a copy of this vector, clamped inside of a cube.
	*
	* @param Radius Half size of the cube.
	* @return A copy of this vector, bound by cube.
	*/
	YVector						BoundToCube(float Radius) const;

	/** Create a copy of this vector, with its magnitude clamped between Min and Max. */
	YVector						GetClampedToSize(float Min, float Max) const;

	/** Create a copy of this vector, with the 2D magnitude clamped between Min and Max. Z is unchanged. */
	YVector						GetClampedToSize2D(float Min, float Max) const;

	/** Create a copy of this vector, with its maximum magnitude clamped to MaxSize. */
	YVector						GetClampedToMaxSize(float MaxSize) const;

	/** Create a copy of this vector, with the maximum 2D magnitude clamped to MaxSize. Z is unchanged. */
	YVector						GetClampedToMaxSize2D(float MaxSize) const;

	/**
	* Add a vector to this and clamp the result in a cube.
	*
	* @param V Vector to add.
	* @param Radius Half size of the cube.
	*/
	void						AddBounded(const YVector& V, float Radius = MAX_int16);

	/**
	* Gets the reciprocal of this vector, avoiding division by zero.
	* Zero components are set to BIG_NUMBER.
	*
	* @return Reciprocal of this vector.
	*/
	YVector						Reciprocal() const;

	/**
	* Check whether X, Y and Z are nearly equal.
	*
	* @param Tolerance Specified Tolerance.
	* @return true if X == Y == Z within the specified tolerance.
	*/
	bool						IsUniform(float Tolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Mirror a vector about a normal vector.
	*
	* @param MirrorNormal Normal vector to mirror about.
	* @return Mirrored vector.
	*/
	YVector						MirrorByVector(const YVector& MirrorNormal) const;

	/**
	* Mirrors a vector about a plane.
	*
	* @param Plane Plane to mirror about.
	* @return Mirrored vector.
	*/
	YVector						MirrorByPlane(const YPlane& Plane) const;

	/**
	* Rotates around Axis (assumes Axis.Size() == 1).
	*
	* @param Angle Angle to rotate (in degrees).
	* @param Axis Axis to rotate around.
	* @return Rotated Vector.
	*/
	YVector						RotateAngleAxis(const float AngleDeg, const YVector& Axis) const;

	/**
	* Gets a normalized copy of the vector, checking it is safe to do so based on the length.
	* Returns zero vector if vector length is too small to safely normalize.
	*
	* @param Tolerance Minimum squared vector length.
	* @return A normalized copy if safe, (0,0,0) otherwise.
	*/
	YVector						GetSafeNormal(float Tolerance = SMALL_NUMBER) const;

	/**
	* Gets a normalized copy of the 2D components of the vector, checking it is safe to do so. Z is set to zero.
	* Returns zero vector if vector length is too small to normalize.
	*
	* @param Tolerance Minimum squared vector length.
	* @return Normalized copy if safe, otherwise returns zero vector.
	*/
	YVector						GetSafeNormal2D(float Tolerance = SMALL_NUMBER) const;

	/**
	* Returns the cosine of the angle between this vector and another projected onto the XY plane (no Z).
	*
	* @param B the other vector to find the 2D cosine of the angle with.
	* @return The cosine.
	*/
	FORCEINLINE float			CosineAngle2D(YVector B) const;

	/**
	* Gets a copy of this vector projected onto the input vector.
	*
	* @param A	Vector to project onto, does not assume it is normalized.
	* @return Projected vector.
	*/
	FORCEINLINE YVector			ProjectOnTo(const YVector& A) const;

	/**
	* Gets a copy of this vector projected onto the input vector, which is assumed to be unit length.
	*
	* @param  Normal Vector to project onto (assumed to be unit length).
	* @return Projected vector.
	*/
	FORCEINLINE YVector			ProjectOnToNormal(const YVector& Normal) const;

	/**
	* Return the FRotator orientation corresponding to the direction in which the vector points.
	* Sets Yaw and Pitch to the proper numbers, and sets Roll to zero because the roll can't be determined from a vector.
	*
	* @return FRotator from the Vector's direction, without any roll.
	* @see ToOrientationQuat()
	*/
	CORE_API YRotator			ToOrientationRotator() const;

	/**
	* Return the Quaternion orientation corresponding to the direction in which the vector points.
	* Similar to the FRotator version, returns a result without roll such that it preserves the up vector.
	*
	* @note If you don't care about preserving the up vector and just want the most direct rotation, you can use the faster
	* 'FQuat::FindBetweenVectors(YVector::ForwardVector, YourVector)' or 'FQuat::FindBetweenNormals(...)' if you know the vector is of unit length.
	*
	* @return Quaternion from the Vector's direction, without any roll.
	* @see ToOrientationRotator(), FQuat::FindBetweenVectors()
	*/
	CORE_API YQuat				ToOrientationQuat() const;

	/**
	* Return the FRotator orientation corresponding to the direction in which the vector points.
	* Sets Yaw and Pitch to the proper numbers, and sets Roll to zero because the roll can't be determined from a vector.
	* @note Identical to 'ToOrientationRotator()' and preserved for legacy reasons.
	* @return FRotator from the Vector's direction.
	* @see ToOrientationRotator(), ToOrientationQuat()
	*/
	CORE_API YRotator			Rotation() const;

	/**
	* Find good arbitrary axis vectors to represent U and V axes of a plane,
	* using this vector as the normal of the plane.
	*
	* @param Axis1 Reference to first axis.
	* @param Axis2 Reference to second axis.
	*/
	CORE_API void				FindBestAxisVectors(YVector& Axis1, YVector& Axis2) const;

	/** When this vector contains Euler angles (degrees), ensure that angles are between +/-180 */
	CORE_API void				UnwindEuler();

	/**
	* Utility to check if there are any non-finite values (NaN or Inf) in this vector.
	*
	* @return true if there are any non-finite values in this vector, false otherwise.
	*/
	bool						ContainsNaN() const;

	/**
	* Check if the vector is of unit length, with specified tolerance.
	*
	* @param LengthSquaredTolerance Tolerance against squared length.
	* @return true if the vector is a unit vector within the specified tolerance.
	*/
	FORCEINLINE bool			IsUnit(float LengthSquaredTolerance = KINDA_SMALL_NUMBER) const;

	/**
	* Get a textual representation of this vector.
	*
	* @return A string describing the vector.
	*/
	//!!FIXME by zyx
	//YString ToString() const;

	/**
	* Get a locale aware textual representation of this vector.
	*
	* @return A string describing the vector.
	*/
	//!!FIXME by zyx
	//FText ToText() const;

	/** Get a short textural representation of this vector, for compact readable logging. */
	//!!FIXME by zyx
	//YString ToCompactString() const;

	/** Get a short locale aware textural representation of this vector, for compact readable logging. */
	//!!FIXME by zyx
	//FText ToCompactText() const;

	/**
	* Initialize this Vector based on an YString. The String is expected to contain X=, Y=, Z=.
	* The YVector will be bogus when InitFromString returns false.
	*
	* @param	InSourceString	YString containing the vector values.
	* @return true if the X,Y,Z values were read successfully; false otherwise.
	*/
	//!!FIXME by zyx
	//bool InitFromString(const YString& InSourceString);

	/**
	* Converts a Cartesian unit vector into spherical coordinates on the unit sphere.
	* @return Output Theta will be in the range [0, PI], and output Phi will be in the range [-PI, PI].
	*/
	YVector2D					UnitCartesianToSpherical() const;

	/**
	* Convert a direction vector into a 'heading' angle.
	*
	* @return 'Heading' angle between +/-PI. 0 is pointing down +X.
	*/
	float						HeadingAngle() const;

	/**
	* Create an orthonormal basis from a basis with at least two orthogonal vectors.
	* It may change the directions of the X and Y axes to make the basis orthogonal,
	* but it won't change the direction of the Z axis.
	* All axes will be normalized.
	*
	* @param XAxis The input basis' XAxis, and upon return the orthonormal basis' XAxis.
	* @param YAxis The input basis' YAxis, and upon return the orthonormal basis' YAxis.
	* @param ZAxis The input basis' ZAxis, and upon return the orthonormal basis' ZAxis.
	*/
	static CORE_API void		CreateOrthonormalBasis(YVector& XAxis, YVector& YAxis, YVector& ZAxis);

	/**
	* Compare two points and see if they're the same, using a threshold.
	*
	* @param P First vector.
	* @param Q Second vector.
	* @return Whether points are the same within a threshold. Uses fast distance approximation (linear per-component distance).
	*/
	static bool					PointsAreSame(const YVector &P, const YVector &Q);

	/**
	* Compare two points and see if they're within specified distance.
	*
	* @param Point1 First vector.
	* @param Point2 Second vector.
	* @param Dist Specified distance.
	* @return Whether two points are within the specified distance. Uses fast distance approximation (linear per-component distance).
	*/
	static bool					PointsAreNear(const YVector &Point1, const YVector &Point2, float Dist);

	/**
	* Calculate the signed distance (in the direction of the normal) between a point and a plane.
	*
	* @param Point The Point we are checking.
	* @param PlaneBase The Base Point in the plane.
	* @param PlaneNormal The Normal of the plane (assumed to be unit length).
	* @return Signed distance between point and plane.
	*/
	static float				PointPlaneDist(const YVector &Point, const YVector &PlaneBase, const YVector &PlaneNormal);

	/**
	* Calculate the projection of a point on the given plane.
	*
	* @param Point The point to project onto the plane
	* @param Plane The plane
	* @return Projection of Point onto Plane
	*/
	static YVector				PointPlaneProject(const YVector& Point, const YPlane& Plane);

	/**
	* Calculate the projection of a point on the plane defined by counter-clockwise (CCW) points A,B,C.
	*
	* @param Point The point to project onto the plane
	* @param A 1st of three points in CCW order defining the plane
	* @param B 2nd of three points in CCW order defining the plane
	* @param C 3rd of three points in CCW order defining the plane
	* @return Projection of Point onto plane ABC
	*/
	static YVector				PointPlaneProject(const YVector& Point, const YVector& A, const YVector& B, const YVector& C);

	/**
	* Calculate the projection of a point on the plane defined by PlaneBase and PlaneNormal.
	*
	* @param Point The point to project onto the plane
	* @param PlaneBase Point on the plane
	* @param PlaneNorm Normal of the plane (assumed to be unit length).
	* @return Projection of Point onto plane
	*/
	static YVector				PointPlaneProject(const YVector& Point, const YVector& PlaneBase, const YVector& PlaneNormal);

	/**
	* Calculate the projection of a vector on the plane defined by PlaneNormal.
	*
	* @param  V The vector to project onto the plane.
	* @param  PlaneNormal Normal of the plane (assumed to be unit length).
	* @return Projection of V onto plane.
	*/
	static YVector				VectorPlaneProject(const YVector& V, const YVector& PlaneNormal);

	/**
	* Euclidean distance between two points.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The distance between two points.
	*/
	static FORCEINLINE float Dist(const YVector &V1, const YVector &V2);

	/**
	* Squared distance between two points.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The squared distance between two points.
	*/
	static FORCEINLINE float	DistSquared(const YVector &V1, const YVector &V2);

	/**
	* Squared distance between two points in the XY plane only.
	*
	* @param V1 The first point.
	* @param V2 The second point.
	* @return The squared distance between two points in the XY plane
	*/
	static FORCEINLINE float	DistSquaredXY(const YVector &V1, const YVector &V2);

	/**
	* Compute pushout of a box from a plane.
	*
	* @param Normal The plane normal.
	* @param Size The size of the box.
	* @return Pushout required.
	*/
	static FORCEINLINE float	BoxPushOut(const YVector& Normal, const YVector& Size);

	/**
	* See if two normal vectors are nearly parallel, meaning the angle between them is close to 0 degrees.
	*
	* @param  Normal1 First normalized vector.
	* @param  Normal1 Second normalized vector.
	* @param  ParallelCosineThreshold Normals are parallel if absolute value of dot product (cosine of angle between them) is greater than or equal to this. For example: cos(1.0 degrees).
	* @return true if vectors are nearly parallel, false otherwise.
	*/
	static bool					Parallel(const YVector& Normal1, const YVector& Normal2, float ParallelCosineThreshold = THRESH_NORMALS_ARE_PARALLEL);

	/**
	* See if two normal vectors are coincident (nearly parallel and point in the same direction).
	*
	* @param  Normal1 First normalized vector.
	* @param  Normal2 Second normalized vector.
	* @param  ParallelCosineThreshold Normals are coincident if dot product (cosine of angle between them) is greater than or equal to this. For example: cos(1.0 degrees).
	* @return true if vectors are coincident (nearly parallel and point in the same direction), false otherwise.
	*/
	static bool					Coincident(const YVector& Normal1, const YVector& Normal2, float ParallelCosineThreshold = THRESH_NORMALS_ARE_PARALLEL);

	/**
	* See if two normal vectors are nearly orthogonal (perpendicular), meaning the angle between them is close to 90 degrees.
	*
	* @param  Normal1 First normalized vector.
	* @param  Normal2 Second normalized vector.
	* @param  OrthogonalCosineThreshold Normals are orthogonal if absolute value of dot product (cosine of angle between them) is less than or equal to this. For example: cos(89.0 degrees).
	* @return true if vectors are orthogonal (perpendicular), false otherwise.
	*/
	static bool					Orthogonal(const YVector& Normal1, const YVector& Normal2, float OrthogonalCosineThreshold = THRESH_NORMALS_ARE_ORTHOGONAL);

	/**
	* See if two planes are coplanar. They are coplanar if the normals are nearly parallel and the planes include the same set of points.
	*
	* @param Base1 The base point in the first plane.
	* @param Normal1 The normal of the first plane.
	* @param Base2 The base point in the second plane.
	* @param Normal2 The normal of the second plane.
	* @param ParallelCosineThreshold Normals are parallel if absolute value of dot product is greater than or equal to this.
	* @return true if the planes are coplanar, false otherwise.
	*/
	static bool					Coplanar(const YVector& Base1, const YVector& Normal1, const YVector& Base2, const YVector& Normal2, float ParallelCosineThreshold = THRESH_NORMALS_ARE_PARALLEL);

	/**
	* Triple product of three vectors: X dot (Y cross Z).
	*
	* @param X The first vector.
	* @param Y The second vector.
	* @param Z The third vector.
	* @return The triple product: X dot (Y cross Z).
	*/
	static float				Triple(const YVector& X, const YVector& Y, const YVector& Z);

	/**
	* Generates a list of sample points on a Bezier curve defined by 2 points.
	*
	* @param ControlPoints	Array of 4 YVectors (vert1, controlpoint1, controlpoint2, vert2).
	* @param NumPoints Number of samples.
	* @param OutPoints Receives the output samples.
	* @return The path length.
	*/
	//!!FIXME by zyx
	//static CORE_API float		EvaluateBezier(const YVector* ControlPoints, int32 NumPoints, TArray<YVector>& OutPoints);

	/**
	* Converts a vector containing radian values to a vector containing degree values.
	*
	* @param RadVector	Vector containing radian values
	* @return Vector  containing degree values
	*/
	static YVector				RadiansToDegrees(const YVector& RadVector);

	/**
	* Converts a vector containing degree values to a vector containing radian values.
	*
	* @param DegVector	Vector containing degree values
	* @return Vector containing radian values
	*/
	static YVector				DegreesToRadians(const YVector& DegVector);

	/**
	* Given a current set of cluster centers, a set of points, iterate N times to move clusters to be central.
	*
	* @param Clusters Reference to array of Clusters.
	* @param Points Set of points.
	* @param NumIterations Number of iterations.
	* @param NumConnectionsToBeValid Sometimes you will have long strings that come off the mass of points
	* which happen to have been chosen as Cluster starting points.  You want to be able to disregard those.
	*/
	//!!FIXME by zyx
	//static CORE_API void		GenerateClusterCenters(TArray<YVector>& Clusters, const TArray<YVector>& Points, int32 NumIterations, int32 NumConnectionsToBeValid);

	/**
	* Serializer.
	*
	* @param Ar Serialization Archive.
	* @param V Vector to serialize.
	* @return Reference to Archive after serialization.
	*/
	//!!FIXME by zyx
	//friend FArchive& operator<<(FArchive& Ar, YVector& V)
	//{
	//	// @warning BulkSerialize: YVector is serialized as memory dump
	//	// See TArray::BulkSerialize for detailed description of implied limitations.
	//	return Ar << V.X << V.Y << V.Z;
	//}

	//!!FIXME by zyx
	//bool Serialize(FArchive& Ar)
	//{
	//	Ar << *this;
	//	return true;
	//}

	/**
	* Network serialization function.
	* YVectors NetSerialize without quantization (ie exact values are serialized).
	*
	* @see YVector_NetQuantize, YVector_NetQuantize10, YVector_NetQuantize100, YVector_NetQuantizeNormal
	*/
	//!!FIXME by zyx
	//CORE_API bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};


/* YVector inline functions
*****************************************************************************/

/**
* Multiplies a vector by a scaling factor.
*
* @param Scale Scaling factor.
* @param V Vector to scale.
* @return Result of multiplication.
*/
FORCEINLINE YVector operator*(float Scale, const YVector& V)
{
	return V.operator*(Scale);
}


/**
* Creates a hash value from a YVector.
*
* @param Vector the vector to create a hash value for
* @return The hash value from the components
*/
//!!FIXME by zyx
//FORCEINLINE uint32 GetTypeHash(const YVector& Vector)
//{
//	// Note: this assumes there's no padding in YVector that could contain uncompared data.
//	return FCrc::MemCrc_DEPRECATED(&Vector, sizeof(Vector));
//}


#if PLATFORM_LITTLE_ENDIAN
#define INTEL_ORDER_VECTOR(x) (x)
#else
static FORCEINLINE YVector INTEL_ORDER_VECTOR(YVector v)
{
	return YVector(INTEL_ORDERF(v.X), INTEL_ORDERF(v.Y), INTEL_ORDERF(v.Z));
}
#endif


/**
* Util to calculate distance from a point to a bounding box
*
* @param Mins 3D Point defining the lower values of the axis of the bound box
* @param Max 3D Point defining the lower values of the axis of the bound box
* @param Point 3D position of interest
* @return the distance from the Point to the bounding box.
*/
FORCEINLINE float				ComputeSquaredDistanceFromBoxToPoint(const YVector& Mins, const YVector& Maxs, const YVector& Point)
{
	// Accumulates the distance as we iterate axis
	float DistSquared = 0.f;

	// Check each axis for min/max and add the distance accordingly
	// NOTE: Loop manually unrolled for > 2x speed up
	if (Point.X < Mins.X)
	{
		DistSquared += YMath::Square(Point.X - Mins.X);
	}
	else if (Point.X > Maxs.X)
	{
		DistSquared += YMath::Square(Point.X - Maxs.X);
	}

	if (Point.Y < Mins.Y)
	{
		DistSquared += YMath::Square(Point.Y - Mins.Y);
	}
	else if (Point.Y > Maxs.Y)
	{
		DistSquared += YMath::Square(Point.Y - Maxs.Y);
	}

	if (Point.Z < Mins.Z)
	{
		DistSquared += YMath::Square(Point.Z - Mins.Z);
	}
	else if (Point.Z > Maxs.Z)
	{
		DistSquared += YMath::Square(Point.Z - Maxs.Z);
	}

	return DistSquared;
}


FORCEINLINE YVector::YVector(const YVector2D V, float InZ)
	: X(V.X), Y(V.Y), Z(InZ)
{
	DiagnosticCheckNaN();
}


inline YVector	YVector::RotateAngleAxis(const float AngleDeg, const YVector& Axis) const
{
	float S, C;
	YMath::SinCos(&S, &C, YMath::DegreesToRadians(AngleDeg));

	const float XX = Axis.X * Axis.X;
	const float YY = Axis.Y * Axis.Y;
	const float ZZ = Axis.Z * Axis.Z;

	const float XY = Axis.X * Axis.Y;
	const float YZ = Axis.Y * Axis.Z;
	const float ZX = Axis.Z * Axis.X;

	const float XS = Axis.X * S;
	const float YS = Axis.Y * S;
	const float ZS = Axis.Z * S;

	const float OMC = 1.f - C;

	return YVector(
		(OMC * XX + C) * X + (OMC * XY - ZS) * Y + (OMC * ZX + YS) * Z,
		(OMC * XY + ZS) * X + (OMC * YY + C) * Y + (OMC * YZ - XS) * Z,
		(OMC * ZX - YS) * X + (OMC * YZ + XS) * Y + (OMC * ZZ + C) * Z
	);
}

inline bool YVector::PointsAreSame(const YVector &P, const YVector &Q)
{
	float Temp;
	Temp = P.X - Q.X;
	if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
	{
		Temp = P.Y - Q.Y;
		if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
		{
			Temp = P.Z - Q.Z;
			if ((Temp > -THRESH_POINTS_ARE_SAME) && (Temp < THRESH_POINTS_ARE_SAME))
			{
				return true;
			}
		}
	}
	return false;
}

inline bool YVector::PointsAreNear(const YVector &Point1, const YVector &Point2, float Dist)
{
	float Temp;
	Temp = (Point1.X - Point2.X); if (YMath::Abs(Temp) >= Dist) return false;
	Temp = (Point1.Y - Point2.Y); if (YMath::Abs(Temp) >= Dist) return false;
	Temp = (Point1.Z - Point2.Z); if (YMath::Abs(Temp) >= Dist) return false;
	return true;
}

inline float YVector::PointPlaneDist
(
	const YVector &Point,
	const YVector &PlaneBase,
	const YVector &PlaneNormal
)
{
	return (Point - PlaneBase) | PlaneNormal;
}


inline YVector YVector::PointPlaneProject(const YVector& Point, const YVector& PlaneBase, const YVector& PlaneNorm)
{
	//Find the distance of X from the plane
	//Add the distance back along the normal from the point
	return Point - YVector::PointPlaneDist(Point, PlaneBase, PlaneNorm) * PlaneNorm;
}

inline YVector YVector::VectorPlaneProject(const YVector& V, const YVector& PlaneNormal)
{
	return V - V.ProjectOnToNormal(PlaneNormal);
}

inline bool YVector::Parallel(const YVector& Normal1, const YVector& Normal2, float ParallelCosineThreshold)
{
	const float NormalDot = Normal1 | Normal2;
	return YMath::Abs(NormalDot) >= ParallelCosineThreshold;
}

inline bool YVector::Coincident(const YVector& Normal1, const YVector& Normal2, float ParallelCosineThreshold)
{
	const float NormalDot = Normal1 | Normal2;
	return NormalDot >= ParallelCosineThreshold;
}

inline bool YVector::Orthogonal(const YVector& Normal1, const YVector& Normal2, float OrthogonalCosineThreshold)
{
	const float NormalDot = Normal1 | Normal2;
	return YMath::Abs(NormalDot) <= OrthogonalCosineThreshold;
}

inline bool YVector::Coplanar(const YVector &Base1, const YVector &Normal1, const YVector &Base2, const YVector &Normal2, float ParallelCosineThreshold)
{
	if (!YVector::Parallel(Normal1, Normal2, ParallelCosineThreshold)) return false;
	else if (YVector::PointPlaneDist(Base2, Base1, Normal1) > THRESH_POINT_ON_PLANE) return false;
	else return true;
}

inline float YVector::Triple(const YVector& X, const YVector& Y, const YVector& Z)
{
	return
		((X.X * (Y.Y * Z.Z - Y.Z * Z.Y))
			+ (X.Y * (Y.Z * Z.X - Y.X * Z.Z))
			+ (X.Z * (Y.X * Z.Y - Y.Y * Z.X)));
}

inline YVector YVector::RadiansToDegrees(const YVector& RadVector)
{
	return RadVector * (180.f / PI);
}

inline YVector YVector::DegreesToRadians(const YVector& DegVector)
{
	return DegVector * (PI / 180.f);
}

FORCEINLINE YVector::YVector()
{}

FORCEINLINE YVector::YVector(float InF)
	: X(InF), Y(InF), Z(InF)
{
	DiagnosticCheckNaN();
}

FORCEINLINE YVector::YVector(float InX, float InY, float InZ)
	: X(InX), Y(InY), Z(InZ)
{
	DiagnosticCheckNaN();
}

FORCEINLINE YVector::YVector(const YLinearColor& InColor)
	: X(InColor.R), Y(InColor.G), Z(InColor.B)
{
	DiagnosticCheckNaN();
}

FORCEINLINE YVector::YVector(YIntVector InVector)
	: X(InVector.X), Y(InVector.Y), Z(InVector.Z)
{
	DiagnosticCheckNaN();
}

FORCEINLINE YVector::YVector(YIntPoint A)
	: X(A.X), Y(A.Y), Z(0.f)
{
	DiagnosticCheckNaN();
}

FORCEINLINE YVector::YVector(EForceInit)
	: X(0.0f), Y(0.0f), Z(0.0f)
{
	DiagnosticCheckNaN();
}

#ifdef IMPLEMENT_ASSIGNMENT_OPERATOR_MANUALLY
FORCEINLINE YVector& YVector::operator=(const YVector& Other)
{
	this->X = Other.X;
	this->Y = Other.Y;
	this->Z = Other.Z;

	DiagnosticCheckNaN();

	return *this;
}
#endif

FORCEINLINE YVector YVector::operator^(const YVector& V) const
{
	return YVector
	(
		Y * V.Z - Z * V.Y,
		Z * V.X - X * V.Z,
		X * V.Y - Y * V.X
	);
}

FORCEINLINE YVector YVector::CrossProduct(const YVector& A, const YVector& B)
{
	return A ^ B;
}

FORCEINLINE float YVector::operator|(const YVector& V) const
{
	return X*V.X + Y*V.Y + Z*V.Z;
}

FORCEINLINE float YVector::DotProduct(const YVector& A, const YVector& B)
{
	return A | B;
}

FORCEINLINE YVector YVector::operator+(const YVector& V) const
{
	return YVector(X + V.X, Y + V.Y, Z + V.Z);
}

FORCEINLINE YVector YVector::operator-(const YVector& V) const
{
	return YVector(X - V.X, Y - V.Y, Z - V.Z);
}

FORCEINLINE YVector YVector::operator-(float Bias) const
{
	return YVector(X - Bias, Y - Bias, Z - Bias);
}

FORCEINLINE YVector YVector::operator+(float Bias) const
{
	return YVector(X + Bias, Y + Bias, Z + Bias);
}

FORCEINLINE YVector YVector::operator*(float Scale) const
{
	return YVector(X * Scale, Y * Scale, Z * Scale);
}

FORCEINLINE YVector YVector::operator/(float Scale) const
{
	const float RScale = 1.f / Scale;
	return YVector(X * RScale, Y * RScale, Z * RScale);
}

FORCEINLINE YVector YVector::operator*(const YVector& V) const
{
	return YVector(X * V.X, Y * V.Y, Z * V.Z);
}

FORCEINLINE YVector YVector::operator/(const YVector& V) const
{
	return YVector(X / V.X, Y / V.Y, Z / V.Z);
}

FORCEINLINE bool YVector::operator==(const YVector& V) const
{
	return X == V.X && Y == V.Y && Z == V.Z;
}

FORCEINLINE bool YVector::operator!=(const YVector& V) const
{
	return X != V.X || Y != V.Y || Z != V.Z;
}

FORCEINLINE bool YVector::Equals(const YVector& V, float Tolerance) const
{
	return YMath::Abs(X - V.X) <= Tolerance && YMath::Abs(Y - V.Y) <= Tolerance && YMath::Abs(Z - V.Z) <= Tolerance;
}

FORCEINLINE bool YVector::AllComponentsEqual(float Tolerance) const
{
	return YMath::Abs(X - Y) <= Tolerance && YMath::Abs(X - Z) <= Tolerance && YMath::Abs(Y - Z) <= Tolerance;
}


FORCEINLINE YVector YVector::operator-() const
{
	return YVector(-X, -Y, -Z);
}


FORCEINLINE YVector YVector::operator+=(const YVector& V)
{
	X += V.X; Y += V.Y; Z += V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE YVector YVector::operator-=(const YVector& V)
{
	X -= V.X; Y -= V.Y; Z -= V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE YVector YVector::operator*=(float Scale)
{
	X *= Scale; Y *= Scale; Z *= Scale;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE YVector YVector::operator/=(float V)
{
	const float RV = 1.f / V;
	X *= RV; Y *= RV; Z *= RV;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE YVector YVector::operator*=(const YVector& V)
{
	X *= V.X; Y *= V.Y; Z *= V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE YVector YVector::operator/=(const YVector& V)
{
	X /= V.X; Y /= V.Y; Z /= V.Z;
	DiagnosticCheckNaN();
	return *this;
}

FORCEINLINE float& YVector::operator[](int32 Index)
{
	//!!FIXME by zyx
	//check(Index >= 0 && Index < 3);
	if (Index == 0)
	{
		return X;
	}
	else if (Index == 1)
	{
		return Y;
	}
	else
	{
		return Z;
	}
}

FORCEINLINE float YVector::operator[](int32 Index)const
{
	//!!FIXME by zyx
	//check(Index >= 0 && Index < 3);
	if (Index == 0)
	{
		return X;
	}
	else if (Index == 1)
	{
		return Y;
	}
	else
	{
		return Z;
	}
}

FORCEINLINE void YVector::Set(float InX, float InY, float InZ)
{
	X = InX;
	Y = InY;
	Z = InZ;
	DiagnosticCheckNaN();
}

FORCEINLINE float YVector::GetMax() const
{
	return YMath::Max(YMath::Max(X, Y), Z);
}

FORCEINLINE float YVector::GetAbsMax() const
{
	return YMath::Max(YMath::Max(YMath::Abs(X), YMath::Abs(Y)), YMath::Abs(Z));
}

FORCEINLINE float YVector::GetMin() const
{
	return YMath::Min(YMath::Min(X, Y), Z);
}

FORCEINLINE float YVector::GetAbsMin() const
{
	return YMath::Min(YMath::Min(YMath::Abs(X), YMath::Abs(Y)), YMath::Abs(Z));
}

FORCEINLINE YVector YVector::ComponentMin(const YVector& Other) const
{
	return YVector(YMath::Min(X, Other.X), YMath::Min(Y, Other.Y), YMath::Min(Z, Other.Z));
}

FORCEINLINE YVector YVector::ComponentMax(const YVector& Other) const
{
	return YVector(YMath::Max(X, Other.X), YMath::Max(Y, Other.Y), YMath::Max(Z, Other.Z));
}

FORCEINLINE YVector YVector::GetAbs() const
{
	return YVector(YMath::Abs(X), YMath::Abs(Y), YMath::Abs(Z));
}

FORCEINLINE float YVector::Size() const
{
	return YMath::Sqrt(X*X + Y*Y + Z*Z);
}

FORCEINLINE float YVector::SizeSquared() const
{
	return X*X + Y*Y + Z*Z;
}

FORCEINLINE float YVector::Size2D() const
{
	return YMath::Sqrt(X*X + Y*Y);
}

FORCEINLINE float YVector::SizeSquared2D() const
{
	return X*X + Y*Y;
}

FORCEINLINE bool YVector::IsNearlyZero(float Tolerance) const
{
	return
		YMath::Abs(X) <= Tolerance
		&&	YMath::Abs(Y) <= Tolerance
		&&	YMath::Abs(Z) <= Tolerance;
}

FORCEINLINE bool YVector::IsZero() const
{
	return X == 0.f && Y == 0.f && Z == 0.f;
}

FORCEINLINE bool YVector::Normalize(float Tolerance)
{
	const float SquareSum = X*X + Y*Y + Z*Z;
	if (SquareSum > Tolerance)
	{
		const float Scale = YMath::InvSqrt(SquareSum);
		X *= Scale; Y *= Scale; Z *= Scale;
		return true;
	}
	return false;
}

FORCEINLINE bool YVector::IsNormalized() const
{
	return (YMath::Abs(1.f - SizeSquared()) < THRESH_VECTOR_NORMALIZED);
}

FORCEINLINE void YVector::ToDirectionAndLength(YVector &OutDir, float &OutLength) const
{
	OutLength = Size();
	if (OutLength > SMALL_NUMBER)
	{
		float OneOverLength = 1.0f / OutLength;
		OutDir = YVector(X*OneOverLength, Y*OneOverLength,
			Z*OneOverLength);
	}
	else
	{
		OutDir = YVector::ZeroVector;
	}
}

FORCEINLINE YVector YVector::GetSignVector() const
{
	return YVector
	(
		YMath::FloatSelect(X, 1.f, -1.f),
		YMath::FloatSelect(Y, 1.f, -1.f),
		YMath::FloatSelect(Z, 1.f, -1.f)
	);
}

FORCEINLINE YVector YVector::Projection() const
{
	const float RZ = 1.f / Z;
	return YVector(X*RZ, Y*RZ, 1);
}

FORCEINLINE YVector YVector::GetUnsafeNormal() const
{
	const float Scale = YMath::InvSqrt(X*X + Y*Y + Z*Z);
	return YVector(X*Scale, Y*Scale, Z*Scale);
}


FORCEINLINE YVector YVector::GridSnap(const float& GridSz) const
{
	return YVector(YMath::GridSnap(X, GridSz), YMath::GridSnap(Y, GridSz), YMath::GridSnap(Z, GridSz));
}

FORCEINLINE YVector YVector::BoundToCube(float Radius) const
{
	return YVector
	(
		YMath::Clamp(X, -Radius, Radius),
		YMath::Clamp(Y, -Radius, Radius),
		YMath::Clamp(Z, -Radius, Radius)
	);
}

FORCEINLINE YVector YVector::GetClampedToSize(float Min, float Max) const
{
	float VecSize = Size();
	const YVector VecDir = (VecSize > SMALL_NUMBER) ? (*this / VecSize) : YVector::ZeroVector;

	VecSize = YMath::Clamp(VecSize, Min, Max);

	return VecSize * VecDir;
}

FORCEINLINE YVector YVector::GetClampedToSize2D(float Min, float Max) const
{
	float VecSize2D = Size2D();
	const YVector VecDir = (VecSize2D > SMALL_NUMBER) ? (*this / VecSize2D) : YVector::ZeroVector;

	VecSize2D = YMath::Clamp(VecSize2D, Min, Max);

	return YVector(VecSize2D * VecDir.X, VecSize2D * VecDir.Y, Z);
}

FORCEINLINE YVector YVector::GetClampedToMaxSize(float MaxSize) const
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return YVector::ZeroVector;
	}

	const float VSq = SizeSquared();
	if (VSq > YMath::Square(MaxSize))
	{
		const float Scale = MaxSize * YMath::InvSqrt(VSq);
		return YVector(X*Scale, Y*Scale, Z*Scale);
	}
	else
	{
		return *this;
	}
}

FORCEINLINE YVector YVector::GetClampedToMaxSize2D(float MaxSize) const
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return YVector(0.f, 0.f, Z);
	}

	const float VSq2D = SizeSquared2D();
	if (VSq2D > YMath::Square(MaxSize))
	{
		const float Scale = MaxSize * YMath::InvSqrt(VSq2D);
		return YVector(X*Scale, Y*Scale, Z);
	}
	else
	{
		return *this;
	}
}

FORCEINLINE void YVector::AddBounded(const YVector& V, float Radius)
{
	*this = (*this + V).BoundToCube(Radius);
}

FORCEINLINE float& YVector::Component(int32 Index)
{
	return (&X)[Index];
}

FORCEINLINE float YVector::Component(int32 Index) const
{
	return (&X)[Index];
}

FORCEINLINE YVector YVector::Reciprocal() const
{
	YVector RecVector;
	if (X != 0.f)
	{
		RecVector.X = 1.f / X;
	}
	else
	{
		RecVector.X = BIG_NUMBER;
	}
	if (Y != 0.f)
	{
		RecVector.Y = 1.f / Y;
	}
	else
	{
		RecVector.Y = BIG_NUMBER;
	}
	if (Z != 0.f)
	{
		RecVector.Z = 1.f / Z;
	}
	else
	{
		RecVector.Z = BIG_NUMBER;
	}

	return RecVector;
}

FORCEINLINE bool YVector::IsUniform(float Tolerance) const
{
	return AllComponentsEqual(Tolerance);
}

FORCEINLINE YVector YVector::MirrorByVector(const YVector& MirrorNormal) const
{
	return *this - MirrorNormal * (2.f * (*this | MirrorNormal));
}

FORCEINLINE YVector YVector::GetSafeNormal(float Tolerance) const
{
	const float SquareSum = X*X + Y*Y + Z*Z;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		return *this;
	}
	else if (SquareSum < Tolerance)
	{
		return YVector::ZeroVector;
	}
	const float Scale = YMath::InvSqrt(SquareSum);
	return YVector(X*Scale, Y*Scale, Z*Scale);
}

FORCEINLINE YVector YVector::GetSafeNormal2D(float Tolerance) const
{
	const float SquareSum = X*X + Y*Y;

	// Not sure if it's safe to add tolerance in there. Might introduce too many errors
	if (SquareSum == 1.f)
	{
		if (Z == 0.f)
		{
			return *this;
		}
		else
		{
			return YVector(X, Y, 0.f);
		}
	}
	else if (SquareSum < Tolerance)
	{
		return YVector::ZeroVector;
	}

	const float Scale = YMath::InvSqrt(SquareSum);
	return YVector(X*Scale, Y*Scale, 0.f);
}

FORCEINLINE float YVector::CosineAngle2D(YVector B) const
{
	YVector A(*this);
	A.Z = 0.0f;
	B.Z = 0.0f;
	A.Normalize();
	B.Normalize();
	return A | B;
}

FORCEINLINE YVector YVector::ProjectOnTo(const YVector& A) const
{
	return (A * ((*this | A) / (A | A)));
}

FORCEINLINE YVector YVector::ProjectOnToNormal(const YVector& Normal) const
{
	return (Normal * (*this | Normal));
}


FORCEINLINE bool YVector::ContainsNaN() const
{
	return (!YMath::IsFinite(X) ||
		!YMath::IsFinite(Y) ||
		!YMath::IsFinite(Z));
}

FORCEINLINE bool YVector::IsUnit(float LengthSquaredTolerance) const
{
	return YMath::Abs(1.0f - SizeSquared()) < LengthSquaredTolerance;
}

//!!FIXME by zyx
//FORCEINLINE YString YVector::ToString() const
//{
//	return YString::Printf(TEXT("X=%3.3f Y=%3.3f Z=%3.3f"), X, Y, Z);
//}

//!!FIXME by zyx
//FORCEINLINE FText YVector::ToText() const
//{
//	FFormatNamedArguments Args;
//	Args.Add(TEXT("X"), X);
//	Args.Add(TEXT("Y"), Y);
//	Args.Add(TEXT("Z"), Z);
//
//	return FText::Format(NSLOCTEXT("Core", "Vector3", "X={X} Y={Y} Z={Z}"), Args);
//}

//!!FIXME by zyx
//FORCEINLINE FText YVector::ToCompactText() const
//{
//	if (IsNearlyZero())
//	{
//		return NSLOCTEXT("Core", "Vector3_CompactZeroVector", "V(0)");
//	}
//
//	const bool XIsNotZero = !YMath::IsNearlyZero(X);
//	const bool YIsNotZero = !YMath::IsNearlyZero(Y);
//	const bool ZIsNotZero = !YMath::IsNearlyZero(Z);
//
//	FNumberFormattingOptions FormatRules;
//	FormatRules.MinimumFractionalDigits = 2;
//	FormatRules.MinimumIntegralDigits = 0;
//
//	FFormatNamedArguments Args;
//	Args.Add(TEXT("X"), FText::AsNumber(X, &FormatRules));
//	Args.Add(TEXT("Y"), FText::AsNumber(Y, &FormatRules));
//	Args.Add(TEXT("Z"), FText::AsNumber(Z, &FormatRules));
//
//	if (XIsNotZero && YIsNotZero && ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactXYZ", "V(X={X}, Y={Y}, Z={Z})"), Args);
//	}
//	else if (!XIsNotZero && YIsNotZero && ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactYZ", "V(Y={Y}, Z={Z})"), Args);
//	}
//	else if (XIsNotZero && !YIsNotZero && ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactXZ", "V(X={X}, Z={Z})"), Args);
//	}
//	else if (XIsNotZero && YIsNotZero && !ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactXY", "V(X={X}, Y={Y})"), Args);
//	}
//	else if (!XIsNotZero && !YIsNotZero && ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactZ", "V(Z={Z})"), Args);
//	}
//	else if (XIsNotZero && !YIsNotZero && !ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactX", "V(X={X})"), Args);
//	}
//	else if (!XIsNotZero && YIsNotZero && !ZIsNotZero)
//	{
//		return FText::Format(NSLOCTEXT("Core", "Vector3_CompactY", "V(Y={Y})"), Args);
//	}
//
//	return NSLOCTEXT("Core", "Vector3_CompactZeroVector", "V(0)");
//}

//!!FIXME by zyx
//FORCEINLINE YString YVector::ToCompactString() const
//{
//	if (IsNearlyZero())
//	{
//		return YString::Printf(TEXT("V(0)"));
//	}
//
//	YString ReturnString(TEXT("V("));
//	bool bIsEmptyString = true;
//	if (!YMath::IsNearlyZero(X))
//	{
//		ReturnString += YString::Printf(TEXT("X=%.2f"), X);
//		bIsEmptyString = false;
//	}
//	if (!YMath::IsNearlyZero(Y))
//	{
//		if (!bIsEmptyString)
//		{
//			ReturnString += YString(TEXT(", "));
//		}
//		ReturnString += YString::Printf(TEXT("Y=%.2f"), Y);
//		bIsEmptyString = false;
//	}
//	if (!YMath::IsNearlyZero(Z))
//	{
//		if (!bIsEmptyString)
//		{
//			ReturnString += YString(TEXT(", "));
//		}
//		ReturnString += YString::Printf(TEXT("Z=%.2f"), Z);
//		bIsEmptyString = false;
//	}
//	ReturnString += YString(TEXT(")"));
//	return ReturnString;
//}

//!!FIXME by zyx
//FORCEINLINE bool YVector::InitFromString(const YString& InSourceString)
//{
//	X = Y = Z = 0;
//
//	// The initialization is only successful if the X, Y, and Z values can all be parsed from the string
//	const bool bSuccessful = FParse::Value(*InSourceString, TEXT("X="), X) && FParse::Value(*InSourceString, TEXT("Y="), Y) && FParse::Value(*InSourceString, TEXT("Z="), Z);
//
//	return bSuccessful;
//}

//!!FIXME by zyx
//FORCEINLINE YVector2D YVector::UnitCartesianToSpherical() const
//{
//	checkSlow(IsUnit());
//	const float Theta = YMath::Acos(Z / Size());
//	const float Phi = YMath::Atan2(Y, X);
//	return YVector2D(Theta, Phi);
//}

FORCEINLINE float YVector::HeadingAngle() const
{
	// Project Dir into Z plane.
	YVector PlaneDir = *this;
	PlaneDir.Z = 0.f;
	PlaneDir = PlaneDir.GetSafeNormal();

	float Angle = YMath::Acos(PlaneDir.X);

	if (PlaneDir.Y < 0.0f)
	{
		Angle *= -1.0f;
	}

	return Angle;
}



FORCEINLINE float YVector::Dist(const YVector &V1, const YVector &V2)
{
	return YMath::Sqrt(YMath::Square(V2.X - V1.X) + YMath::Square(V2.Y - V1.Y) + YMath::Square(V2.Z - V1.Z));
}

FORCEINLINE float YVector::DistSquared(const YVector &V1, const YVector &V2)
{
	return YMath::Square(V2.X - V1.X) + YMath::Square(V2.Y - V1.Y) + YMath::Square(V2.Z - V1.Z);
}

FORCEINLINE float YVector::DistSquaredXY(const YVector &V1, const YVector &V2)
{
	return YMath::Square(V2.X - V1.X) + YMath::Square(V2.Y - V1.Y);
}

FORCEINLINE float YVector::BoxPushOut(const YVector& Normal, const YVector& Size)
{
	return YMath::Abs(Normal.X*Size.X) + YMath::Abs(Normal.Y*Size.Y) + YMath::Abs(Normal.Z*Size.Z);
}

/** Component-wise clamp for YVector */
FORCEINLINE YVector ClampVector(const YVector& V, const YVector& Min, const YVector& Max)
{
	return YVector(
		YMath::Clamp(V.X, Min.X, Max.X),
		YMath::Clamp(V.Y, Min.Y, Max.Y),
		YMath::Clamp(V.Z, Min.Z, Max.Z)
	);
}

//!!FIXME by zyx
//template <> struct TIsPODType<YVector> { enum { Value = true }; };

/* YMath inline functions
*****************************************************************************/

inline YVector YMath::LinePlaneIntersection
(
	const YVector &Point1,
	const YVector &Point2,
	const YVector &PlaneOrigin,
	const YVector &PlaneNormal
)
{
	return
		Point1
		+ (Point2 - Point1)
		*	(((PlaneOrigin - Point1) | PlaneNormal) / ((Point2 - Point1) | PlaneNormal));
}

inline bool YMath::LineSphereIntersection(const YVector& Start, const YVector& Dir, float Length, const YVector& Origin, float Radius)
{
	const YVector	EO = Start - Origin;
	const float		v = (Dir | (Origin - Start));
	const float		disc = Radius * Radius - ((EO | EO) - v * v);

	if (disc >= 0.0f)
	{
		const float	Time = (v - Sqrt(disc)) / Length;

		if (Time >= 0.0f && Time <= 1.0f)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

inline YVector YMath::VRand()
{
	YVector Result;

	float L;

	do
	{
		// Check random vectors in the unit sphere so result is statistically uniform.
		Result.X = FRand() * 2.f - 1.f;
		Result.Y = FRand() * 2.f - 1.f;
		Result.Z = FRand() * 2.f - 1.f;
		L = Result.SizeSquared();
	} while (L > 1.0f || L < KINDA_SMALL_NUMBER);

	return Result * (1.0f / Sqrt(L));
}


/* YVector2D inline functions
*****************************************************************************/

FORCEINLINE YVector2D::YVector2D(const YVector& V)
	: X(V.X), Y(V.Y)
{
}

inline YVector YVector2D::SphericalToUnitCartesian() const
{
	const float SinTheta = YMath::Sin(X);
	return YVector(YMath::Cos(Y) * SinTheta, YMath::Sin(Y) * SinTheta, YMath::Cos(X));
}
