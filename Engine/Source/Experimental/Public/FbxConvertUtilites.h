#pragma once
/**
* FBX basic data conversion class.
*/
#include "fbxsdk.h"
#include "Core.h"
class YFbxDataConverter
{
public:
	static void SetJointPostConversionMatrix(FbxAMatrix ConversionMatrix) { JointPostConversionMatrix = ConversionMatrix; }
	static const FbxAMatrix &GetJointPostConversionMatrix() { return JointPostConversionMatrix; }

	static FVector ConvertPos(FbxVector4 Vector);
	static FVector ConvertDir(FbxVector4 Vector);
	static FRotator ConvertEuler(FbxDouble3 Euler);
	static FVector ConvertScale(FbxDouble3 Vector);
	static FVector ConvertScale(FbxVector4 Vector);
	static FRotator ConvertRotation(FbxQuaternion Quaternion);
	static FVector ConvertRotationToFVect(FbxQuaternion Quaternion, bool bInvertRot);
	static FQuat ConvertRotToQuat(FbxQuaternion Quaternion);
	static float ConvertDist(FbxDouble Distance);
	static bool ConvertPropertyValue(FbxProperty& FbxProperty, UProperty& UnrealProperty, union UPropertyValue& OutUnrealPropertyValue);
	static FTransform ConvertTransform(FbxAMatrix Matrix);
	static FMatrix ConvertMatrix(FbxAMatrix Matrix);

	/*
	 * Convert fbx linear space color to sRGB FColor
	 */
	static FColor ConvertColor(FbxDouble3 Color);

	static FbxVector4 ConvertToFbxPos(FVector Vector);
	static FbxVector4 ConvertToFbxRot(FVector Vector);
	static FbxVector4 ConvertToFbxScale(FVector Vector);

	/*
	* Convert sRGB FColor to fbx linear space color
	*/
	static FbxDouble3   ConvertToFbxColor(FColor Color);
	static FbxString	ConvertToFbxString(FName Name);
	static FbxString	ConvertToFbxString(const FString& String);
private:
	static FbxAMatrix JointPostConversionMatrix;
};
