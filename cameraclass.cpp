#include "cameraclass.h"

static const auto RADIAN_CONVERT = 0.0174532925f;

CameraClass::CameraClass()
	: m_positionX(0.0f)
,m_positionY(0.0f)
,m_positionZ(0.0f)
,m_rotationX(0.0f)
,m_rotationY(0.0f)
,m_rotationZ(0.0f)
{

}

CameraClass::CameraClass(const CameraClass &)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

DirectX::XMFLOAT3 CameraClass::GetPosition()
{
	return DirectX::XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

DirectX::XMFLOAT3 CameraClass::GetRotation()
{
	return DirectX::XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

/*
The Render function uses the position and rotation of the camera to build and update the view matrix.
We first setup our variables for up, position, rotation, and so forth. Then at the origin of the scene we first rotate the camera based on the x, y, and z rotation.
Once it is properly rotated we then translate the camera to the position in 3D space. With the correct values in the position, lookAt, and up we can then use the 
D3DXMatrixLookAtLH function to create the view matrix to represent the current camera rotation and translation
*/


void CameraClass::Render()
{
	DirectX::XMFLOAT3 up, position, lookAt;
	DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	DirectX::XMMATRIX rotationMatrix;


	//setup the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	
	upVector = XMLoadFloat3(&up);

	//setup the position of the camera in the world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positionVector = XMLoadFloat3(&position);

	//setup where the cmaera is looking by default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//create the rotation matrix from the yaw, pitch and roll values
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);


	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	DirectX::XMMATRIX finalView;
	finalView = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	DirectX::XMStoreFloat4x4(&m_viewMatrix, finalView);

	return;
}

void CameraClass::GetViewMatrix(DirectX::XMFLOAT4X4 & viewMatrix)
{
	viewMatrix = m_viewMatrix;
}
