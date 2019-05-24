#pragma once

#include "InstanceMesh.h"

class InstanceCylinder : public InstanceMesh
{
public:
	InstanceCylinder(Shader* shader, float radius, float height, UINT sliceCount = 20, UINT stackCount = 20);
	~InstanceCylinder();

protected:
	void Create() override;

private:
	void BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices);
	void BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices);

private:
	float topRadius;  //���� ������
	float bottomRadius;  //�Ʒ��� ������
	float height;  //���̰���

	UINT sliceCount; //slicecount = ������ �󸶳� ����ȭ �Ұ��ΰ� == �Ѳ��� �ٴ��� �ﰢ�� ��
	UINT stackCount;  //����������

	Texture* texture;
};