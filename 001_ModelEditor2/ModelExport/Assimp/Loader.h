#pragma once

class Loader
{
public:
	Loader();
	~Loader();

	void ReadFile(wstring file);

public:
	void ExportMaterial(wstring savePath, bool bOverwrite = true); // ������ �̹� ������ �����


private:
	void ReadMaterial(); //scene���κ��� �о��
	void WriteMaterial(wstring savePath, bool bOverwrite);
	string WriteTexture(string file, string saveFolder);
public:
	void ExportMesh(wstring savePath, bool bOverwrite = true);  //�Ž��� �θ���.

private:
	void ReadBoneData(aiNode* node, int index, int parent);  //�� ����Ÿ �θ�, ���
	void ReadMeshData(aiNode* node, int bone); 
	void ReadSkinData();
	UINT FindBoneIndex(string name);

	void WriteMehData(wstring savePath, bool bOverwrite);

public:
	//�� Ŭ���� ���� �ϳ�
	void GetClipList(vector<wstring>* list);
	void ExportAnimClip(UINT index, wstring savePath, bool bOverwrite = true);

private:
	struct AsClip* ReadClipData(aiAnimation* animation);
	void ReadKeyframeData(struct AsClip* clip, aiNode* node, vector<struct AsClipNode>& aiNodeInfos);
	struct AsClipNode* FindClipNode(vector<AsClipNode>& aniNodeInfos, aiString name);

	void WriteClipData(struct AsClip* clip, wstring savePath, bool bOverwrite);

private:
	wstring file;
	Assimp::Importer* importer;  // �ҷ����̴� ����
	const aiScene* scene; //assimp�� ��� �ڷ������� �տ� ai�� �ٴ´�.

	vector<struct AsMaterial*> materials; //�⺻ Ŭ������ �ƴϴϱ� �տ��ٰ� ���漱��
	vector<struct AsBone*> bones;
	vector <struct AsMesh*> meshes;
};