#include "stdafx.h"
#include "Tower.h"
#include "Environment/Terrain.h"
#include "Utilities/BinaryFile.h"
#include "Viewer\Viewport.h"

Tower::Tower(Shader* modelShader,Model * model, ModelRender* modelRendere, Terrain* terrain)
	:modelShader(modelShader), model(model), towerRender(modelRendere)
{
	this->terrain = terrain;
	rotation = D3DXVECTOR3(0, Math::PI, 0.0f);
	scale = D3DXVECTOR3(0.02f, 0.02f, 0.02f);
	boxScale = D3DXVECTOR3(1, 1, 1);
	
	changeYZHigh.x = this->model->GetHighVal().x + 1.5f;
	changeYZHigh.y = this->model->GetHighVal().y + 14.0f;
	changeYZHigh.z = 3.5f;

	changeYZLow.x = this->model->GetLowVal().x - 1.5f;
	changeYZLow.y = this->model->GetLowVal().y + 3.0f;
	changeYZLow.z = this->model->GetLowVal().z + 4.3f;

	box = new InstanceCollisionBox(modelShader, changeYZHigh , changeYZLow);
	box->Color(0, 1, 0);  // �浹 �ڽ� ����
	box->InstancePass(8);  // �浹 �ڽ� �н� 1�� �����

	saveTower.reserve(100);  //���̺� �� ���� �̸� �Ҵ�. ���� ����ϱ�
	saveBox.reserve(100);
}

Tower::~Tower()
{
	SafeDelete(box);
}

void Tower::Update(Terrain* terrain)
{
	towerRender->Update();
	box->Update();
	Selected();
	EditTower();
	CheckHeight();
}

void Tower::PreRender()
{
	towerRender->Pass(10);
	towerRender->Render();
	box->Render();
}

void Tower::Render()
{
	towerRender->Pass(14);
	towerRender->Render();
	box->Render();
}

void Tower::InstanceTower(D3DXVECTOR3 & picked)
{
	UINT index = towerRender->AddTransform();
	Transform* transform = towerRender->GetTransform(index);

	transform->Position(picked);
	scale = D3DXVECTOR3(0.02f, 0.02f, 0.02f);
	rotation = D3DXVECTOR3(0, Math::PI, 0);
	boxScale = D3DXVECTOR3(1, 1, 1);
	transform->Scale(scale);
	transform->Rotation(rotation);

	towerRender->UpdateTransform();

	UINT boxIndex = box->PushTexture(0);//0���׳� �־������ν� �ؽ��Ĵ� ����
	Transform* boxPos = box->GetTransform(boxIndex);

	boxPos->Position(picked);
	boxPos->Scale(boxScale);
	boxPos->Rotation(0, 0, 0);
}

void Tower::Selected()
{
	UINT count = box->GetDrawCount();
	if (count == 0) //������ �ٷ� Ż��
		return;
	D3DXMATRIX W, V, P;
	D3DXVECTOR3 start, direction;

	D3DXMatrixIdentity(&W);
	V = Context::Get()->ViewMatrix();
	P = Context::Get()->ProjectionMatrix();

	Viewport* vp = Context::Get()->GetViewport();

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR3 n, f;

	mouse.z = 0.0f;
	Context::Get()->GetViewport()->Unproject(&n, mouse, W, V, P);

	mouse.z = 1.0f;
	Context::Get()->GetViewport()->Unproject(&f, mouse, W, V, P);

	direction = f - n;

	start = n;
	//�ӱ��� ������ isClick ���� �ȵǸ鼭 ȭ��� �ٸ��� ������ �ش� ��ŷ ����
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (box->Selected(start, direction, i))
				{
					isClick = true;
					selectIndex = i;
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isClick = true;
					else
						isClick = false;
				}
			}
		}
	}
}

