// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	Box.cpp: Implements the FBox class.
=============================================================================*/

#include "Math/Box.h"
#include "Math/Vector4.h"
#include "Math/VectorRegister.h"
#include "Math/Transform.h"


/* FBox structors
 *****************************************************************************/

YBox::YBox(const YVector* Points, int32 Count)
	: Min(0, 0, 0)
	, Max(0, 0, 0)
	, IsValid(0)
{
	for (int32 i = 0; i < Count; i++)
	{
		*this += Points[i];
	}
}


YBox::YBox(const TArray<YVector>& Points)
	: Min(0, 0, 0)
	, Max(0, 0, 0)
	, IsValid(0)
{
	for (int32 i = 0; i < Points.Num(); ++i)
	{
		*this += Points[i];
	}
}


/* FBox interface
 *****************************************************************************/

YBox YBox::TransformBy(const YMatrix& M) const
{
	// if we are not valid, return another invalid box.
	if (!IsValid)
	{
		return YBox(0);
	}

	YBox NewBox;

	const VectorRegister VecMin = VectorLoadFloat3(&Min);
	const VectorRegister VecMax = VectorLoadFloat3(&Max);

	const VectorRegister m0 = VectorLoadAligned(M.M[0]);
	const VectorRegister m1 = VectorLoadAligned(M.M[1]);
	const VectorRegister m2 = VectorLoadAligned(M.M[2]);
	const VectorRegister m3 = VectorLoadAligned(M.M[3]);

	const VectorRegister Half = VectorSetFloat3(0.5f, 0.5f, 0.5f);
	const VectorRegister Origin = VectorMultiply(VectorAdd(VecMax, VecMin), Half);
	const VectorRegister Extent = VectorMultiply(VectorSubtract(VecMax, VecMin), Half);

	VectorRegister NewOrigin = VectorMultiply(VectorReplicate(Origin, 0), m0);
	NewOrigin = VectorMultiplyAdd(VectorReplicate(Origin, 1), m1, NewOrigin);
	NewOrigin = VectorMultiplyAdd(VectorReplicate(Origin, 2), m2, NewOrigin);
	NewOrigin = VectorAdd(NewOrigin, m3);

	VectorRegister NewExtent = VectorAbs(VectorMultiply(VectorReplicate(Extent, 0), m0));
	NewExtent = VectorAdd(NewExtent, VectorAbs(VectorMultiply(VectorReplicate(Extent, 1), m1)));
	NewExtent = VectorAdd(NewExtent, VectorAbs(VectorMultiply(VectorReplicate(Extent, 2), m2)));

	const VectorRegister NewVecMin = VectorSubtract(NewOrigin, NewExtent);
	const VectorRegister NewVecMax = VectorAdd(NewOrigin, NewExtent);
	
	VectorStoreFloat3(NewVecMin, &NewBox.Min );
	VectorStoreFloat3(NewVecMax, &NewBox.Max );
	
	NewBox.IsValid = 1;

	return NewBox;
}

YBox YBox::TransformBy(const YTransform& M) const
{
	return TransformBy(M.ToMatrixWithScale());
}

YBox YBox::InverseTransformBy(const YTransform& M) const
{
	YVector Vertices[8] = 
	{
		YVector(Min),
		YVector(Min.X, Min.Y, Max.Z),
		YVector(Min.X, Max.Y, Min.Z),
		YVector(Max.X, Min.Y, Min.Z),
		YVector(Max.X, Max.Y, Min.Z),
		YVector(Max.X, Min.Y, Max.Z),
		YVector(Min.X, Max.Y, Max.Z),
		YVector(Max)
	};

	YBox NewBox(0);

	for (int32 VertexIndex = 0; VertexIndex < ARRAY_COUNT(Vertices); VertexIndex++)
	{
		YVector4 ProjectedVertex = M.InverseTransformPosition(Vertices[VertexIndex]);
		NewBox += ProjectedVertex;
	}

	return NewBox;
}


YBox YBox::TransformProjectBy(const YMatrix& ProjM) const
{
	YVector Vertices[8] = 
	{
		YVector(Min),
		YVector(Min.X, Min.Y, Max.Z),
		YVector(Min.X, Max.Y, Min.Z),
		YVector(Max.X, Min.Y, Min.Z),
		YVector(Max.X, Max.Y, Min.Z),
		YVector(Max.X, Min.Y, Max.Z),
		YVector(Min.X, Max.Y, Max.Z),
		YVector(Max)
	};

	YBox NewBox(0);

	for (int32 VertexIndex = 0; VertexIndex < ARRAY_COUNT(Vertices); VertexIndex++)
	{
		YVector4 ProjectedVertex = ProjM.TransformPosition(Vertices[VertexIndex]);
		NewBox += ((YVector)ProjectedVertex) / ProjectedVertex.W;
	}

	return NewBox;
}

YBox YBox::Overlap( const YBox& Other ) const
{
	if(Intersect(Other) == false)
	{
		static YBox EmptyBox(ForceInit);
		return EmptyBox;
	}

	// otherwise they overlap
	// so find overlapping box
	YVector MinVector, MaxVector;

	MinVector.X = YMath::Max(Min.X, Other.Min.X);
	MaxVector.X = YMath::Min(Max.X, Other.Max.X);

	MinVector.Y = YMath::Max(Min.Y, Other.Min.Y);
	MaxVector.Y = YMath::Min(Max.Y, Other.Max.Y);

	MinVector.Z = YMath::Max(Min.Z, Other.Min.Z);
	MaxVector.Z = YMath::Min(Max.Z, Other.Max.Z);

	return YBox(MinVector, MaxVector);
}
