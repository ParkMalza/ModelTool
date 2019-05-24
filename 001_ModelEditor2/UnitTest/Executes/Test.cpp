#include "stdafx.h"
#include "Test.h"
#include "Utilities/Xml.h"
#include "Environment/Sky.h"
#include "Environment/Terrain.h"
#include "Models/ModelManager.h"
#include "Models/ModelEditor.h"

void Test::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(12, 0, 0);
	//Context::Get()->GetCamera()->Position(102, 23, -2);
	Context::Get()->GetCamera()->Position(30, 23, -8);
	
	integrationShader = new Shader(L"000_ModelEditor.fx");

	//Sky
	{
		sky = new Sky(L"Environment/SnowCube1024.dds");

	}

	//Terrain
	{
		//terrainShader = new Shader(L"000_PPintegration.fx");

		//Projector
		wstring terrainBrushImage = L"Environment/MagicQuad.jpg";
		wstring terrainBrushImage2 = L"Environment/MagicCircle.png";
		vector<wstring> bImages;
		bImages.push_back(terrainBrushImage);
		bImages.push_back(terrainBrushImage2);

		startTerrain = L"HeightMaps/BaseMap256.png";
		paint = L"Terrain/Dirt.png";

		for (int i = 0; i < 5; i++)
			textureNames.push_back(paint);


		terrain = new Terrain(integrationShader, startTerrain, paint, textureNames);
		terrain->BaseMap(paint);
		terrain->LayerMap(paint);

	}
	//Model
	{
	
		//modelShader = new Shader(L"039_Model_Instance.fx");
		modelManager = new ModelManager(integrationShader, terrain);
	}
	//ModelEditor
	{
		modelEditor = new ModelEditor(integrationShader);
	}
	////Shadow
	//{
		shadow = new Shadow(integrationShader, Vector3(130, 0, 0), 200, 2048, 2048);
	//}
}

void Test::Destroy()
{
	//SafeDelete(textureCube);
	SafeDelete(shadow);
	//SafeDelete(cubeShader);
	SafeDelete(modelManager);
	//SafeDelete(modelShader);
	//SafeDelete(billBoardRender);
	//SafeDelete(instanceShader);
	SafeDelete(terrain);
	//SafeDelete(terrainShader);
	SafeDelete(sky);
	//SafeDelete(renderTarget);
	SafeDelete(integrationShader);
}

void Test::Update()
{
	picked = terrain->GetPickedHeight();
	ImGui::LabelText("Picking", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);

	sky->Update();
	terrain->Update();

	SetImgui();
	modelManager->Update(terrain);
	modelEditor->Update();

	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down('Z'))
			COMMANDMANAGER->Undo();
		if (Keyboard::Get()->Down('Y'))
			COMMANDMANAGER->Redo();
	}
	SetPass();

}

void Test::PreRender()
{
	shadow->Set();
	PreRenderObject();
}

void Test::Render()
{
	RenderObject();
}

void Test::PostRender()
{
	//renderTarget->PostRender();
}

void Test::SetImgui()
{
	ImGui::Begin("Menu");
	if (ImGui::BeginTabBar("Tabs"))
	{
		//Model
		modelManager->SetImgui(picked);

		ImGui::EndTabBar();
	} ImGui::Separator();
	
	if (ImGui::Button("Save"))  //��ü ���̺�
	{
		D3DDesc desc = D3D::GetDesc();
		function<void(wstring)> func = bind(&Test::Save, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::XmlFilter, L"../XmlFolder/", func, desc.Handle);
	}
	if (ImGui::Button("Load"))  //��ü �ε�
	{
		D3DDesc desc = D3D::GetDesc();
		function<void(wstring)> func = bind(&Test::Load, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::XmlFilter, L"../XmlFolder/", func, desc.Handle);
	}

	ImGui::End();
}

void Test::PreRenderObject()
{
	terrain->PreRender();
	modelManager->PreRender();
	modelEditor->PreRender();
}

void Test::RenderObject()
{
	sky->Render();
	terrain->Render();
	modelManager->Redner();
	modelEditor->Render();
}

