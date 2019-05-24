#pragma once
#include "Systems/IExecute.h"

class Test : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	Shader* integrationShader;
	//RenderTarget
	class RTVMinimap* renderTarget;
	//Weather
	class Sky* sky;
	//Terrain
	Shader* terrainShader;
	class Terrain* terrain;
	//model
	Shader* modelShader;
	class ModelManager* modelManager;
	//modelEditor
	class ModelEditor* modelEditor;
	//Shadow
	Shadow* shadow;
private:
	//���� ����
	void SetImgui();
	//Save
	void Save(wstring savePath);
	void Load(wstring savePath);
	void RenderObject();
	void PreRenderObject();
	void SetPass();

private:
	//���̾� ���� �Ѱ��ַ��� �̸� ��Ƴ���
	vector<wstring> textureNames;
	//�޽� ���� �Ѱ��ַ��� �̸� ��Ƴ���
	vector<wstring> meshTexturesNames;
	
private:
	wstring startTerrain;
	wstring paint;
private:
	D3DXVECTOR3 picked;

};