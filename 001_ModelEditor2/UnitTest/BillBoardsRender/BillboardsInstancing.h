#pragma once

class BillboardsInstancing : public Command
{
public:
	BillboardsInstancing(class Projector* projector, class Terrain* terrain);
	~BillboardsInstancing();

	void Update();
	void Render();
	void SettingImgui(class Terrain* terrain, Vector3& picked);
	void SettingBillboard(class Terrain* terrain, Vector3& picked);
	void MakeRandomBillboard(D3D11_BOX & box, class Terrain* terrain, D3DXVECTOR3& picked);

	void Save(wstring name);
	void Load(wstring name);



	// Command��(��) ���� ��ӵ�
	virtual void Undo() override;

	virtual void Redo() override;

	virtual void Excute() override;

private:
	class Terrain* terrain;
	Texture* texture;
	Shader* shader;
	class BillboardInstance* billboard;
	class Projector* projector;

private:
	UINT selectGrass;
	vector<wstring> getFolderImage;  //�������� ������ �̸�
	vector<wstring> grassNames;  //����� �̸�

private:
	float imageX;  //Ǯ �̹��� ũ��
	float imageY;
	UINT grassType;
	UINT brushType;
	UINT brushRange;
	UINT grassNum;
	D3DXCOLOR brushColor;
};