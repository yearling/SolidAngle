// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Misc/MapErrors.h"
	
YName FMapErrors::MatchingLightGUID(TEXT("MatchingLightGUID"));
YName FMapErrors::ActorLargeShadowCaster(TEXT("ActorLargeShadowCaster"));
YName FMapErrors::NoDamageType(TEXT("NoDamageType"));
YName FMapErrors::NonCoPlanarPolys(TEXT("NonCoPlanarPolys"));
YName FMapErrors::SameLocation(TEXT("SameLocation"));
YName FMapErrors::InvalidDrawscale(TEXT("InvalidDrawscale"));
YName FMapErrors::ActorIsObselete(TEXT("ActorIsObselete"));
YName FMapErrors::StaticPhysNone(TEXT("StaticPhysNone"));
YName FMapErrors::VolumeActorCollisionComponentNULL(TEXT("VolumeActorCollisionComponentNULL"));
YName FMapErrors::VolumeActorZeroRadius(TEXT("VolumeActorZeroRadius"));
YName FMapErrors::VertexColorsNotMatchOriginalMesh(TEXT("VertexColorsNotMatchOriginalMesh"));
YName FMapErrors::CollisionEnabledNoCollisionGeom(TEXT("CollisionEnabledNoCollisionGeom"));
YName FMapErrors::ShadowCasterUsingBoundsScale(TEXT("ShadowCasterUsingBoundsScale"));
YName FMapErrors::MultipleSkyLights(TEXT("MultipleSkyLights"));
YName FMapErrors::InvalidTrace(TEXT("InvalidTrace"));
YName FMapErrors::BrushZeroPolygons(TEXT("BrushZeroPolygons"));
YName FMapErrors::CleanBSPMaterials(TEXT("CleanBSPMaterials"));
YName FMapErrors::BrushComponentNull(TEXT("BrushComponentNull"));
YName FMapErrors::PlanarBrush(TEXT("PlanarBrush"));
YName FMapErrors::CameraAspectRatioIsZero(TEXT("CameraAspectRatioIsZero"));
YName FMapErrors::AbstractClass(TEXT("AbstractClass"));
YName FMapErrors::DeprecatedClass(TEXT("DeprecatedClass"));
YName FMapErrors::FoliageMissingStaticMesh(TEXT("FoliageMissingStaticMesh"));
YName FMapErrors::FoliageMissingClusterComponent(TEXT("FoliageMissingStaticMesh"));
YName FMapErrors::FixedUpDeletedLayerWeightmap(TEXT("FixedUpDeletedLayerWeightmap"));
YName FMapErrors::FixedUpIncorrectLayerWeightmap(TEXT("FixedUpIncorrectLayerWeightmap"));
YName FMapErrors::FixedUpSharedLayerWeightmap(TEXT("FixedUpSharedLayerWeightmap"));
YName FMapErrors::LandscapeComponentPostLoad_Warning(TEXT("LandscapeComponentPostLoad_Warning"));
YName FMapErrors::DuplicateLevelInfo(TEXT("DuplicateLevelInfo"));
YName FMapErrors::NoKillZ(TEXT("NoKillZ"));
YName FMapErrors::LightComponentNull(TEXT("LightComponentNull"));
YName FMapErrors::RebuildLighting(TEXT("RebuildLighting"));
YName FMapErrors::StaticComponentHasInvalidLightmapSettings(TEXT("StaticComponentHasInvalidLightmapSettings"));
YName FMapErrors::RebuildPaths(TEXT("RebuildPaths"));
YName FMapErrors::ParticleSystemComponentNull(TEXT("ParticleSystemComponentNull"));
YName FMapErrors::PSysCompErrorEmptyActorRef(TEXT("PSysCompErrorEmptyActorRef"));
YName FMapErrors::PSysCompErrorEmptyMaterialRef(TEXT("PSysCompErrorEmptyMaterialRef"));
YName FMapErrors::SkelMeshActorNoPhysAsset(TEXT("SkelMeshActorNoPhysAsset"));
YName FMapErrors::SkeletalMeshComponent(TEXT("SkeletalMeshComponent"));
YName FMapErrors::SkeletalMeshNull(TEXT("SkeletalMeshNull"));
YName FMapErrors::AudioComponentNull(TEXT("AudioComponentNull"));
YName FMapErrors::SoundCueNull(TEXT("SoundCueNull"));
YName FMapErrors::StaticMeshNull(TEXT("StaticMeshNull"));
YName FMapErrors::StaticMeshComponent(TEXT("StaticMeshComponent"));
YName FMapErrors::SimpleCollisionButNonUniformScale(TEXT("SimpleCollisionButNonUniformScale"));
YName FMapErrors::MoreMaterialsThanReferenced(TEXT("MoreMaterialsThanReferenced"));
YName FMapErrors::ElementsWithZeroTriangles(TEXT("ElementsWithZeroTriangles"));
YName FMapErrors::LevelStreamingVolume(TEXT("LevelStreamingVolume"));
YName FMapErrors::NoLevelsAssociated(TEXT("NoLevelsAssociated"));
YName FMapErrors::FilenameIsTooLongForCooking(TEXT("FilenameIsTooLongForCooking"));
YName FMapErrors::UsingExternalObject(TEXT("UsingExternalObject"));
YName FMapErrors::RepairedPaintedVertexColors(TEXT("RepairedPaintedVertexColors"));
YName FMapErrors::LODActorMissingStaticMesh(TEXT("LODActorMissingStaticMesh"));
YName FMapErrors::LODActorMissingActor(TEXT("LODActorMissingActor"));
YName FMapErrors::LODActorNoActorFound(TEXT("LODActorNoActor"));
YName FMapErrors::HLODSystemNotEnabled(TEXT("HLODSystemNotEnabled"));

static const YString MapErrorsPath = TEXT("Shared/Editor/MapErrors");

FMapErrorToken::FMapErrorToken(const YName& InErrorName)
	: FDocumentationToken(MapErrorsPath, MapErrorsPath, InErrorName.ToString())
{
}

TSharedRef<FMapErrorToken> FMapErrorToken::Create(const YName& InErrorName)
{
	return MakeShareable(new FMapErrorToken(InErrorName));
}
