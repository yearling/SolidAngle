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
