#include "Framework.h"
#include "Context.h"
#include "Viewer/Viewport.h"
#include "Viewer/Perspective.h"
#include "Viewer/Freedom.h"
#include "Viewer/Following.h"

Context* Context::instance = NULL;

Context * Context::Get()
{
	assert(instance != NULL);

	return instance;
}

void Context::Create()
{
	assert(instance == NULL);

	instance = new Context();
}

void Context::Delete()
{
	SafeDelete(instance);
}

Context::Context()
{
	D3DDesc desc = D3D::GetDesc();

	projection = new Perspective(desc.Width, desc.Height);
	viewport = new Viewport(desc.Width, desc.Height);
	camera = new Freedom();

	lightAmbient = D3DXCOLOR(0, 0, 0, 1);
	lightSpecular = D3DXCOLOR(1, 1, 1, 1);
	lightDirection = D3DXVECTOR3(-1, -1, 1);
	lightPosition = D3DXVECTOR3(0, 0, 0);

	pointLightCount = 0;
	spotLightCount = 0;
	spotIndex = 0;
	pointIndex = 0;

	subCamera = NULL;
	subPerspective = NULL;
}

Context::~Context()
{
	SafeDelete(camera);
	SafeDelete(projection);
	SafeDelete(viewport);
}

void Context::Update()
{
	ImGui::SliderFloat3("Light Direction", (float*)&lightDirection, -1, 1);
	camera->Update();
}

void Context::Render()
{
	SetMainCamera();
	SetMainPerspective();

	viewport->RSSetViewport();
}

D3DXMATRIX Context::ViewMatrix()
{
	D3DXMATRIX view;

	if (subCamera != NULL)
		subCamera->GetMatrix(&view);
	else
		camera->GetMatrix(&view);

	return view;
}

D3DXMATRIX Context::ProjectionMatrix()
{
	D3DXMATRIX temp;

	if (subPerspective != NULL)
		subPerspective->GetMatrix(&temp);
	else
		projection->GetMatrix(&temp);

	return temp;
}

UINT Context::PointLights(OUT PointLight * lights)
{
	//���̴��� �о��ֱ� ����
	memcpy(lights, pointLights, sizeof(PointLight) * pointIndex);

	return pointIndex;
}

void Context::AddPointLight(PointLight & light)
{
	if (savePointIndex.size() > 0)
	{
		pointLightCount++;
		int temp = savePointIndex.back();
		savePointIndex.pop_back();
		pointLights[temp] = light;
	}
	else
	{
		pointIndex = pointLightCount;
		pointLights[pointIndex] = light;
		pointLightCount++;
		pointIndex++;
	}
}


PointLight & Context::GetPointLight(UINT index)
{
	return pointLights[index];
}

void Context::DeletePointLight(UINT index)
{
	savePointIndex.push_back(index);
	//pointLights[index].Intensity = 0; //�̰� ������.. ���� �� ���ư�
	pointLights[index].Color = Color(0, 0, 0, 1);
	pointLights[index].Position = Vector3(0, 0, 0);
	pointLights[index].Range = 0;
	pointLightCount--;
}

UINT Context::SpotLights(OUT SpotLight * lights)
{
	memcpy(lights, spotLights, sizeof(SpotLight) * spotIndex);

	return spotIndex;
}

SpotLight & Context::GetSpotLight(UINT index)
{
	return spotLights[index];
}

void Context::AddSpotLight(SpotLight & light)
{
	if (saveSpotIndex.size() > 0)
	{
		spotLightCount++;
		int temp = saveSpotIndex.back();
		saveSpotIndex.pop_back();
		spotLights[temp] = light;
	}
	else
	{
		spotIndex = spotLightCount;
		spotLights[spotIndex] = light;
		spotLightCount++;
		spotIndex++;
	}
}
void Context::DeleteSpotLight(UINT index)
{
	saveSpotIndex.push_back(index);  //���� ���� �ʾƵ� ���̴����� ��� ���� �Ѿ�� �ٶ��� ������ �� ����
	spotLights[index].Angle = 0;
	spotLights[index].Color = Color(0,0,0, 1);
	spotLights[index].Direction = Vector3(0,0,0);
	spotLights[index].Intensity = 0;
	spotLights[index].Position = Vector3(0,0,0);
	spotLights[index].Range = 0;
	spotLightCount--;
	//if (index == MAX_SPOT_LIGHT - 1)
	//{
	//	spotLights[index].Angle = 0;
	//	spotLights[index].Color = Color(0,0,0, 1);
	//	spotLights[index].Direction = Vector3(0,0,0);
	//	spotLights[index].Intensity = 0;
	//	spotLights[index].Position = Vector3(0,0,0);
	//	spotLights[index].Range = 0;
	//	spotLightCount--;
	//	return;
	//}
	//else
	//{
	//	for (int i = index; i < spotLightCount; i++)
	//	{
	//		spotLights[i] = spotLights[i + 1];
	//	}
	//	spotLightCount--;
	//}
}

