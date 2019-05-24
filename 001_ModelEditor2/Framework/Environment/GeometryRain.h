#pragma once

class GeometryRain : public Renderer
{
public:
	GeometryRain(D3DXVECTOR3& extent, UINT count);
	~GeometryRain();

	void Update();
	void Render();

private:
	void CreateData();

private:
	struct BufferDesc
	{
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1); //����
		D3DXVECTOR3 Velocity = D3DXVECTOR3(-10, -100, 0); //�ӵ�
		float DrawDistance = 1000.0f;  //�׷��� ����??

		D3DXVECTOR3 Origin = D3DXVECTOR3(0, 0, 0);  //���� ����
		float Padding;

		D3DXVECTOR3 Extent = D3DXVECTOR3(0, 0, 0);  //ũ��
		float Padding2;
	}bufferDesc;

	struct VertexRain  //���̴��� �о��� ���ؽ�
	{
		D3DXVECTOR3 Position;
		//D3DXVECTOR2 Uv;
		D3DXVECTOR2 Scale;
	};
private:
	ConstantBuffer* buffer;// c���� �о��ֱ�
	ID3DX11EffectConstantBuffer* sBuffer;//

	//VertexRain* vertices;

	Texture* texture;//

	UINT drawCount;
	vector<VertexRain> vertices;
};