#include "Framework.h"
#include "InstanceQuad.h"

InstanceQuad::InstanceQuad(Shader * shader)
	:InstanceMesh(shader)
{
}

InstanceQuad::~InstanceQuad()
{
}

void InstanceQuad::Create()
{
	float w = MESH_DEFAUL_LENGTH;
	float h = MESH_DEFAUL_LENGTH;

	vector<MeshVertex> v;
	v.push_back(MeshVertex(-w, -h, 0, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(-w, +h, 0, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, -h, 0, 1, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

	vertices = new MeshVertex[v.size()];  //���ؽ� 4���ʿ�, �ε����� 6�� �ʿ�.
	vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex *>(vertices, vertexCount) //���͸� �迭�� ���� �� �ִ� ���
	);


	indexCount = 6;
	indices = new UINT[indexCount]
	{
		0, 1, 2, 2, 1, 3
	};
}
