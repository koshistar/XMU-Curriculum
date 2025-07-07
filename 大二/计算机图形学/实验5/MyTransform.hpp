#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iostream>
using namespace std;
#define PI 3.1415927

extern Eigen::Matrix4f mModelView;
extern Eigen::Matrix4f mProjection;
extern Eigen::Matrix4f mViewPort;


void Test()
{
	//�����÷�
	Eigen::Matrix4f m;
	m.setZero();
	m(0, 0) = 1;
	m(3, 3) = 1;
	m(1, 1) = 2.3;   m(1, 2) = 0.99;
	m(2, 1) = 1.1;   m(2, 2) = 0;
	cout << "Matrix:" << endl;
	cout << m << endl;

	//�����÷�
	Eigen::Vector4f v;
	v[0] = 1.0;
	v[1] = 2.0;
	v[2] = 3.0;
	v[3] = 1.0;
	//v[4] = 1.0;
	v.normalize();//��һ��

	//���������������˵Ľ��
	cout << "result:" << endl;
	cout << m * v << endl;
}

vector<Eigen::Vector3f> VertexTransform(vector<Eigen::Vector3f> vertices)
{
	vector<Eigen::Vector3f> rlt(vertices.size());
	Eigen::Vector4f v;

	Eigen::Matrix4f M = mViewPort * mProjection * mModelView;

	for (int i = 0; i < vertices.size(); ++i)
	{
		v[3] = 1.0;
		v.head<3>() = vertices[i];

		v = M * v;
		rlt[i][0] = v[0] / v[3];
		rlt[i][1] = v[1] / v[3];
		rlt[i][2] = v[2] / v[3];
	}
	return rlt;
}

Eigen::Matrix4f myLookAt(float ex, float ey, float ez, float atx, float aty, float atz, float upx, float upy, float upz)
{
	Eigen::Matrix4f m;
	m.setZero();

	//��������հ״���ɴ˺���

	//m.setIdentity();//���Ӧ��ȥ��
	Eigen::Vector3f e(ex, ey, ez);
	Eigen::Vector3f g(atx,aty,atz);
	Eigen::Vector3f t(upx,upy,upz);

	Eigen::Vector3f w = (-g).normalized();
	Eigen::Vector3f u = t.normalized().cross(w).normalized();
	Eigen::Vector3f v = w.cross(u);

	Eigen::Matrix3f rotation;
	rotation.col(0) = u;
	rotation.col(1) = v;
	rotation.col(2) = w;
	
	m.block<3, 3>(0, 0) = rotation;
	m.block<3, 1>(0, 3) = -rotation * e;
	m(3, 3) = 1;
	//��������հ״���ɴ˺���

	mModelView *= m;
	return m;
}

Eigen::Matrix4f myRotateX(float angle)
{
	float theta = angle / 180.0 * PI;
	Eigen::Matrix4f m;
	m.setZero();

	//��������ճ���ϴ�ʵ��Ĵ���

	//m.setIdentity();//���Ӧ��ȥ��
	float costheta = cos(theta);
	float sintheta = sin(theta);
	m(0, 0) = 1;
	m(3, 3) = 1;
	m(1, 1) = costheta;   m(1, 2) = -sintheta;
	m(2, 1) = sintheta;   m(2, 2) = costheta;

	//��������ճ���ϴ�ʵ��Ĵ���

	mModelView *= m;
	return m;
}

Eigen::Matrix4f myRotateY(float angle)
{
	float theta = angle / 180.0 * PI;
	Eigen::Matrix4f m;
	m.setZero();

	//��������ճ���ϴ�ʵ��Ĵ���

	//m.setIdentity();//���Ӧ��ȥ��
	float costheta = cos(theta);
	float sintheta = sin(theta);
	m(1, 1) = 1;
	m(3, 3) = 1;
	m(0, 0) = costheta;
	m(0, 2) = sintheta;
	m(2, 0) = -sintheta;
	m(2, 2) = costheta;

	//��������ճ���ϴ�ʵ��Ĵ���

	mModelView *= m;
	return m;
}

Eigen::Matrix4f myRotateZ(float angle)
{
	float theta = angle / 180.0 * PI;
	Eigen::Matrix4f m;
	m.setZero();

	//��������ճ���ϴ�ʵ��Ĵ���

	//m.setIdentity();//���Ӧ��ȥ��
	float costheta = cos(theta);
	float sintheta = sin(theta);
	m(2, 2) = 1;
	m(3, 3) = 1;
	m(0, 0) = costheta;
	m(0, 1) = -sintheta;
	m(1, 0) = sintheta;
	m(1, 1) = costheta;

	//��������ճ���ϴ�ʵ��Ĵ���

	mModelView *= m;
	return m;
}


