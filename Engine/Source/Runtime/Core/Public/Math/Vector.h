#pragma once
#include "HAL/Platform.h"
// A vector in 3-D space composed conponents (X, Y, Z) with floating point precision
struct YVector
{
public:
	float X;
	float Y;
	float Z;

public:
	// (0,0,0)
	static CORE_API const YVector ZeroVector;
	// (0,0,1)
	static CORE_API const YVector UpVector;
	// (1,0,0)
	static CORE_API const YVector ForwardVector;
	// (0,1,0)
	static CORE_API const YVector RightVector;
	
public:
	/*FORCEINLINE void			DiagosticCheckNaN() const
	{
		if (ContainNaN())
		{
			*const_cast<YVector*>(this) = ZeroVector;
		}
	}*/
	// Default constructor (no initialization).
	FORCEINLINE					YVector();
	// Constructor initializing all components to a single float value.
	explicit FORCEINLINE		YVector(float InF);
	// Constructor initial values for each component.
	FORCEINLINE					YVector(float InX, float InY, float InZ);
	// Constructor a vector from a YVector2D and Z Value, YVector2D can put to the register,so pass value is more effective than reference.
	//explicit FORCEINLINE		YVector(const YVector2D V, float InZ);
	//// Constructor using the XYZ components from a 4D vector.
	//FORCEINLINE					YVector(const YVector4D& V);
	//// Constructor a vector from a YLinearColor
	//explicit FORCEINLINE		YVector(const YLinearColor& InColor);
	//// Constructor a vector from a YIntVector
	//explicit					YVector(const YIntVector InVector);
	//// Constructor a vector from a YIntPoint
	//explicit					YVector(YIntPoint A);


};
