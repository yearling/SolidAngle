#include "YYUTCamera.h"
#include <iostream>
using std::cout;
using std::endl;

ICamera::ICamera() :
	m_fCameraYawAngle(0),
	m_fCameraPitchAngle(0),
	m_fFov(0.0f),
	m_fAspect(0.0f),
	m_fNearPlane(1.0f),
	m_fFarPlane(1000.0f),
	m_Width(0),
	m_Height(0)
{
	m_matView.SetIdentity();
	m_matProjection.SetIdentity();
	m_vDir = FVector::ZeroVector;
	m_vMouseDelata = FVector2D::ZeroVector;
	m_vEye = FVector::ZeroVector;
	m_vLookat = FVector::ZeroVector;
}

void ICamera::SetViewParam(const FVector &eye, const FVector &lookat)
{
	m_vEye = eye;
	m_vLookat = lookat;
	m_matView = FLookAtMatrix(m_vEye, m_vLookat, FVector(.0f, 1.0f, .0f));
	m_vDir = m_vLookat - m_vEye;

	FVector zBasis(m_matView.M[0][2], m_matView.M[1][2], m_matView.M[2][2]);
	m_fCameraYawAngle = atan2f(zBasis.X, zBasis.Z);
	float len = sqrtf(powf(zBasis.Z, 2) + powf(zBasis.X, 2));
	m_fCameraPitchAngle = -atan2f(zBasis.Y, len);
	float RAD_2_DEG = 180.0f / PI;
	FRotator CameraRotator(m_fCameraPitchAngle*RAD_2_DEG, m_fCameraYawAngle*RAD_2_DEG, 0);
	FMatrix CameraTransform = FRotationMatrix(CameraRotator);
	FVector ZBasisOriginal(0, 0, 1);
	FVector ZBasisTrans = CameraTransform.TransformVector(ZBasisOriginal);
}

void ICamera::SetProjParam(float FOV, float aspect, float near_plane, float far_plane)
{
	m_fFov = FOV;
	m_fAspect = aspect;
	m_fNearPlane = near_plane;
	m_fFarPlane = far_plane;
	m_matProjection = FPerspectiveMatrix(FOV / 2, aspect, 1.0f, m_fNearPlane, m_fNearPlane);
}

void ICamera::SetWindow(int width, int height)
{
	m_Width = width;
	m_Height = height;
}

void ICamera::AddPitchYaw(float pitch, float yaw)
{
	m_fCameraPitchAngle += pitch;
	m_fCameraYawAngle += yaw;
}

YRay ICamera::GetWorldRayFromScreen(const FVector2D & ScreenCoord) const
{
	FVector4 ClipRay(ScreenCoord.X, ScreenCoord.Y, -1.0, 1.0);
	FMatrix InvProjectionMatrix = GetProjInv();
	FVector4 RayDirInCamera = InvProjectionMatrix.TransformFVector4(ClipRay).GetSafeNormal();
	FMatrix ViewToWorld = GetViewInverse();
	FVector RayDirInWorld = ViewToWorld.TransformVector(RayDirInCamera).GetUnsafeNormal3();
	FVector RayStart = GetCameraPos();
	return YRay(RayStart, RayDirInWorld);
}

FMatrix ICamera::GetView() const
{
	return m_matView;
}

FMatrix ICamera::GetViewInverse() const
{
	return m_matView.Inverse();
}

FMatrix ICamera::GetProject() const
{
	return m_matProjection;
}

FMatrix ICamera::GetProjInv() const
{
	return m_matProjection.Inverse();
}

FVector ICamera::GetEyePt() const
{
	return m_vEye;
}

FVector ICamera::GetDir() const
{
	return m_vDir;
}

float ICamera::GetNearClip() const
{
	return m_fNearPlane;
}

float ICamera::GetFarClip() const
{
	return m_fFarPlane;
}

void ICamera::Reset()
{

}



ICamera::~ICamera()
{

}

void ICamera::FrameMove(float elapse_time)
{

}

FMatrix ICamera::GetViewProject() const
{
	return m_matView * m_matProjection;
}


FMatrix ICamera::GetViewProjInv() const
{
	return (m_matView* m_matProjection).Inverse();
}

void ICamera::SetVelocity(FVector elocity)
{
	m_vVelocity = elocity;
}

FirstPersionCamera::FirstPersionCamera()
{

}

FirstPersionCamera::~FirstPersionCamera()
{

}

void FirstPersionCamera::FrameMove(float elapse_time)
{
	m_fCameraPitchAngle = FMath::Min(HALF_PI, m_fCameraPitchAngle);
	m_fCameraPitchAngle = FMath::Max(-HALF_PI, m_fCameraPitchAngle);
	//XMMATRIX CamRote = XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle, m_fCameraYawAngle, 0);

	float RAD_2_DEG = 180.0f / PI;
	FMatrix CamRotate = FRotationMatrix(FRotator(m_fCameraPitchAngle*RAD_2_DEG, m_fCameraYawAngle*RAD_2_DEG, 0));
	FVector FWorldUP = CamRotate.TransformVector(FVector(0.0f, 1.0f, 0.0f));
	FVector FWorldAhead = CamRotate.TransformVector(FVector(0.0f, 0.0f, 100.0f));
	FVector FWorldMove = CamRotate.TransformVector(m_vVelocity);
	m_vEye += (FWorldMove* elapse_time);
	m_vLookat = m_vEye + FWorldAhead;
	m_matView = FLookAtMatrix(m_vEye, m_vLookat, FWorldUP);
	m_vDir = FVector(CamRotate.M[1][0], CamRotate.M[1][1], CamRotate.M[1][2]);
}