Eigen::Matrix4f myTranslate(float x, float y, float z)
{
	Eigen::Matrix4f m;
	m.setZero();

	//��������ճ���ϴ�ʵ��Ĵ���

	//m.setIdentity();//���Ӧ��ȥ��
	m(0, 0) = 1;
	m(1, 1) = 1;
	m(2, 2) = 1;
	m(3, 3) = 1;

	m(0, 3) = x;
	m(1, 3) = y;
	m(2, 3) = z;

	//��������ճ���ϴ�ʵ��Ĵ���

	mModelView *= m;
	return m;
}

Eigen::Matrix4f myRotate(float angle, float x, float y, float z)
{
	float theta = angle / 180.0 * PI;

	Eigen::Matrix4f m;

	//��������ճ���ϴ�ʵ��Ĵ���

	//m.setIdentity();//���Ӧ��ȥ��
	//���Ƚ��й�һ������
	Eigen::Vector3f normal;
	normal[0] = x;
	normal[1] = y;
	normal[2] = z;
	normal.normalize();

	//��������ת����
	m.setZero();
	m(0, 0) = cos(theta) + (1 - cos(theta)) * normal[0] * normal[0];
	m(0, 1) = (1 - cos(theta)) * normal[0] * normal[1] - normal[2] * sin(theta);
	m(0, 2) = (1 - cos(theta)) * normal[0] * normal[2] + normal[1] * sin(theta);
	m(1, 0) = (1 - cos(theta)) * normal[0] * normal[1] + normal[2] * sin(theta);
	m(1, 1) = cos(theta) + (1 - cos(theta)) * normal[1] * normal[1];
	m(1, 2) = (1 - cos(theta)) * normal[1] * normal[2] - normal[0] * sin(theta);
	m(2, 0) = (1 - cos(theta)) * normal[0] * normal[2] - normal[1] * sin(theta);
	m(2, 1) = (1 - cos(theta)) * normal[1] * normal[2] + normal[0] * sin(theta);
	m(2, 2) = cos(theta) + (1 - cos(theta)) * normal[2] * normal[2];
	m(3, 3) = 1;

	//��������ճ���ϴ�ʵ��Ĵ���

	mModelView *= m;

	return m;
}

Eigen::Matrix4f myFrustum(float l, float r, float b, float t, float n, float f)
{
	Eigen::Matrix4f m;
	m.setZero();

	//��������հ״���ɴ˺���

	//m.setIdentity();//���Ӧ��ȥ��
	m << 2 * n / (r - l), 0, (r + l) / (r - l), 0,
		0, 2 * n / (t - b), (t + b) / (t - b), 0,
		0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
		0, 0, -1, 0;

	//��������հ״���ɴ˺���

	mProjection *= m;
	return m;
}

Eigen::Matrix4f myOrtho(float l, float r, float b, float t, float n, float f)
{
	Eigen::Matrix4f m;
	m.setZero();

	//��������հ״���ɴ˺���

	//m.setIdentity();//���Ӧ��ȥ��
	m << 2 / (r - l), 0, 0, -(r + l) / (r - l),
		0, 2 / (t - b), 0, -(t + b) / (t - b),
		0, 0, -2 / (f - n), -(f + n) / (f - n),
		0, 0, 0, 1;

	//��������հ״���ɴ˺���

	mProjection *= m;

	return m;
}

Eigen::Matrix4f myViewPort(float x, float y, float w, float h)
{
	Eigen::Matrix4f m;
	m.setZero();

	//��������հ״���ɴ˺���


	//������δ���Ӧ��ɾ��
	//m(0, 0) = 125;
	//m(1, 1) = 125.0;
	//m(0, 3) = 125.0;
	//m(1, 3) = 125.0;
	//m(2, 2) = 1.0;
	//m(3, 3) = 1.0;
	//������δ���Ӧ��ɾ��

	m << w / 2, 0, 0, x + w / 2,
		0, -h / 2, 0, y + h / 2,
		0, 0, 1 / 2.0, 1 / 2.0,
		0, 0, 0, 1;
	//��������հ״���ɴ˺���

	mViewPort = m;

	return m;
}

Eigen::Matrix4f myPerspective(float fovy, float aspect, float zNear, float zFar)
{
	Eigen::Matrix4f m;
	m.setZero();
	const float pi = 3.1415926535897932384626433832795;

	//��������հ״���ɴ˺���

	//m.setIdentity();//���Ӧ��ȥ��
	float f = 1 / tanf(fovy / 2 * pi / 180);
	m << f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (zFar + zNear) / (zNear - zFar), (2 * zFar * zNear) / (zNear - zFar),
		0, 0, -1, 0;

	//��������հ״���ɴ˺���

	mProjection *= m;
	return m;
}