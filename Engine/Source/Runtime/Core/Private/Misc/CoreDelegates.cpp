// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

// Core includes.
#include "Misc/CoreDelegates.h"
#include "Math/Vector.h"


//////////////////////////////////////////////////////////////////////////
// FCoreDelegates

TArray<FCoreDelegates::FHotFixDelegate> FCoreDelegates::HotFixDelegates;
TArray<FCoreDelegates::FResolvePackageNameDelegate> FCoreDelegates::PackageNameResolvers;

FCoreDelegates::FHotFixDelegate& FCoreDelegates::GetHotfixDelegate(EHotfixDelegates::Type HotFix)
{
	if (HotFix >= HotFixDelegates.Num())
	{
		HotFixDelegates.SetNum(HotFix + 1);
	}
	return HotFixDelegates[HotFix];
}

FCoreDelegates::FOnPreMainInit& FCoreDelegates::GetPreMainInitDelegate()
{
	static FOnPreMainInit StaticDelegate;
	return StaticDelegate;
}

FCoreDelegates::FOnMountPak FCoreDelegates::OnMountPak;
FCoreDelegates::FOnUnmountPak FCoreDelegates::OnUnmountPak;
FCoreDelegates::FOnUserLoginChangedEvent FCoreDelegates::OnUserLoginChangedEvent; 
FCoreDelegates::FOnUserControllerConnectionChange FCoreDelegates::OnControllerConnectionChange;
FCoreDelegates::FOnSafeFrameChangedEvent FCoreDelegates::OnSafeFrameChangedEvent;
FCoreDelegates::FOnHandleSystemEnsure FCoreDelegates::OnHandleSystemEnsure;
FCoreDelegates::FOnHandleSystemError FCoreDelegates::OnHandleSystemError;
FCoreDelegates::FOnActorLabelChanged FCoreDelegates::OnActorLabelChanged;

FCoreDelegates::FPakEncryptionKeyDelegate& FCoreDelegates::GetPakEncryptionKeyDelegate()
{
	static FPakEncryptionKeyDelegate PakEncryptionKeyDelegate;
	return PakEncryptionKeyDelegate;
}

FCoreDelegates::FPakSigningKeysDelegate& FCoreDelegates::GetPakSigningKeysDelegate()
{
	static FPakSigningKeysDelegate PakSigningKeysDelegate;
	return PakSigningKeysDelegate;
}

#if WITH_EDITOR
	FSimpleMulticastDelegate FCoreDelegates::PreModal;
	FSimpleMulticastDelegate FCoreDelegates::PostModal;
#endif	//WITH_EDITOR
FSimpleMulticastDelegate FCoreDelegates::OnShutdownAfterError;
FSimpleMulticastDelegate FCoreDelegates::OnInit;
FSimpleMulticastDelegate FCoreDelegates::OnExit;
FSimpleMulticastDelegate FCoreDelegates::OnPreExit;
FSimpleMulticastDelegate FCoreDelegates::ColorPickerChanged;
FCoreDelegates::FOnModalMessageBox FCoreDelegates::ModalErrorMessage;
FCoreDelegates::FOnInviteAccepted FCoreDelegates::OnInviteAccepted;
FCoreDelegates::FWorldOriginOffset FCoreDelegates::PreWorldOriginOffset;
FCoreDelegates::FWorldOriginOffset FCoreDelegates::PostWorldOriginOffset;
FCoreDelegates::FStarvedGameLoop FCoreDelegates::StarvedGameLoop;

FCoreDelegates::FApplicationLifetimeDelegate FCoreDelegates::ApplicationWillDeactivateDelegate;
FCoreDelegates::FApplicationLifetimeDelegate FCoreDelegates::ApplicationHasReactivatedDelegate;
FCoreDelegates::FApplicationLifetimeDelegate FCoreDelegates::ApplicationWillEnterBackgroundDelegate;
FCoreDelegates::FApplicationLifetimeDelegate FCoreDelegates::ApplicationHasEnteredForegroundDelegate;
FCoreDelegates::FApplicationLifetimeDelegate FCoreDelegates::ApplicationWillTerminateDelegate; 

FCoreDelegates::FApplicationRegisteredForRemoteNotificationsDelegate FCoreDelegates::ApplicationRegisteredForRemoteNotificationsDelegate;
FCoreDelegates::FApplicationRegisteredForUserNotificationsDelegate FCoreDelegates::ApplicationRegisteredForUserNotificationsDelegate;
FCoreDelegates::FApplicationFailedToRegisterForRemoteNotificationsDelegate FCoreDelegates::ApplicationFailedToRegisterForRemoteNotificationsDelegate;
FCoreDelegates::FApplicationReceivedRemoteNotificationDelegate FCoreDelegates::ApplicationReceivedRemoteNotificationDelegate;
FCoreDelegates::FApplicationReceivedLocalNotificationDelegate FCoreDelegates::ApplicationReceivedLocalNotificationDelegate;

