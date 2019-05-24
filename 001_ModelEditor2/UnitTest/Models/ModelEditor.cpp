#include "stdafx.h"
#include "ModelEditor.h"
#include "Assimp/Loader.h"
#include "Models/Kachujin.h"
#include "Model/ModelMesh.h"
#include "Model/ModelClip.h"
#include "EnumGroup.h"

ModelEditor::ModelEditor(Shader * shader)
	:shader(shader), modelRender(NULL), modelAnimator(NULL), model(NULL), aniModel(NULL)
	, selectAni(0), clip(0)
	, blendTime(0.18f), frameSpeed(1.0f), modelIndex(0), aniModelIndex(0), currFrame(0)
	, isEdit(false), boneNum(0), countBone(0), importInfo(false), selectBone(0)
	,importPos(0, 0, 0), importScale(1, 1, 1), importRotation(0, 0, 0), current_state("")
	,editPos(0,0,0), editScale(1, 1, 1), editRotation(0,0,0), isShowBones(false)
{
	boneBox = new InstanceCollisionBox(shader, 1.0f, 1.0f, 1.0f);
	boneBox->InstancePass(17);
	exportName = L"";
	directoryBeforFbx = L"";

	vecBone.reserve(70);
}

ModelEditor::~ModelEditor()
{
	SafeDelete(boneBox);
}

void ModelEditor::Update()
{
	ImportModel();
	SetImport();
	SetAnimationImport();
	if (modelAnimator != NULL)
	{
		ImGui::Checkbox("Show Bones", &isShowBones);

		Hierarchy();
		ChangeAni();
		//TPose();
		NoneTPose();
		bonePicking();
		modelAnimator->Update();

		Transform* editAni = modelAnimator->GetTransform(0);
		editAni->Position(&editPos);
		editAni->Scale(&editScale);
		editAni->Rotation(&editRotation);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "AnimataionModel");
		ImGui::SliderFloat3("AniPosition", (float*)&editPos, -254.0f, 254.0f);
		ImGui::SliderFloat3("AniScale", (float*)&editScale, 0.1f, 1.0f);
		ImGui::SliderFloat3("AniRotation", (float*)&editRotation, 0.0f, Math::PI * 2);

		editAni->Position(editPos);
		editAni->Scale(editScale);
		editAni->Rotation(editRotation);
		for (UINT i = 0; i < boneNum; i++)
		{
			Transform* trans = boneBox->GetTransform(i);
			trans->Scale(editScale);
		}

		modelAnimator->UpdateTransform();
		ImGui::Separator();
		boneBox->Update();
	}		
	
	if (modelRender != NULL)
	{
		modelRender->Update();

		Transform* edit = modelRender->GetTransform(0);
		Vector3 tempPos;
		Vector3 tempScale;
		Vector3 tempRotation;
		edit->Position(&tempPos);
		edit->Scale(&tempScale);
		edit->Rotation(&tempRotation);
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Model");
		ImGui::SliderFloat3("ModelPosition", (float*)&tempPos, -254.0f, 254.0f);
		ImGui::SliderFloat3("ModelScale", (float*)&tempScale, 1.0f, 100.0f);
		ImGui::SliderFloat3("ModelRotation", (float*)&tempRotation, 0.0f, Math::PI * 2);
		edit->Position(tempPos);
		edit->Scale(tempScale);
		edit->Rotation(tempRotation);
		modelRender->UpdateTransform();
	}
}

void ModelEditor::PreRender()
{
	if (modelAnimator != NULL)
	{
		
			modelAnimator->Pass(11);  //애니용
			modelAnimator->Render();
		
		//else
		//{
		//	modelAnimator->Pass(10);  //가마니 용
		//	modelAnimator->Render();
		//}
	}
	if (modelRender != NULL)
	{
		modelRender->Pass(10);
		modelRender->Render();
	}
}

void ModelEditor::Render()
{
	if (modelAnimator != NULL)
	{
		
		
			modelAnimator->Pass(15); //애니용
			modelAnimator->Render();
		
		//else
		//{
		//	modelAnimator->Pass(14); //가마니 용
		//	modelAnimator->Render();
		//}
		if(isShowBones == true)
			boneBox->Render();
	}
	if (modelRender != NULL)
	{
		modelRender->Pass(14);
		modelRender->Render();
	}
}

void ModelEditor::SetImport()
{
	ImGui::Begin("ModelImport");
	if (ImGui::Button("Import") == true)
	{
		D3DDesc desc = D3D::GetDesc();
		function<void(wstring)> func = bind(&ModelEditor::ExportModel, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::FbxModelFilter, L"../../_Assets/", func, desc.Handle);
	}
	if (modelAnimator != NULL)
	{
		modelAnimator->RunningTime(&currFrame);
		ImGui::SliderInt("Current Frame", (int*)&currFrame, 0.0f, 255.0f);
		modelAnimator->RunningTime(currFrame);
	}
	ImGui::End();
}

