#include "Matrix4.h"

Matrix4::Matrix4(void)	{
	ToIdentity();
}

Matrix4::Matrix4( float elements[16] )	{
	memcpy(this->values,elements,16*sizeof(float));
}

Matrix4::~Matrix4(void)	{
	ToIdentity();
}

void Matrix4::ToIdentity() {
	ToZero();
	values[0]  = 1.0f;
	values[5]  = 1.0f;
	values[10] = 1.0f;
	values[15] = 1.0f;
}

void Matrix4::ToZero()	{
	for(int i = 0; i < 16; i++)	{
		values[i] = 0.0f;
	}
}

//TODO: Delete me, matrix shouldn't relate to camera
//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
Matrix4 Matrix4::Orthographic(float znear, float zfar,float right, float left, float top, float bottom)	{
	Matrix4 m;

	m.values[0]	= 2.0f / (right-left);
	m.values[5]	= 2.0f / (top-bottom);
	m.values[10]	= -2.0f / (zfar-znear);

	m.values[12]  = -(right+left)/(right-left);
	m.values[13]  = -(top+bottom)/(top-bottom);
	m.values[14]  = -(zfar+znear)/(zfar-znear);
	m.values[15]  = 1.0f;

	return m;
}

//TODO: Delete me, matrix shouldn't relate to camera
Matrix4 Matrix4::BuildViewMatrix(const Vector3 &from, const Vector3 &lookingAt, const Vector3 up /*= Vector3(1,0,0)*/ )	{
	Matrix4 r;
	//r.SetPositionVector(Vector3(-from.x,-from.y,-from.z));

	Matrix4 m;

	Vector3 f = (lookingAt - from);
	f.Normalise();

	Vector3 s = Vector3::Cross(f,up);
	Vector3 u = Vector3::Cross(s,f);

	s.Normalise();
	u.Normalise();

	m.values[0] = s.x;
	m.values[4] = s.y;
	m.values[8] = s.z;

	m.values[1] = u.x;
	m.values[5] = u.y;
	m.values[9] = u.z;

	m.values[2]  = -f.x;
	m.values[6]  = -f.y;
	m.values[10] = -f.z;

	return m*r;
}

Matrix4 Matrix4::Rotation(float degrees, const Vector3 &inaxis)	 {
	Matrix4 m;

	Vector3 axis = inaxis;

	axis.Normalise();

	float c = cos((float)DegToRad(degrees));
	float s = sin((float)DegToRad(degrees));

	m.values[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.values[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.values[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.values[4]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.values[5]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.values[6]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.values[8]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.values[9]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.values[10] = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

Matrix4 Matrix4::Scale( const Vector3 &scale )	{
	Matrix4 m;

	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;	

	return m;
}

Matrix4 Matrix4::Translation( const Vector3 &translation )	{
	Matrix4 m;

	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;	

	return m;
}

Matrix4 Matrix4::GetTransposed() {
	Matrix4 temp;
	temp.values[0] = values[0];
	temp.values[5] = values[5];
	temp.values[10] = values[10];
	temp.values[15] = values[15];

	temp.values[1] = values[4];
	temp.values[2] = values[8];
	temp.values[3] = values[12];
	temp.values[4] = values[1];
	temp.values[6] = values[9];
	temp.values[7] = values[13];
	temp.values[8] = values[2];
	temp.values[9] = values[6];
	temp.values[11] = values[14];
	temp.values[12] = values[3];
	temp.values[13] = values[7];
	temp.values[14] = values[11];
	return temp;
}