void Test::SetPass()
{
	static int pass = 0;
	ImGui::Text("Terrain");
	ImGui::SliderInt("Pass", (int *)&pass, 0, 1);
	terrain->Pass(pass);
}

//��ü ���̺�
void Test::Save(wstring savePath)
{
	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));
	file = file + ".xml";

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement(file.c_str());

	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	// mesh
	Xml::XMLElement* element = document->NewElement("Meshes");
	Xml::XMLElement* child = NULL;

	wstring mesh = Path::GetFileName(savePath);
	mesh = mesh + L".mesh";
	string save = String::ToString(mesh);

	D3DXVECTOR3 pos(0,0,0);
	D3DXVECTOR3 scale(0,0,0);
	D3DXVECTOR3 rotation(0,0,0);
	
	element->SetAttribute("file", save.c_str());
	// element�� �ڽ� child �� �ֱ�
	child = document->NewElement("Cube");
	child->SetAttribute("CubePosX", pos.x);
	element->LinkEndChild(child);
	//
	root->LinkEndChild(element);

	//meshRender->Save(mesh);  //�� ����

	//height
	element = document->NewElement("Height");
	wstring height = Path::GetFileName(savePath);
	height = height + L".height";
	save = String::ToString(height);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	terrain->Save(height);  //�� ����

	//grass
	element = document->NewElement("Grass");
	wstring grass = Path::GetFileName(savePath);
	grass = grass + L".grass";
	save = String::ToString(grass);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	//billboardsInstancing->Save(grass);

	//Tank
	element = document->NewElement("Tank");
	wstring tank = Path::GetFileName(savePath);
	tank = tank + L".tank";
	save = String::ToString(tank);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	modelManager->tankSave(tank);  //�� ����

	//Tower
	element = document->NewElement("Tower");
	wstring tower = Path::GetFileName(savePath);
	tower = tower + L".tower";
	save = String::ToString(tower);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	modelManager->towerSave(tower);  //�� ����

	//Light
	element = document->NewElement("Light");
	wstring light = Path::GetFileName(savePath);
	light = light + L".light";
	save = String::ToString(light);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	//lights->Save(light);  //�� ����


	document->SaveFile((folder + file).c_str());
}
//��ü �ε�
void Test::Load(wstring savePath)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(savePath).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* element = root->FirstChildElement();
	Xml::XMLElement* child = element->FirstChildElement();
	D3DXVECTOR3 pos;
	//ù��° ���� mesh
	{
		const char* mesh = element->Attribute("file");
		pos.x = child->FloatAttribute("CubePosX");  //save���� CubePosX �̸����缭
		string loadMesh = mesh;

		wstring file = String::ToWString(loadMesh);
		//meshRender->Load(file);

		element = element->NextSiblingElement();
	}
	//�ι�° ���� height
	{
		const char* height = element->Attribute("file");
		string loadHeight = height;

		wstring file2 = String::ToWString(loadHeight);
		terrain->Load(file2);

		element = element->NextSiblingElement();
	}
	//3��° ���� grass
	{
		const char* grass = element->Attribute("file");
		string loadGrass = grass;

		wstring file3 = String::ToWString(loadGrass);

		//billboardsInstancing->Load(file3);

		element = element->NextSiblingElement();
	}
	//4��° ���� tank
	{
		const char* tank = element->Attribute("file");
		string loadTank = tank;

		wstring file5 = String::ToWString(loadTank);
		modelManager->tankLoad(file5);

		element = element->NextSiblingElement();
	}
	//5��° ���� tower
	{
		const char* tower = element->Attribute("file");
		string loadTower = tower;

		wstring file5 = String::ToWString(loadTower);
		modelManager->towerLoad(file5);

		element = element->NextSiblingElement();
	}
	//6��° ���� light
	{
		const char* light = element->Attribute("file");
		string loadLight = light;

		wstring file6 = String::ToWString(loadLight);
		//lights->Load(file6);

		element = element->NextSiblingElement();
	}
}