void ModelEditor::SetAnimationImport()
{
	if (modelAnimator != NULL)
	{
		ImGui::Begin("AnimationImport");
		if (ImGui::Button("AnimationImport") == true)
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&ModelEditor::ImportAni, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::FbxModelFilter, L"../../_Assets/", func, desc.Handle);
		}
		ImGui::End();
	}
}

void ModelEditor::ExportModel(wstring fbxName) //exportName fullName
{
	exportName = Path::GetDirectoryFbxName(fbxName); // L카추진/0_메쉬.fbx, L탱크/탱크.fbx
	directoryBeforFbx = Path::GetDirectoryBeforeFbx(fbxName); // L카추진/, L탱크
	wstring path = L"../../_Assets/";
	path = path + directoryBeforFbx;
	wstring filter = Path::GetExtension(exportName);
	filter = L"*." + filter;
	Path::GetFiles(&getFolderImage, path, filter, false);  //getFolderImage : fullName
	importInfo = true;
}

void ModelEditor::ImportAni(wstring importName)
{
	exportName = Path::GetDirectoryFbxName(importName); // L카추진/0_메쉬.fbx, L탱크/탱크.fbx
	directoryBeforFbx = Path::GetDirectoryBeforeFbx(importName); // L카추진/, L탱크
	wstring FileName = Path::GetFileName(exportName);
	wstring folderFileName = directoryBeforFbx + FileName;

	Loader* loader = new Loader();
	loader = new Loader();
	loader->ReadFile(exportName); //without skin 했으므로 애니 정보만 있다~
	loader->ExportAnimClip(0, folderFileName);
	SafeDelete(loader);

	aniModel->ReadClip(folderFileName);

	SafeDelete(modelAnimator);
	modelAnimator = new ModelAnimator(shader, aniModel);

	modelAnimator->PlayNextClip(0, selectAni, blendTime);

	UINT index = modelAnimator->AddTransform();
	Transform* trans = modelAnimator->GetTransform(index);
	trans->Position(editPos);
	trans->Scale(editScale);
	trans->Rotation(editRotation);

	string tpose = String::ToString(Path::GetFileNameWithoutExtension(FileName));
	aniNames.push_back(tpose);
}

