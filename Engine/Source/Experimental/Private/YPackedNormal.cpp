#include "YPackedNormal.h"
/** X=127.5, Y=127.5, Z=1/127.5f, W=-1.0 */
const VectorRegister YVectorPackingConstants = MakeVectorRegister(127.5f, 127.5f, 1.0f / 127.5f, -1.0f);

/** Zero Normal **/
YPackedNormal YPackedNormal::ZeroNormal(127, 127, 127, 127);

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
