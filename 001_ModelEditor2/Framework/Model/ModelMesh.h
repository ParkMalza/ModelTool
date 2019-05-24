#pragma once
//
class ModelBone
{
public:
	friend class Model;

public:
	ModelBone();
	~ModelBone();

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	int Index() { return index; }
	vector<ModelBone *>& GetChilds() { return childs; }
	UINT ChildCount() { return childs.size(); }

	wstring Name() { return name; }

	D3DXMATRIX& Transform() { return transform; }
	void Transform(D3DXMATRIX& matrix) { transform = matrix; }

private:
	//������
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	D3DXMATRIX transform;  //����
	vector<ModelBone *> childs;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMesh
{
public:
	friend class Model;

public:
	ModelMesh();
	~ModelMesh();

	void Binding(Model* model);

public:
	void Pass(UINT pass);
	void SetShader(Shader* shader);

	void Update();
	void Render(UINT count);

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class ModelBone* Bone() { return bone; }

	//Deprecated
	//void Transforms(D3DXMATRIX* transforms);
	//void SetTransform(Transform* transform);

	void TransformsSRV(ID3D11ShaderResourceView* srv);
	void FrameBuffer(ConstantBuffer* buffer);
private:
	wstring name;

	int boneIndex;
	class ModelBone* bone;  //���� �����ؼ� ���� ��ġ�� �׸���.

	vector<class ModelMeshPart *> meshParts;
	//�ڱⰡ �����ִ� �Ž� ��Ʈ

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::ModelVertex* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;
};