void ModelEditor::ImportModel()
{
	if (getFolderImage.size() == 1) //걍 모델
	{
		if (importInfo == true)
		{
			//편집 함 해주고
			ImGui::Begin("Import Model");
			ImGui::SliderFloat3("Import Translation", (float*)importPos, -255.0f, 255.0f);
			ImGui::SliderFloat3("Import Rotation", (float*)importRotation, 0, Math::PI * 2);
			ImGui::SliderFloat3("Import Scale", (float*)importScale, 0, 10.0f);
		
			//임포트 누르면?
			if (ImGui::Button("Do You Want It?!"))
			{//쭉 읽고
				Loader* loader = new Loader();
				wstring fileName = Path::GetFileName(exportName);
				wstring fileNameWithoutExtension = Path::GetFileNameWithoutExtension(fileName);
				loader->ReadFile(exportName);
				wstring tempExportName = directoryBeforFbx + fileNameWithoutExtension;
				loader->ExportMaterial(tempExportName, false);
				loader->ExportMesh(tempExportName);
				SafeDelete(loader);

				model = new Model();
				model->ReadMaterial(tempExportName);
				model->ReadMesh(tempExportName);

				modelRender = new ModelRender(shader, model);

				UINT index = modelRender->AddTransform();
				Transform* trans = modelRender->GetTransform(index);
				//편집한거 넣어주고
				trans->Position(importPos);
				trans->Scale(importScale);
				trans->Rotation(importRotation);
				modelRender->UpdateTransform();
				//초기화 해주고
				importPos = importRotation = Vector3(0, 0, 0);
				importScale = Vector3(1, 1, 1);
				importInfo = false;
				getFolderImage.clear();
			}
			ImGui::End();
		}
	}
	else //애니메이션 모델
		if (importInfo == true)
		{
			{//쭉 읽고
				ImGui::Begin("Import Model");
				ImGui::SliderFloat3("Import Translation", (float*)importPos, -255.0f, 255.0f);
				ImGui::SliderFloat3("Import Rotation", (float*)importRotation, 0, Math::PI * 2);
				ImGui::SliderFloat3("Import Scale", (float*)importScale, 0, 10.0f);
				editPos = importPos;
				editScale = importScale;
				editRotation = importRotation;
				//임포트 누르면?
				if (ImGui::Button("Do You Want It?!"))
				{
					Loader* loader = new Loader();
					wstring fileName = Path::GetFileName(exportName);  // fileName : 0_메쉬.fbx
					wstring fileNameWithoutExtension = Path::GetFileNameWithoutExtension(fileName);
					loader->ReadFile(exportName); // exportName :L"Kachujin/0_Mesh.fbx"
					wstring tempExportName = directoryBeforFbx + fileNameWithoutExtension;
					loader->ExportMaterial(tempExportName, false);  //tempExportName : L"Kachujin/0_Mesh"
					loader->ExportMesh(tempExportName); //tempExportName : L"Kachujin/0_Mesh"
					SafeDelete(loader);

					loader = new Loader();
					loader->ReadFile(exportName);
					loader->ExportAnimClip(0, tempExportName);  // 메쉬 파일을 애니로 넣어보자
					SafeDelete(loader);

					string modelPose = "T-Pose";
					aniNames.push_back(modelPose);

					////기존에 했던건데..
					//for (UINT i = 1; i < getFolderImage.size(); i++)
					//{
					//	loader = new Loader();
					//	wstring aniName = Path::GetFileName(getFolderImage[i]);  //확장자 포함 애니
					//	fileNameWithoutExtension = Path::GetFileNameWithoutExtension(aniName); //확장자 제거 애니
					//	wstring readFile = directoryBeforFbx + aniName;
					//	wstring exportAnimClip = directoryBeforFbx + fileNameWithoutExtension;c
					//	loader->ReadFile(readFile);
					//	loader->ExportAnimClip(0, exportAnimClip);
					//	clipNames.push_back(exportAnimClip);
					//	modelPose = String::ToString(fileNameWithoutExtension);
					//	aniNames.push_back(modelPose);
					//	SafeDelete(loader);
					//}

					aniModel = new Model();
					aniModel->ReadMaterial(tempExportName);
					aniModel->ReadMesh(tempExportName);
					aniModel->ReadClip(tempExportName);
					//for (UINT i = 0; i <1/* clipNames.size()*/; i++)
					//{
					//	model->ReadClip(clipNames[i]);
					//}

					modelAnimator = new ModelAnimator(shader, aniModel);

					UINT index = modelAnimator->AddTransform();
					Transform* trans = modelAnimator->GetTransform(index);
					trans->Position(importPos);
					trans->Scale(importScale);
					trans->Rotation(importRotation);
					modelAnimator->UpdateTransform();
					selectAni = 0;

					getFolderImage.clear();

					vecBone = aniModel->Bones();  //본 받아와버리기
					boneNum = vecBone.size();
					
					//콜박스 테스트
					for (UINT i = 0; i < boneNum; i++)
					{
						UINT bIndex = boneBox->Push();
						Transform* boxTrans = boneBox->GetTransform(bIndex);
						boxTrans->Position(importPos);
						boxTrans->Scale(importScale);
						boxTrans->Rotation(importRotation);
					}
					//초기화 해주고
					importPos = importRotation = Vector3(0, 0, 0);
					importScale = Vector3(1, 1, 1);
					importInfo = false;
				}
				ImGui::End();
			}
		}
}

void ModelEditor::ChangeAni()
{
	ImGui::Begin("AniClip");
	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("EditModel"))
		{
			if (ImGui::BeginCombo("##combo", current_state.c_str()))
			{
				for (UINT i = 0; i < aniNames.size(); i++)
				{
					bool is_selected = (current_state.c_str() == aniNames[i].c_str());
					if (ImGui::Selectable(aniNames[i].c_str(), is_selected))
					{
						current_state = aniNames[i].c_str();
						selectAni = i;
						modelAnimator->PlayNextClip(0, selectAni, blendTime);
					}
				}
				ImGui::EndCombo();
			}
			ImGui::InputFloat("Tween", &blendTime, 0.01f);
			ImGui::InputFloat("FrameSpeed", (float*)&frameSpeed, 0.1f);
			frameSpeed = frameSpeed <= 0 ? 0 : frameSpeed;
			modelAnimator->FrameSpeed(frameSpeed);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	} ImGui::Separator();
	ImGui::End();
}

