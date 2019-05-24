#pragma once

class Terrain : public Renderer
{
public:
	typedef VertexColorTextureNormalTangent TerrainVertex;

public:
	Terrain(Shader* shader, wstring heightMap, wstring alphaMap, vector<wstring>& textureNames);
	~Terrain();

	void Update() override;
	void Render() override;
	void PreRender();

	void BrushType(UINT val) { brushDesc.Type = val; }
	void BrushLocation(D3DXVECTOR3& vec) { brushDesc.Location = vec; }
	void BrushRange(UINT val) { brushDesc.Range = val; }
	void BrushColor(D3DXCOLOR& color) { brushDesc.Color = color; }
	void BrushRange(UINT* val) { *val = brushDesc.Range; }
	void BrushColor(D3DXCOLOR* color) { *color = brushDesc.Color; }

	void GridLineVisible(UINT val) { gridLineDesc.Visible = val; }
	void GridLineThick(float val) { gridLineDesc.Thickness = val; }
	void GridLineSize(float val) { gridLineDesc.Size= val; }

	const wstring GetBaseTextureName() { return baseTextureName; }
	const wstring GetLayerTextureName() { return layerTextureName; }
	const wstring GetAlphaTextureName() { return alphaTextureName; }
public:
	float GetHeight(D3DXVECTOR3& position);
	float GetPickedHeight(D3DXVECTOR3& position);
	D3DXVECTOR3 GetPickedHeight();

	void RaiseHeight(D3DXVECTOR3& position, UINT type, UINT range, float speed);
	void FallDownHeight(D3DXVECTOR3& position, UINT type, UINT range, float speed);
	void LockHeight(D3DXVECTOR3& position, UINT type, UINT range, float _height, float speed);
	void Smoothing(D3DXVECTOR3& position, UINT type, UINT range, float speed);
	//���� ��ü
	void BaseMap(wstring file);  
	void LayerMap(wstring file);
	void LayerMaps(wstring file, int i);
	//���� �޾ƿ�����
	void GetBaeMap(Texture** texture);
	void LayerMap1(Texture** texture);
	void LayerMap2(Texture** texture);
	void LayerMap3(Texture** texture);
	void LayerMap4(Texture** texture);

	void BrushLayer(D3DXVECTOR3& position, UINT type, UINT range, float power, UINT layerNum);
	void SettingAlpha(float alpha);

	void Save(wstring fileName);
	void Load(wstring fileName);

	void Mapping();
	void CreateNormalData();

public:
	void SetOldTerrain();  //Ŭ���� ����.. redo�� ������ ����
	void MakeOldTerrain();
	void MakeNewTerrain();

	void UndoTerrain(); // c + z
	void RedoTerrain(); // c + y

private:
	vector<TerrainVertex*> undoStack;
	vector<TerrainVertex*> redoStack;
	vector<TerrainVertex*>::iterator it;

private:
	void CreateVertexData();
	void CreateIndexData();

private:
	struct BrushDesc
	{
		//16����Ʈ�� ����� �Ѵ�. 
		//���� ���̴����� �׷��ʿ�� ����
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);
		D3DXVECTOR3 Location = D3DXVECTOR3(-1, -1, -1);
		int Type = 0;
		int Range = 1;

		float Padding[3];//16����Ʈ ���߱� ���� �����Ⱚ  //���⼭ 12����Ʈ �߰������
	} brushDesc;

	struct GridLineDesc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		int Visible = 1; //������ ���̳�
		float Thickness = 0.1f; //�׷����� ���� �β�
		float Size = 255; //size ũ�Ⱑ �׸��� ��ĭ�� �� ����

		float Padding;
	} gridLineDesc;

	float alpha;

	bool isBase;

private:
	Texture* heightMap;
	Texture* layer[4];

	UINT width, height;
	TerrainVertex* vertices;
	TerrainVertex* oldTerrainVertices;
	TerrainVertex* newTerrainVertices;

	UINT* indices;

	wstring baseTextureName;
	wstring layerTextureName;
	wstring alphaTextureName;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* gridLineBuffer;
	ID3DX11EffectConstantBuffer* sGridLineBuffer;

	Texture* baseMap;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* alphaMap;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;

	TextureArray* layerMaps;
	ID3DX11EffectShaderResourceVariable* sLayerMaps;

	ID3DX11EffectScalarVariable * sIndex;

	vector<wstring> layersNames;
};