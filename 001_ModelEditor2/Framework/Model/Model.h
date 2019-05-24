#pragma once
#define MAX_MODEL_INSTANCE 100
#define MAX_MODEL_TRANSFORMS 128 //�ִ� BONE�� ����

class ModelBone;
class ModelMesh;  //���漱��~
class ModelClip;

class Model
{
public:
	typedef VertexTextureNormalTangentBlend ModelVertex;

public:
	Model();
	~Model();

	UINT MaterialCount() { return materials.size(); }
	vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);  // Material�� �̸��� ã�Ƽ� ������ ���Ϲ޾ƿ�

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh *>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);

	UINT ClipCount() { return clips.size(); }
	vector<ModelClip *>& Clips() { return clips; }
	ModelClip* ClipByIndex(UINT index) { return clips[index]; }
	ModelClip* ClipByName(wstring name);

	void ReadMaterial(wstring file); //Load ���ѳ��� XML���� �б�
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

	D3DXVECTOR3 GetHighVal() { return HighValue; }
	D3DXVECTOR3 GetLowVal() { return LowValue; }

private:
	void BindingBone();
	void BindingMesh();

private:
	vector<Material *> materials;

	ModelBone* root;
	vector<ModelBone *> bones;
	vector<ModelMesh *> meshes;
	vector<ModelClip *> clips;

	void SetHighLowVal();
	D3DXVECTOR3 HighValue;
	D3DXVECTOR3 LowValue;

};