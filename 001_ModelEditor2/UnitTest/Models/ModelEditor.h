#pragma once
#define BONE_NUMBER 128

class ModelEditor
{
public:
	ModelEditor(Shader* shader);
	~ModelEditor();

	void Update();
	void PreRender();
	void Render();
private:
	void SetImport();
	void SetAnimationImport();
	void ChangeAni();
	void Hierarchy();
	void TPose();
	void NoneTPose();
	void BoneTree(ModelBone* bone);
	void bonePicking();
private:
	void ExportModel(wstring exportName);
	void ImportAni(wstring importName);
	void ImportModel();
private:
	Matrix W, V, P;
	Vector3 start, direction;
	Vector3 n, f;

private:
	float blendTime;
	float frameSpeed;
	int selectAni;
	int selectBone;
	UINT clip;
	UINT modelIndex;
	UINT aniModelIndex;
	string aniName;
	string current_state;
	wstring exportName;
	wstring directoryBeforFbx;
	vector<string> aniNames;

private:
	Shader* shader;
	Model* model;
	Model* aniModel;
	ModelRender* modelRender;
	ModelAnimator* modelAnimator;

	vector<wstring> getFolderImage;
	vector<wstring> clipNames;
	
private:
	InstanceMesh* boneBox;
	vector<ModelBone*> vecBone;

private:
	Vector3 importPos;
	Vector3 importScale;
	Vector3 importRotation;
	Vector3 editPos;
	Vector3 editScale;
	Vector3 editRotation;
	UINT currFrame;
	UINT boneNum;
	UINT countBone;
	Vector3 scale;
	bool isShowBones;
	bool isEdit;
	bool importInfo;
	bool isAnimationModel;
};