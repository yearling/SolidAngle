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
	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProjection, XMMatrixIdentity());
	
	m_vDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vMouseDelata = XMFLOAT2(0.0f, 0.0f);
	m_vEye = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vLookat = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_matViewF.SetIdentity();
	m_matProjectionF.SetIdentity();
	m_vDirF = FVector::ZeroVector;
	m_vMouseDelataF = FVector2D::ZeroVector;
	m_vEyeF = FVector::ZeroVector;
	m_vLookatF = FVector::ZeroVector;
}

void ICamera::SetViewParam(const XMFLOAT3 &eye, const XMFLOAT3 &lookat)
{
	m_vEye = eye;
	m_vLookat = lookat;
	XMVECTOR vEye = XMLoadFloat3(&eye);
	XMVECTOR vLookat = XMLoadFloat3(&lookat);
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	XMMATRIX ViewMatriex = XMMatrixLookAtLH(vEye, vLookat, XMLoadFloat3(&up));
	XMStoreFloat4x4(&m_matView, ViewMatriex);
	XMFLOAT4X4 viewInv44;
	XMVECTOR determinate;
	XMStoreFloat4x4(&viewInv44, XMMatrixInverse(&determinate, ViewMatriex));
	XMFLOAT3 *zBasis = (XMFLOAT3*)&viewInv44._31;
	m_fCameraYawAngle = atan2f(zBasis->x, zBasis->z);
	float len = sqrtf(powf(zBasis->z, 2) + powf(zBasis->x, 2));
	m_fCameraPitchAngle = -atan2f(zBasis->y, len);
	XMStoreFloat3(&m_vDir, XMVector3Normalize(vLookat - vEye));
}

void ICamera::SetViewParamF(const FVector &eye, const FVector &lookat)
{
	m_vEyeF = eye;
	m_vLookatF = lookat;
	m_matViewF = FLookAtMatrix(m_vEyeF, m_vLookatF,FVector(.0f,1.0f,.0f));
	m_vDirF = m_vLookatF - m_vEyeF;

	FVector zBasis(m_matViewF.M[0][2], m_matViewF.M[1][2], m_matViewF.M[2][2]);
	m_fCameraYawAngle = atan2f(zBasis.X, zBasis.Z);
	float len = sqrtf(powf(zBasis.Z, 2) + powf(zBasis.X, 2));
	m_fCameraPitchAngle = -atan2f(zBasis.Y, len);
}

void ICamera::SetProjParam(float FOV, float aspect, float near_plane, float far_plane)
{
	m_fFov = FOV;
	m_fAspect = aspect;
	m_fNearPlane = near_plane;
	m_fFarPlane = far_plane;
	XMStoreFloat4x4(&m_matProjection, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane));
}

