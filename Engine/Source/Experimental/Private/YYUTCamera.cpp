#include "YYUTCamera.h"
#include <iostream>
#include <algorithm>
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

void ICamera::SetProjParam(float FOV, float aspect, float near_plane, float far_plane)
{
	m_fFov = FOV;
	m_fAspect = aspect;
	m_fNearPlane = near_plane;
	m_fFarPlane = far_plane;
	XMStoreFloat4x4(&m_matProjection, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane));
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

const XMMATRIX ICamera::GetViewInverse() const
{
	XMVECTOR determinant;
	return XMMatrixInverse(&determinant, XMLoadFloat4x4(&m_matView));
}

const XMMATRIX ICamera::GetProject() const
{
	return XMLoadFloat4x4(&m_matProjection);
}

const XMMATRIX ICamera::GetProjInv() const
{
	XMVECTOR determinant;
	return XMMatrixInverse(&determinant, XMLoadFloat4x4(&m_matProjection));
}

const XMVECTOR ICamera::GetEyePt() const
{
	return XMLoadFloat3(&m_vEye);
}

const XMVECTOR ICamera::GetDir() const
{
	return XMLoadFloat3(&m_vDir);
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

const XMMATRIX ICamera::GetViewProject() const
{
	return XMMatrixMultiply(XMLoadFloat4x4(&m_matView), XMLoadFloat4x4(&m_matProjection));
}

const XMMATRIX ICamera::GetViewProjInv() const
{
	XMVECTOR determinant;
	return XMMatrixInverse(&determinant, XMMatrixMultiply(XMLoadFloat4x4(&m_matView), XMLoadFloat4x4(&m_matProjection)));
}

void ICamera::SetVelocity(XMFLOAT3 const &velocity)
{
	m_vVelocity = velocity;
}




FirstPersionCamera::FirstPersionCamera()
{

}

FirstPersionCamera::~FirstPersionCamera()
{

}

void FirstPersionCamera::FrameMove(float elapse_time)
{
	m_fCameraPitchAngle = (std::min)(XM_PIDIV2, m_fCameraPitchAngle);
	m_fCameraPitchAngle = (std::max)(-XM_PIDIV2, m_fCameraPitchAngle);
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
}



