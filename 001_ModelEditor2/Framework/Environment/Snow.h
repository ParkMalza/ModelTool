#pragma once

class Snow : public Renderer
{
public:
	Snow(D3DXVECTOR3& extent, UINT count);
	~Snow();

	void Update();
	void Render();

private:
	void CreateData();
private:
	struct BufferDesc
	{
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1); //����
		D3DXVECTOR3 Velocity = D3DXVECTOR3(0, -5, 0); //�ӵ�
		float DrawDistance = 1000.0f;  //�׷��� ����??

		D3DXVECTOR3 Origin = D3DXVECTOR3(0, 0, 0);  //���� ����
		float Turbulence = 5;

		D3DXVECTOR3 Extent = D3DXVECTOR3(0, 0, 0);  //ũ��
		float Padding2;
	}bufferDesc;

	struct VertexSnow  //���̴��� �о��� ���ؽ�
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 Uv;
		D3DXVECTOR2 Random;
		float Scale;
	};
private:
	ConstantBuffer* buffer;// c���� �о��ֱ�
	ID3DX11EffectConstantBuffer* sBuffer;//

	VertexBuffer* vertexBuffer;//
	IndexBuffer* indexBuffer;//

	VertexSnow* vertices;
	UINT* indices;

	Texture* texture;//

	UINT drawCount;
};