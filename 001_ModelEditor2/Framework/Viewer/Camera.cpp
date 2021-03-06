#include "Framework.h"
#include "Camera.h"

Camera::Camera()
	: position(0, 0, 0), rotation(0, 0, 0)
	, forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	Rotation();
	Move();
}

Camera::~Camera()
{

}

void Camera::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Camera::Position(Vector3 & vec)
{
	position = vec;

	Move();
}

void Camera::Position(Vector3 * vec)
{
	*vec = position;
}

void Camera::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Camera::Rotation(Vector3 & vec)
{
	rotation = vec;

	Rotation();
}

void Camera::Rotation(Vector3 * vec)
{
	*vec = rotation;
}

void Camera::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Camera::RotationDegree(Vector3 & vec)
{
	//rotation = vec * Math::PI / 180.0f;
	rotation = vec * 0.01745328f;

	Rotation();
}

void Camera::RotationDegree(Vector3 * vec)
{
	//*vec = rotation * 180.0f / Math::PI;
	*vec = rotation * 57.29577957f;
}

void Camera::GetMatrix(Matrix * matrix)
{
	memcpy(matrix, &matView, sizeof(Matrix));
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	Matrix X, Y, Z;
	D3DXMatrixRotationX(&X, rotation.x);
	D3DXMatrixRotationY(&Y, rotation.y);
	D3DXMatrixRotationZ(&Z, rotation.z);

	matRotation = X * Y * Z;


	D3DXVec3TransformNormal(&forward, &Vector3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &Vector3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &Vector3(0, 1, 0), &matRotation);
}

void Camera::View()
{
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}

//#include "Framework.h"
//#include "Camera.h"
//
//Camera::Camera()
//	: position(0, 0, 0), rotation(0, 0)
//	, right(1, 0, 0), up(0, 1, 0)
//{
//	D3DXMatrixIdentity(&matRotation);
//	D3DXMatrixIdentity(&matView);
//
//	Rotation();
//	Move();
//}
//
//Camera::~Camera()
//{
//}
//
//void Camera::Position(float x, float y, float z)
//{
//	Position(D3DXVECTOR3(x, y, z));
//}
//
//void Camera::Position(D3DXVECTOR3 & vec)
//{
//	position = vec;
//
//	Move();
//}
//
//void Camera::Position(D3DXVECTOR3 * vec)
//{
//	*vec = position;
//}
//
//void Camera::Rotation(float x, float y)
//{
//	Rotation(D3DXVECTOR2(x, y));
//}
//
//void Camera::Rotation(D3DXVECTOR2 & vec)
//{
//	rotation = vec;
//
//	Rotation();
//}
//
//void Camera::Rotation(D3DXVECTOR2 * vec)
//{
//	*vec = rotation;
//}
//
//void Camera::RotationDegree(float x, float y)
//{
//	RotationDegree(D3DXVECTOR2(x, y));
//}
//
//void Camera::RotationDegree(D3DXVECTOR2 & vec)
//{
//	//rotation = vec * Math::PI / 180.0f;
//	rotation = vec * 0.01745328f;
//
//	Rotation();
//}
//
//void Camera::RotationDegree(D3DXVECTOR2 * vec)
//{
//	*vec = rotation * 180.0f / Math::PI;
//}
//
//void Camera::GetMatrix(D3DXMATRIX * mat)
//{
//	*mat = matView;
//}
//
//void Camera::Move()
//{
//	View();
//}
//
//void Camera::Rotation()
//{
//	D3DXMATRIX X, Y;
//	D3DXMatrixRotationX(&X, rotation.x);
//	D3DXMatrixRotationY(&Y, rotation.y);
//
//	matRotation = X * Y;
//
//
//	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
//	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);
//	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
//}
//
//void Camera::View()
//{
//	//D3DXMatrixLookAtLH(&matView, &position, &(position+(fix-forward)), &up);
//	//D3DXMatrixLookAtLH(&matView, &position, &(fix - (position + forward)), &up);
//	//D3DXMatrixLookAtLH(&matView, &position, &(fix - forward), &up);
//	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
//}

