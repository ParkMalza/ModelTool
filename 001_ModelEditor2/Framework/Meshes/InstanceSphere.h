#pragma once
#include "InstanceMesh.h"

class InstanceSphere : public InstanceMesh
{
public:
	InstanceSphere(Shader* shader, float radius, UINT stackCount = 20, UINT sliceCount = 20);
	~InstanceSphere();

protected:
	void Create() override;

private:
	float radius;

	UINT stackCount;
	UINT sliceCount;  	//slicecount = ������ �󸶳� ����ȭ �Ұ��ΰ�

	Texture* texture;
};