#pragma once
// �ؽ��� ����� �迭�� �޾ƾ� �ؼ� ���� Ŭ���� ����
class Command;

class MeshRender : public Command
{
public:
	MeshRender(Shader* shader, vector<wstring>& textureNames, class Terrain* terrain);
	~MeshRender();

	// Command��(��) ���� ��ӵ�
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Excute() override;

	void Update();
	void PreRender();
	void Render();

	void Instancing(D3DXVECTOR3& picked, class Terrain* terrain, UINT mesh, UINT textureNum);
	void BoxInstancing(UINT boxNum, UINT meshNum, D3DXVECTOR3 reSize);
	//����
	void SelectedQuad();
	void SelectedCube();
	void SelectedGrid();
	void SelectedSphere();
	void SelectedCylinder();

	//����
	void EditQuad();
	void EditCube();
	void EditGrid();
	void EditSphere();
	void EditCylinder();
	void EditTexture();
	//�����ν�
	void QuadCheckHeight();
	void CubeCheckHeight();
	void GridCheckHeight();
	void SphereCheckHeight();
	void CylinderCheckHeight();

	void GetState();
	void SetState();

	void UnProject();
	void SetSelectFalse();
	void SetImgui(D3DXVECTOR3& picked, class Terrain* terrain);
	void SettingMesh(D3DXVECTOR3& picked, Terrain* terrain);
	void Delete(UINT meshType, UINT selectNum);
	void ShowCurrentTexture();

	void Save(wstring fileName);
	void Load(wstring fileName);
private:
	class Terrain* terrain;
	Shader* meshShader;
	Shader* shader;

	class InstanceMesh* quad;
	class InstanceMesh* cube;
	class InstanceMesh* grid;
	class InstanceMesh* sphere;
	class InstanceMesh* cylinder;
	class InstanceMesh* quadBox[5];
	class vector<InstanceMesh*> meshes;
	
	TextureArray* textures;

private:
	D3DXVECTOR3 tempPos;
	D3DXVECTOR3 tempScale;
	D3DXVECTOR3 tempRotation;
	D3DXCOLOR tempColor;
	D3DXVECTOR3 boxScale;
	//��������
	D3DXVECTOR3 quadResize;
	D3DXVECTOR3 cubeResize;
	D3DXVECTOR3 gridResize;
	D3DXVECTOR3 sphereResize;
	D3DXVECTOR3 cylinderResize;
	//���� ����
	D3DXMATRIX W, V, P;
	D3DXVECTOR3 start, direction;
	D3DXVECTOR3 n, f;

	UINT boxIndex;
	
	Transform* boxTrans;
	Transform* editTrans;
	int selectMeshTexture;  //�޽� ���������� ��Ÿ�� �̹���
	int comboTexture; // �ӱ��� ��������
	int instanceTexture;  //�޽��� ������ �̹���
	UINT selectMesh;
	float imageX;
	float imageY;
	int changeTexture;
private:
	Texture* quadImage;
	Texture* cubeImage;
	Texture* gridImage;
	Texture* sphereImage;
	Texture* cylinderImage;

	Texture* whiteImage;
	Texture* redImage;
	Texture* MagentaImage;
	Texture* boxImage;
	Texture* brickImage;
	Texture* stoneImage;
	Texture* floorImage;
	Texture* wallImage;
	vector<Texture*> currentImages;
private:
	bool isMeshSelect[5];
	bool isChange;
	bool isShowCurrentTex;

	int selectQuad;
	int selectCube;
	int selectGrid;
	int selectSphere;
	int selectCylinder;

	int saveIndex;
	int boxNum;

private:
	//����Ʈ
	Shader* mShader;
	Material* box;
	vector<wstring> meshTextures;
	vector<wstring> specularTextures;
	vector<wstring> normalTextures;
	wstring specNames;

	D3DXCOLOR specular;

	vector<string> names; //�޺���
};