void ModelEditor::Hierarchy()
{
	modelAnimator->UpdateTransform();
	ImGuiTreeNodeFlags nodeFlag = NULL;
	nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	ImGui::Begin("Hierarchy");

	UINT count = vecBone.size();

	ImGui::CollapsingHeader(String::ToString(vecBone[0]->Name()).c_str());

	for (UINT i = 0; i < count; i++)
	{
		ModelBone * bone = aniModel->BoneByIndex(i);

		if (bone->ParentIndex() == -1)
		{
			if (String::ToString(bone->Name()) == "RootNode")
				continue;
			if (String::ToString(bone->Name()) == "Kachujin")
				continue;
			UINT i = bone->GetChilds().size();
			if (ImGui::TreeNodeEx(String::ToString(bone->Name()).c_str(), nodeFlag))
			{
				if (ImGui::IsItemClicked())
				{
					selectBone = bone->Index();
					boneBox->WireColors(Color(0, 1, 0, 1));
					boneBox->WireColors(Color(1, 0, 0, 1), selectBone);
				}
					

				for (UINT i = 0; i < bone->GetChilds().size(); i++)
				{
					BoneTree(bone->GetChilds()[i]);
				}

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}

void ModelEditor::BoneTree(ModelBone * bone)
{
	ImGuiTreeNodeFlags nodeFlag = NULL;
	nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (bone->GetChilds().size() == 0)
		nodeFlag |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx(String::ToString(bone->Name()).c_str(), nodeFlag))
	{
		if (ImGui::IsItemClicked())
		{
			selectBone = bone->Index();
			boneBox->WireColors(Color(0, 1, 0, 1));
			boneBox->WireColors(Color(1, 0, 0, 1), selectBone);
		}

		if (bone->GetChilds().size() == 0)
		{
			ImGui::TreePop();
			return;
		}

		for (UINT i = 0; i < bone->GetChilds().size(); i++)
			BoneTree(bone->GetChilds()[i]);

		ImGui::TreePop();
	}

}

void ModelEditor::bonePicking()
{
	UINT count = boneBox->GetDrawCount();
	if (count == 0)
		return;

	D3DXMatrixIdentity(&W);
	V = Context::Get()->ViewMatrix();
	P = Context::Get()->ProjectionMatrix();

	Viewport* vp = Context::Get()->GetViewport();

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	mouse.z = 0.0f;
	Context::Get()->GetViewport()->Unproject(&n, mouse, W, V, P);

	mouse.z = 1.0f;
	Context::Get()->GetViewport()->Unproject(&f, mouse, W, V, P);

	direction = f - n;

	start = n;

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (boneBox->Selected(start, direction, i))
				{
					boneBox->WireColors(Color(0, 1, 0, 1));
					boneBox->WireColors(Color(1, 0, 0, 1), i);
					break;
				}
			}
		}
	}
}

void ModelEditor::TPose()
{
	//if (selectAni < 0)  // 티 포즈일때
	//{
	//	for (UINT i = 0; i < boneNum; i++)
	//	{
	//		Vector3 pos;
	//		Transform* trans = boneBox->GetTransform(i);
	//		trans->Position(&pos);

	//		Matrix boneMatrix = model->BoneByIndex(i)->Transform();
	//		pos.x = boneMatrix._41;
	//		pos.y = boneMatrix._42;
	//		pos.z = boneMatrix._43;
	//		Matrix animatorWolrds = modelAnimator->Worlds(0);
	//		D3DXVec3TransformCoord(&pos, &pos, &animatorWolrds);
	//		trans->Position(pos);
	//	}
	//}
}

void ModelEditor::NoneTPose()
{
	for (UINT i = 0; i < boneNum; i++)
	{
		Vector3 pos;
		Transform* trans = boneBox->GetTransform(i);
		trans->Position(&pos);

		Matrix totalMatrix = modelAnimator->AnimationWolrd(0, selectAni, i);
		pos.x = totalMatrix._41;
		pos.y = totalMatrix._42;
		pos.z = totalMatrix._43;
		Matrix animatorWolrds = modelAnimator->Worlds(0);
		D3DXVec3TransformCoord(&pos, &pos, &animatorWolrds);
		trans->Position(pos);

	}
	if (modelRender != NULL && modelAnimator != NULL)
	{
		Transform* modelTrans = modelRender->GetTransform(0);
		Matrix boneWorld = modelAnimator->AnimationWolrd(0, selectAni, selectBone);
		Vector3 pos;
		Vector3 scale;
		Vector3 rotation;
		scale *= boneWorld._11;
		pos.x = boneWorld._41;
		pos.y = boneWorld._42;
		pos.z = boneWorld._43;
		Matrix animatorWolrds = modelAnimator->Worlds(0);
		D3DXVec3TransformCoord(&pos, &pos, &animatorWolrds);
		modelTrans->Position(pos);
		//Matrix testBone = aniModel->BoneByIndex(selectBone)->Transform();
		//Matrix objectM = modelTrans->World();
		//objectM *= boneWorld;
		//modelTrans->World(objectM);
	}
}

