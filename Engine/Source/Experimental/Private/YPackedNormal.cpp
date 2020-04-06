#include "YPackedNormal.h"


//
// FPackedNormal serializer
//
FArchive& operator<<(FArchive& Ar, YPackedNormal& N)
{
	Ar << N.Vector.Packed;
	return Ar;
}

FArchive& operator<<(FArchive& Ar, YPackedRGB10A2N& N)
{
	Ar << N.Vector.Packed;
	return Ar;
}

FArchive& operator<<(FArchive& Ar, YPackedRGBA16N& N)
{
	Ar << N.X;
	Ar << N.Y;
	Ar << N.Z;
	Ar << N.W;
	return Ar;
}