FCoreDelegates::FOnFConfigFileCreated FCoreDelegates::OnFConfigCreated;
FCoreDelegates::FOnFConfigFileCreated FCoreDelegates::OnFConfigDeleted;

#if WITH_EDITOR
FCoreDelegates::FOnTargetPlatformChangedSupportedFormats FCoreDelegates::OnTargetPlatformChangedSupportedFormats;
#endif 

FCoreDelegates::FStatCheckEnabled FCoreDelegates::StatCheckEnabled;
FCoreDelegates::FStatEnabled FCoreDelegates::StatEnabled;
FCoreDelegates::FStatDisabled FCoreDelegates::StatDisabled;
FCoreDelegates::FStatDisableAll FCoreDelegates::StatDisableAll;

FCoreDelegates::FApplicationLicenseChange FCoreDelegates::ApplicationLicenseChange;
FCoreDelegates::FPlatformChangedLaptopMode FCoreDelegates::PlatformChangedLaptopMode;

FCoreDelegates::FLoadStringAssetReferenceInCook FCoreDelegates::LoadStringAssetReferenceInCook;

FCoreDelegates::FVRHeadsetRecenter FCoreDelegates::VRHeadsetTrackingInitializingAndNeedsHMDToBeTrackedDelegate;
FCoreDelegates::FVRHeadsetRecenter FCoreDelegates::VRHeadsetTrackingInitializedDelegate;
FCoreDelegates::FVRHeadsetRecenter FCoreDelegates::VRHeadsetRecenter;
FCoreDelegates::FVRHeadsetLost FCoreDelegates::VRHeadsetLost;
FCoreDelegates::FVRHeadsetReconnected FCoreDelegates::VRHeadsetReconnected;
FCoreDelegates::FVRHeadsetConnectCanceled FCoreDelegates::VRHeadsetConnectCanceled;
FCoreDelegates::FVRHeadsetPutOnHead FCoreDelegates::VRHeadsetPutOnHead;
FCoreDelegates::FVRHeadsetRemovedFromHead FCoreDelegates::VRHeadsetRemovedFromHead;

FCoreDelegates::FOnUserActivityStringChanged FCoreDelegates::UserActivityStringChanged;
FCoreDelegates::FOnGameSessionIDChange FCoreDelegates::GameSessionIDChanged;
FCoreDelegates::FOnCrashOverrideParamsChanged FCoreDelegates::CrashOverrideParamsChanged;
FCoreDelegates::FOnIsVanillaProductChanged FCoreDelegates::IsVanillaProductChanged;

FCoreDelegates::FOnAsyncLoadingFlush FCoreDelegates::OnAsyncLoadingFlush;
FCoreDelegates::FRenderingThreadChanged FCoreDelegates::PostRenderingThreadCreated;
FCoreDelegates::FRenderingThreadChanged FCoreDelegates::PreRenderingThreadDestroyed;
FSimpleMulticastDelegate FCoreDelegates::OnFEngineLoopInitComplete;
FCoreDelegates::FImageIntegrityChanged  FCoreDelegates::OnImageIntegrityChanged;

FCoreDelegates::FApplicationReceivedOnScreenOrientationChangedNotificationDelegate FCoreDelegates::ApplicationReceivedScreenOrientationChangedNotificationDelegate;

FCoreDelegates::FConfigReadyForUse FCoreDelegates::ConfigReadyForUse;

FSimpleMulticastDelegate FCoreDelegates::OnOutOYMemory;
FCoreDelegates::FGetOnScreenMessagesDelegate FCoreDelegates::OnGetOnScreenMessages;

void RegisterEncryptionKey(const char* InEncryptionKey)
{
	FCoreDelegates::GetPakEncryptionKeyDelegate().BindLambda([InEncryptionKey]() { return InEncryptionKey; });
}

void RegisterPakSigningKeys(const char* InExponent, const char* InModulus)
{
	static YString Exponent(ANSI_TO_TCHAR(InExponent));
	static YString Modulus(ANSI_TO_TCHAR(InModulus));

	FCoreDelegates::GetPakSigningKeysDelegate().BindLambda([](YString& OutExponent, YString& OutModulus)
	{
		OutExponent = Exponent;
		OutModulus = Modulus;
	});
}