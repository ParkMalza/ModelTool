#pragma once

class Texture
{
public:
	friend class Textures;
	//friend 클래스는 서로간의 private조차도 공유가 가능하다
public:
	static void SaveFile(wstring file, ID3D11Texture2D* src);
	//ID3D11Texture2D : 2D 텍스처 인터페이스는 구조화 된 메모리인 Texel Data를 관리합니다.
	static D3D11_TEXTURE2D_DESC ReadPixels(ID3D11Texture2D* src, DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels);
	//D3D11_TEXTURE2D_DESC : 텍스쳐를 설명합니다. 텍스쳐의 정보...
	//DXGI_FORMAT : 형식 열거

public:
	Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);
	~Texture();

	operator ID3D11ShaderResourceView*(){ return view; }

	wstring GetFile() { return file; }

	UINT GetWidth() { return metaData.width; }
	UINT GetHeight() { return metaData.height; }

	D3D11_TEXTURE2D_DESC ReadPixels(DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels);
	void SaveFile(wstring file);

	void GetImageInfo(DirectX::TexMetadata* data)
	{
		*data = metaData;
	}

	ID3D11ShaderResourceView* SRV() { return view; }

private:
	wstring file;

	DirectX::TexMetadata metaData;
	ID3D11ShaderResourceView* view;
};

struct TextureDesc
{
	wstring file;
	UINT width, height;
	DirectX::TexMetadata metaData;
	ID3D11ShaderResourceView* view;

	bool operator==(const TextureDesc& desc)
	{
		bool b = true;
		b &= file == desc.file;
		b &= width == desc.width;
		b &= height == desc.height;

		return b;
	}
};

class Textures
{
public:
	friend class Texture;

public:
	static void Create();
	static void Delete();

private:
	static void Load(Texture* texture, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);

private:
	static vector<TextureDesc> descs;
};

class TextureArray
{
public:
	TextureArray(vector<wstring>& names, UINT width = 256, UINT height = 256, UINT mipLevels = 1);
	~TextureArray();

	ID3D11ShaderResourceView* SRV() { return srv; }

private:
	vector<ID3D11Texture2D *> CreateTextures(vector<wstring>& names, UINT width, UINT height, UINT mipLevels);

private:
	ID3D11ShaderResourceView* srv;
};