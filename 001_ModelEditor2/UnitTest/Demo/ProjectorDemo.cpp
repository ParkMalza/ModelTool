#include "stdafx.h"
#include "ProjectorDemo.h"
#include "Environment/Sky.h"
#include "Viewer/Projector.h"

void ProjectorDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0);
	Context::Get()->GetCamera()->Position(0, 46, -85);

	shader = new Shader(L"037_Projector.fx");
	projector = new Projector(shader, L"Environment/MagicCircle.png", 15, 15);
	projector->GetCamera()->Position(-20, 10, 0);

	//Sky
	{
		sky = new Sky(L"Environment/DesertCube1024.dds");
	}

	//Create Mesh
	{
		floor = new Material(shader);
		floor->Specular(1, 1, 1);
		floor->DiffuseMap("Floor.png");
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");

		stone = new Material(shader);
		stone->Diffuse(0.8f, 0.7f, 0.7f);
		stone->DiffuseMap("Stones.png");
		stone->SpecularMap("Stones_Specular.png");
		stone->NormalMap("Stones_Normal.png");

		brick = new Material(shader);
		brick->Specular(1, 0, 0, 20);
		brick->DiffuseMap("Bricks.png");
		brick->SpecularMap("Bricks_Specular.png");
		brick->NormalMap("Bricks_Normal.png");

		wall = new Material(shader);
		wall->Specular(1, 0, 0, 20);
		wall->DiffuseMap("Wall.png");
		wall->SpecularMap("Wall_Specular.png");
		wall->NormalMap("Wall_Normal.png");


		cube = new MeshCube(shader);
		cube->GetTransform()->Position(0, 2.5f, 0);
		cube->GetTransform()->Scale(15.0f, 5.0f, 25.0);

		grid = new MeshGrid(shader, 3, 3);
		grid->GetTransform()->Position(0, 0, 0);
		grid->GetTransform()->Scale(20, 1, 20);


		for (UINT i = 0; i < 5; i++)
		{
			cylinder[i * 2] = new MeshCylinder(shader, 0.5f, 3.0f, 20, 20);
			cylinder[i * 2]->GetTransform()->Position(-30, 6.0f, -15.0f + (float)i * 15.0f);
			cylinder[i * 2]->GetTransform()->Scale(5, 5, 5);

			cylinder[i * 2 + 1] = new MeshCylinder(shader, 0.5f, 3.0f, 20, 20);
			cylinder[i * 2 + 1]->GetTransform()->Position(30, 6.0f, -15.0f + (float)i * 15.0f);
			cylinder[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);


			sphere[i * 2] = new MeshSphere(shader, 0.5f, 20, 20);
			sphere[i * 2]->GetTransform()->Position(-30.0f, 15.5f, -15.0f + i * 15.0f);
			sphere[i * 2]->GetTransform()->Scale(5, 5, 5);

			sphere[i * 2 + 1] = new MeshSphere(shader, 0.5f, 20, 20);
			sphere[i * 2 + 1]->GetTransform()->Position(30.0f, 15.5f, -15.0f + i * 15.0f);
			sphere[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);
		}
	}

	//Load Model
	{
		model[0] = new Model();
		model[0]->ReadMaterial(L"Eclipse/Eclipse");
		model[0]->ReadMesh(L"Eclipse/Eclipse");

		modelRender[0] = new ModelRender(shader, model[0]);
		modelRender[0]->Pass(3);

		UINT index = modelRender[0]->AddTransform();
		modelRender[0]->GetTransform(index)->Position(0, 4.25f, -1);
		modelRender[0]->GetTransform(index)->Scale(0.3f, 0.3f, 0.3f);
		modelRender[0]->UpdateTransform();


		model[1] = new Model();
		model[1]->ReadMaterial(L"Kachujin/Mesh");
		model[1]->ReadMesh(L"Kachujin/Mesh");

		modelRender[1] = new ModelRender(shader, model[1]);

		index = modelRender[1]->AddTransform();
		modelRender[1]->GetTransform(index)->Position(10.0f, 0, 0);
		modelRender[1]->GetTransform(index)->Scale(0.05f, 0.05f, 0.05f);
		modelRender[1]->UpdateTransform();
	}



	//Add PointLight
	{
		PointLight light = { Color(1, 0, 0, 1), Vector3(20, 1, 0), 20, 1.0f };
		Context::Get()->AddPointLight(light);
	}

	//Add SpotLight
	{
		//SpotLight light = { Color(0, 0, 1, 0), Vector3(0, 5, 0), 3, Vector3(0, -1, 0), 0.3f, 1.0f };
		//Context::Get()->AddSpotLight(light);
	}
}

void ProjectorDemo::Destroy()
{

}

void ProjectorDemo::Update()
{
	sky->Update();

	for (UINT i = 0; i < 10; i++)
	{
		sphere[i]->Update();
		cylinder[i]->Update();
	}

	cube->Update();
	grid->Update();

	modelRender[0]->Update();
	modelRender[1]->Update();

	projector->Update();
}

void ProjectorDemo::PreRender()
{

}

void ProjectorDemo::Render()
{
	UINT meshPass = 0;
	UINT modelPass = 1;

	sky->Render();

	wall->Render();
	for (UINT i = 0; i < 10; i++)
	{
		sphere[i]->Pass(meshPass);
		sphere[i]->Render();
	}

	brick->Render();
	for (UINT i = 0; i < 10; i++)
	{
		cylinder[i]->Pass(meshPass);
		cylinder[i]->Render();
	}

	stone->Render();
	cube->Pass(meshPass);
	cube->Render();

	floor->Render();
	grid->Pass(meshPass);
	grid->Render();

	modelRender[0]->Pass(modelPass); 
	modelRender[0]->Render();


	modelRender[1]->Pass(modelPass);  
	modelRender[1]->Render();
}

void ProjectorDemo::PostRender()
{

}

void ProjectorDemo::RenderObject()
{
	sky->Render();

	wall->Render();
	for (UINT i = 0; i < 10; i++)
		sphere[i]->Render();


	brick->Render();
	for (UINT i = 0; i < 10; i++)
		cylinder[i]->Render();


	stone->Render();
	cube->Render();

	floor->Render();
	grid->Render();
}