void Tower::EditTower()
{
	if (isClick == true)
	{
		ImGui::Begin("ToweEdit");
		//Ÿ��
		editTrans = towerRender->GetTransform(selectIndex); //0
		boxEditTrans = box->GetTransform(selectIndex);

		editTrans->Position(&position);
		editTrans->Scale(&scale);
		editTrans->Rotation(&rotation);

		ImGui::SliderFloat3("position", (float*)&position, 3, 253);
		ImGui::SliderFloat3("scale", (float*)&scale, 0.02f, 0.5f);
		ImGui::SliderFloat3("rotation", (float*)&rotation, 0, Math::PI);

		editTrans->Position(position);
		editTrans->Scale(scale);
		editTrans->Rotation(rotation);
		//�ڽ�
		boxScale = scale * 50;
		boxEditTrans->Position(position);
		boxEditTrans->Scale(boxScale);
		boxEditTrans->Rotation(rotation);
		
		towerRender->UpdateTransform();
		//����
		if (Keyboard::Get()->Down(VK_DELETE))
		{
			editTrans->Position(0, 0, 0);
			editTrans->Scale(0, 0, 0);
			editTrans->Rotation(0, 0, 0);

			boxEditTrans->Position(0, 0, 0);
			boxEditTrans->Scale(0, 0, 0);
			boxEditTrans->Rotation(0, 0, 0);

			towerRender->SaveDeleteIndex(selectIndex);
			box->SaveDeleteIndex(selectIndex);

			towerRender->UpdateTransform();
			isClick = false;
		}

		ImGui::End();
	}
}

void Tower::CheckHeight()
{
	for (UINT i = 0; i < towerRender->GetDrawCount(); i++)
	{
		editTrans = towerRender->GetTransform(i);
		boxEditTrans = box->GetTransform(i);
		Vector3 tempPos;
		Vector3 boxpos;
		editTrans->Position(&tempPos);
		boxEditTrans->Position(&boxpos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y;
		boxpos = tempPos;
		editTrans->Position(tempPos);
		boxEditTrans->Position(boxpos);
		towerRender->UpdateTransform();
	}
}

void Tower::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);
	w->Int(towerRender->GetDrawCount());

	D3DXVECTOR3 saveTowerPos;
	D3DXVECTOR3 saveTowerScale;
	D3DXVECTOR3 saveTowerRotation;

	for (UINT i = 0; i < towerRender->GetDrawCount(); i++)
	{
		//Ÿ��
		editTrans = towerRender->GetTransform(i);
		editTrans->Position(&saveTowerPos);
		editTrans->Scale(&saveTowerScale);
		editTrans->Rotation(&saveTowerRotation);
		w->Vector3(saveTowerPos);
		w->Vector3(saveTowerScale);
		w->Vector3(saveTowerRotation);
	}
	for (UINT i = 0; i < towerRender->GetDrawCount(); i++)
	{
		//�ڽ�
		boxEditTrans = box->GetTransform(i);
		boxEditTrans->Position(&saveTowerPos);
		boxEditTrans->Scale(&saveTowerScale);
		boxEditTrans->Rotation(&saveTowerRotation);
		w->Vector3(saveTowerPos);
		w->Vector3(saveTowerScale);
		w->Vector3(saveTowerRotation);
	}
	w->Close();
}

void Tower::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);
	
	loadTowerCount = r->Int();
	//Ÿ��
	for (UINT i = 0; i < loadTowerCount; i++)
	{
		UINT index = towerRender->AddTransform();
		Transform* transform = towerRender->GetTransform(index);

		//��ġ
		position = r->Vector3(); //�޾ƿͼ�
		transform->Position(position);  //�־��ְ�
		saveTower.push_back(position);  //Ǫ��
		//ũ��
		scale = r->Vector3();
		transform->Scale(scale);
		saveTower.push_back(scale);
		//ȸ��
		rotation = r->Vector3();
		transform->Rotation(rotation);
		saveTower.push_back(rotation);

		towerRender->UpdateTransform();
	}
	//�ڽ�
	for (UINT i = 0; i < loadTowerCount; i++)
	{
		UINT boxIndex = box->Push();
		Transform* boxPos = box->GetTransform(boxIndex);
		//��ġ
		position = r->Vector3();
		boxPos->Position(position);
		saveBox.push_back(position);
		//ũ��
		boxScale = r->Vector3();
		boxPos->Scale(boxScale);
		saveBox.push_back(boxScale);
		//ȸ��
		rotation = r->Vector3();
		boxPos->Rotation(rotation);
		saveBox.push_back(rotation);

	}
	r->Close();
}
