// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FbxMeshImportData.h"

class UAnimSequence;
struct FPropertyChangedEvent;

/**
* I know these descriptions don't make sense, but the functions I use act a bit different depending on situation.
a) FbxAnimStack::GetLocalTimeSpan will return you the start and stop time for this stack (or take). It's simply two time values that can be set to anything regardless of animation curves. Generally speaking, applications set these to the start and stop time of the timeline.
b) As for FbxNode::GetAnimationInternval, this one will iterate through all properties recursively, and then for all animation curves it finds, for the animation layer index specified. So in other words, if one property has been animated, it will modify this result. This is completely different from GetLocalTimeSpan since it calculates the time span depending on the keys rather than just using the start and stop time that was saved in the file.
*/

/** Animation length type when importing */
enum EFBXAnimationLengthImportType
{
	/** This option imports animation frames based on what is defined at the time of export */
	FBXALIT_ExportedTime		,
	/** Will import the range of frames that have animation. Can be useful if the exported range is longer than the actual animation in the FBX file */
	FBXALIT_AnimatedKey				,
	/** This will enable the Start Frame and End Frame properties for you to define the frames of animation to import */
	FBXALIT_SetRange				,

	FBXALIT_MAX,
};

/**
* Import data and options used when importing any mesh from FBX
*/
class  UFbxAnimSequenceImportData : public UFbxAssetImportData
{
public:
	UFbxAnimSequenceImportData()
		:bImportMeshesInBoneHierarchy(true)
		, bImportCustomAttribute(true)
		, bRemoveRedundantKeys(true)
		, bDoNotImportCurveWithZero(true)
	{
		FrameImportRange.Min = 0;
		FrameImportRange.Max = 0;

		MaterialCurveSuffixes.Add(TEXT("_mat"));
	}
	static UFbxAnimSequenceImportData* UFbxAnimSequenceImportData::GetImportDataForAnimSequence(UAnimSequence* AnimSequence, UFbxAnimSequenceImportData* TemplateForCreation);
	/** If checked, meshes nested in bone hierarchies will be imported instead of being converted to bones. */
	bool bImportMeshesInBoneHierarchy;
	
	/** Which animation range to import. The one defined at Exported, at Animated time or define a range manually */
	TEnumAsByte<enum EFBXAnimationLengthImportType> AnimationLength;

	/** Start frame when Set Range is used in Animation Length */
	int32	StartFrame_DEPRECATED;

	/** End frame when Set Range is used in Animation Length  */
	int32	EndFrame_DEPRECATED;

	/** Frame range used when Set Range is used in Animation Length */
	FInt32Interval FrameImportRange;

	/** Enable this option to use default sample rate for the imported animation at 30 frames per second */
	bool bUseDefaultSampleRate;

	/** Name of source animation that was imported, used to reimport correct animation from the FBX file */
	FString SourceAnimationName;

	/** Import if custom attribute as a curve within the animation */
	bool bImportCustomAttribute;

	/** Set Material Curve Type for all custom attributes that exists */
	bool bSetMaterialDriveParameterOnCustomAttribute;

	/** Set Material Curve Type for the custom attribute with the following suffixes. This doesn't matter if Set Material Curve Type is true  */
	TArray<FString> MaterialCurveSuffixes;

	/** When importing custom attribute as curve, remove redundant keys */
	bool bRemoveRedundantKeys;

	/** If enabled, this will delete this type of asset from the FBX */
	bool bDeleteExistingMorphTargetCurves;

	/** When importing custom attribute or morphtarget as curve, do not import if it doens't have any value other than zero. This is to avoid adding extra curves to evaluate */
	bool bDoNotImportCurveWithZero;

	/** If enabled, this will import a curve within the animation */
	bool bPreserveLocalTransform;

	

};

