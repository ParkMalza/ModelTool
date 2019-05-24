#pragma once

class BillBoard :public Renderer
{
public:
	BillBoard(Shader* shader, vector<wstring>& textureNames, class Terrain* terrain);
	~BillBoard();

	void FixedY(bool val) { fixedY = val; }
	void Add(D3DXVECTOR3& position, D3DXVECTOR2& scale);
	void Add(D3DXVECTOR3& position, D3DXVECTOR2& scale, UINT& type);

	void CreateBuffer();
	void Update();
	void Render();

	void Save(wstring fileName);
	void Load(wstring fileName);

public:
	//���� �Ѱ��ֱ�
	void Texture0(Texture** texture) { *texture = grass; }
	void Texture1(Texture** texture) { *texture = grass1; }
	void Texture2(Texture** texture) { *texture = grass2; }
	void Texture3(Texture** texture) { *texture = grass3; }
	void Texture4(Texture** texture) { *texture = grass4; }
	void MakeNewGrass();
public:
	void SetOldBillBoard();  //Ŭ���� ����.. redo�� ������ ����
	void MakeOldBillBoard();
	void MakeNewBillBoard();

	void UndoBillBoard(); // c + z
	void RedoBillBoard(); // c + y


private:
	struct VertexScale
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 scale;
		UINT type;  //�̰ɷ� ���� ����
	};

private:
	class Terrain* terrain;
	bool fixedY;

	vector<VertexScale> vertices;
	
	TextureArray* textures;
	Texture* grass;
	Texture* grass1;
	Texture* grass2;
	Texture* grass3;
	Texture* grass4;
	Texture* grass5;
	Texture* grass6;
	Texture* grass7;
	Texture* grass8;
	vector<Texture*> images;

private:
	vector<VertexScale> oldBillBorad;
	vector<VertexScale> newBillBorad;
	vector<vector<VertexScale>> undoStack; 
	vector<vector<VertexScale>> redoStack;
	vector<vector<VertexScale>>::iterator it;
};