void ICamera::SetProjParamF(float FOV, float aspect, float near_plane, float far_plane)
{
	m_fFov = FOV;
	m_fAspect = aspect;
	m_fNearPlane = near_plane;
	m_fFarPlane = far_plane;
	m_matProjectionF = FPerspectiveMatrix(FOV/2, aspect, 1.0f, m_fNearPlane, m_fNearPlane);
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

const XMMATRIX ICamera::GetView() const
{
	return XMLoadFloat4x4(&m_matView);
}

FMatrix ICamera::GetViewF() const
{
	return m_matViewF;
}

const XMMATRIX ICamera::GetViewInverse() const
{
	XMVECTOR determinant;
	return XMMatrixInverse(&determinant, XMLoadFloat4x4(&m_matView));
}

FMatrix ICamera::GetViewInverseF() const
{
	return m_matViewF.Inverse();
}

const XMMATRIX ICamera::GetProject() const
{
	return XMLoadFloat4x4(&m_matProjection);
}

FMatrix ICamera::GetProjectF() const
{
	return m_matProjectionF;
}

const XMMATRIX ICamera::GetProjInv() const
{
	XMVECTOR determinant;
	return XMMatrixInverse(&determinant, XMLoadFloat4x4(&m_matProjection));
}

FMatrix ICamera::GetProjInvF() const
{
	return m_matProjectionF.Inverse();
}

const XMVECTOR ICamera::GetEyePt() const
{
	return XMLoadFloat3(&m_vEye);
}

FVector ICamera::GetEyePtF() const
{
	return m_vEyeF;
}

const XMVECTOR ICamera::GetDir() const
{
	return XMLoadFloat3(&m_vDir);
}

FVector ICamera::GetDirF() const
{
	return m_vDirF;
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

const XMMATRIX ICamera::GetViewProject() const
{
	return XMMatrixMultiply(XMLoadFloat4x4(&m_matView), XMLoadFloat4x4(&m_matProjection));
}

FMatrix ICamera::GetViewProjectF() const
{
	return m_matViewF* m_matProjectionF;
}

const XMMATRIX ICamera::GetViewProjInv() const
{
	XMVECTOR determinant;
	return XMMatrixInverse(&determinant, XMMatrixMultiply(XMLoadFloat4x4(&m_matView), XMLoadFloat4x4(&m_matProjection)));
}

FMatrix ICamera::GetViewProjInvF() const
{
	return (m_matViewF* m_matProjectionF).Inverse();
}

void ICamera::SetVelocity(XMFLOAT3 const &velocity)
{
	m_vVelocity = velocity;
}

void ICamera::SetVelocityF(FVector elocity)
{
	m_vVelocityF = elocity;
}

FirstPersionCamera::FirstPersionCamera()
{

}

FirstPersionCamera::~FirstPersionCamera()
{

}

void FirstPersionCamera::FrameMove(float elapse_time)
{
	m_fCameraPitchAngle = FMath::Min(XM_PIDIV2, m_fCameraPitchAngle);
	m_fCameraPitchAngle = FMath::Max(-XM_PIDIV2, m_fCameraPitchAngle);
	//XMMATRIX CamRote=XMMatrixRotationRollPitchYaw(0,m_fCameraPitchAngle,m_fCameraYawAngle);
	XMMATRIX CamRote = XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle, m_fCameraYawAngle, 0);
	XMFLOAT3 UP3(0.0f, 1.0f, 0.0f);
	XMVECTOR localUP = XMLoadFloat3(&UP3);
	XMFLOAT3 ZDis(0.0f, 0.0f, 100.0f);
	XMVECTOR localAhead = XMLoadFloat3(&ZDis);
	XMVECTOR worldUp = XMVector3TransformCoord(localUP, CamRote);
	XMVECTOR worldAhead = XMVector3TransformCoord(localAhead, CamRote);
	XMVECTOR worldEye = XMLoadFloat3(&m_vEye);
	XMVECTOR worldMove = XMVector3TransformCoord(XMLoadFloat3(&m_vVelocity), CamRote);
	worldMove *= elapse_time;
	worldEye += worldMove;
	XMVECTOR worldlookat = worldEye + worldAhead;
	XMStoreFloat3(&m_vLookat, worldlookat);
	XMStoreFloat3(&m_vEye, worldEye);
	XMStoreFloat4x4(&m_matView, XMMatrixLookAtLH(worldEye, worldlookat, worldUp));
	m_vDir.x = XMVectorGetX(CamRote.r[2]);
	m_vDir.y = XMVectorGetY(CamRote.r[2]);
	m_vDir.z = XMVectorGetZ(CamRote.r[2]);

	XMFLOAT4X4 ToUEMatrix;
	XMStoreFloat4x4(&ToUEMatrix, CamRote);
	FPlane r1(ToUEMatrix._11, ToUEMatrix._12, ToUEMatrix._13, ToUEMatrix._14);
	FPlane r2(ToUEMatrix._21, ToUEMatrix._22, ToUEMatrix._23, ToUEMatrix._24);
	FPlane r3(ToUEMatrix._31, ToUEMatrix._32, ToUEMatrix._33, ToUEMatrix._34);
	FPlane r4(ToUEMatrix._41, ToUEMatrix._42, ToUEMatrix._43, ToUEMatrix._44);
	FMatrix CamRotate(r1, r2, r3, r4);
	FVector FWorldUP = CamRotate.TransformVector(FVector(0.0f, 1.0f, 0.0f));
	FVector FWorldAhead = CamRotate.TransformVector(FVector(0.0f, 0.0f, 100.0f));
	FVector FWorldMove = CamRotate.TransformVector(m_vVelocityF);
	m_vEyeF += (FWorldMove* elapse_time);
	m_vLookatF = m_vEyeF + FWorldAhead;
	m_matViewF = FLookAtMatrix(m_vEyeF, m_vLookatF, FWorldUP);
	m_vDirF = FVector(CamRotate.M[1][0], CamRotate.M[1][1], CamRotate.M[1][2]);
}



