#pragma once

class Command;
class BillBoardsRender : public Command
{
public:
	BillBoardsRender(class Projector* projector, class Terrain* terrain);
	~BillBoardsRender();

	void Update();
	void Render();
	void SettingImgui(class Terrain* terrain, D3DXVECTOR3& picked);
	void SettingBillboard(class Terrain* terrain, D3DXVECTOR3& picked);
	void MakeRandomBillboard(D3D11_BOX & box, class Terrain* terrain, D3DXVECTOR3& picked);

	void AddGrass(wstring file);

	void Save(wstring name);
	void Load(wstring name);

	// Command��(��) ���� ��ӵ�
	virtual void Undo() override;

	virtual void Redo() override;

	virtual void Excute() override;

private:
	class Terrain* terrain;

	Texture* tex;  //� Ǯ ����ϰ� �ִ��� �̹��� �޾ƿ�����
	Shader* shader;
	class BillBoard* billBoard;
	class Projector* projector;

private:
	UINT selectGrass;  //���° Ǯ �����ߴ���
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