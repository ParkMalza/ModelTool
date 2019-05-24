#include "Framework.h"
#include "Model.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "Model/ModelClip.h"
#include "Utilities/Xml.h"  //xml �޾ƿ��� ���� ���
#include "Utilities/BinaryFile.h"


Model::Model()
{
	HighValue = D3DXVECTOR3(0, 0, 0);
	LowValue = D3DXVECTOR3(0, 0, 0);
}

Model::~Model()
{
	for (Material* material : materials)
		SafeDelete(material);

	for (ModelBone* bone : bones)
		SafeDelete(bone);

	for (ModelMesh* mesh : meshes)
		SafeDelete(mesh);

	for (ModelClip* clip : clips)
		SafeDelete(clip);
}

Material * Model::MaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->Name() == name)
			return material;
	}

	return NULL;
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}

	return NULL;
}


ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}

	return NULL;
}

ModelClip * Model::ClipByName(wstring name)
{
	for (ModelClip* clip : clips)
	{
		if (clip->name == name)
			return clip;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Model::ReadMaterial(wstring file)
{
	//�ҷ��鿩�� xml���� �д���...
	file = L"../../_Textures/" + file + L".material";  //�ڿ�  .material �߰�

	Xml::XMLDocument* document = new Xml::XMLDocument();  //xml�� document�κ��� �����Ѵ�.
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());  //���������� �ҷ����� xml_successȣ��
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();  //document�� child . ������ �о���� (�������� ����, �� �Ʒ�����)
	Xml::XMLElement* materialNode = root->FirstChildElement();  //root�� child. root ���� �о����

	// material�� ����� �𸥴�.
	do
	{
		// xml�� <Material> �Ʒ� ������ �޾ƿ´�
		Material* material = new Material();


		Xml::XMLElement* node = NULL;  //���� �޾ƿ��� ���� ���

		node = materialNode->FirstChildElement();  //node���� �������� �����Ҳ��� ��
		material->Name(String::ToWString(node->GetText()));  //node�� �̸� ������


		wstring directory = Path::GetDirectoryName(file);  //��ο��� ���丮 �� �鸸 ������ �´�. ���ϸ� ����!
		String::Replace(&directory, L"../../_Textures", L"");  //�ؽ��� ������, �츮 ��� ���̱� ����


		wstring texture = L"";
		//NextSiblingElement �� ���� �������� �����ٷ� �������� ����Ѵ�.
		node = node->NextSiblingElement();  //�����ٷ� ������
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)  //���ϸ��� �������� ������ش�
			material->DiffuseMap(directory + texture);  //Diffuse �о���

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SpecularMap(directory + texture);  //Specular �о���

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->NormalMap(directory + texture);  //Normal �о���


		D3DXCOLOR color;

		node = node->NextSiblingElement();  //���ó� ��������
		// �츰 ���� �׸��� ������ ConstantBuffer�� ��ġ�� �������� �Ǿ��ִ�.
		
		// Ambient : ������, ������, ������ (���̹�..��)
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Ambient(color); 
		// �� �κп��� 	material->Ambient(color) �� Ÿ�� ������
		//ConstantBuffer�� �־��ֱ� ���� desc�� D3DXCOLOR Ambient
		//�� ���� �־��ְ� �ִٴ°��� �� �� �ִ�. �־��� ������
		//material�� ConstantBuffer�� ���� �о��ְ� �ִ�.

		//diffuse : �θ�����, �л��
		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Diffuse(color);
		//���� Ambient�� ����.
		//Diffuse�� ���� �־��ְ� material�� �̸� ConstantBuffer�� �о��ش�.

		//specular : ���ߴ�, �ݻ��ϴ�
		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Specular(color);
		//���� �����ϴ�. 
		//Specular�� ���� �־��ְ� material�� �̸� ConstantBuffer�� �о��ش�.

		//���ó� material�� ���̴Ͻ��� Shininess�� ���� �־��ְ�
		//material�� ConstantBuffer�� �о��ش�.

		materials.push_back(material);  //���� ������ ��ģ �� �̸� vector�� ���� �ϱ� ����

		materialNode = materialNode->NextSiblingElement();
	} while (materialNode != NULL);
}

void Model::ReadMesh(wstring file)
{
	file = L"../../_Models/" + file + L".mesh";  //�̰� �о�!

	//
	BinaryReader* r = new BinaryReader();

	r->Open(file); // ���

	UINT count = 0;
	count = r->UInt();  //�о ����� ũ��

	for (UINT i = 0; i < count; i++)  //ũ�⸸ŭ �о��
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();

		bones.push_back(bone);  //�޾ƿ� ���� ����
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();

		mesh->name = String::ToWString(r->String());
		mesh->boneIndex = r->Int();

		//VertexData
		{
			UINT count = r->UInt();

			vector<Model::ModelVertex> vertices;
			vertices.assign(count, Model::ModelVertex());
			

			void* ptr = (void *)&(vertices[0]);
			r->Byte(&ptr, sizeof(Model::ModelVertex) * count);
			//�̸� ������ ��ŭ ���� Ȯ���س��� �о���ΰ���

			mesh->vertices = new Model::ModelVertex[count];
			mesh->vertexCount = count;
			copy
			(
				vertices.begin(), vertices.end(),
				stdext::checked_array_iterator<Model::ModelVertex *>(mesh->vertices, count)
				//���� �����ͷ� ����
			);
		}

		//IndexData
		{
			UINT count = r->UInt();

			vector<UINT> indices;
			indices.assign(count, UINT());

			void* ptr = (void *)&(indices[0]);
			r->Byte(&ptr, sizeof(UINT) * count);


			mesh->indices = new UINT[count];
			mesh->indexCount = count;
			copy
			(
				indices.begin(), indices.end(),
				stdext::checked_array_iterator<UINT *>(mesh->indices, count)
			);
		}


		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;

			meshPart->name = String::ToWString(r->String());
			meshPart->materialName = String::ToWString(r->String());

			meshPart->startVertex = r->UInt();
			meshPart->vertexCount = r->UInt();

			meshPart->startIndex = r->UInt();
			meshPart->indexCount = r->UInt();

			mesh->meshParts.push_back(meshPart);
		}//for(k)

		meshes.push_back(mesh);
	}//for(i)

	r->Close();
	SafeDelete(r);

	BindingBone();
	BindingMesh();  //�������
	SetHighLowVal();
}

void Model::ReadClip(wstring file)
{
	file = L"../../_Models/" + file + L".clip";

	BinaryReader* r = new BinaryReader();
	r->Open(file);


	ModelClip* clip = new ModelClip();

	clip->name = String::ToWString(r->String());
	clip->duration = r->Float();
	clip->frameRate = r->Float();
	clip->frameCount = r->UInt();

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->BoneName = String::ToWString(r->String());


		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->Transforms.assign(size, ModelKeyframeData());

			void* ptr = (void *)&keyframe->Transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyframeData) * size);
		}

		clip->keyframeMap[keyframe->BoneName] = keyframe;
	}

	r->Close();
	SafeDelete(r);

	clips.push_back(clip);
}

void Model::BindingBone()
{
	root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)  
		{
			bone->parent = bones[bone->parentIndex];  //�ڱ� �θ� �Ҵ�
			bone->parent->childs.push_back(bone);  //�θ��� �ڽĿ��ٰ� �ڱ� �Ҵ�
		}
		else
			bone->parent = NULL;//�θ� ������
	}
}

void Model::BindingMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelBone* bone : bones)
		{
			if (mesh->boneIndex == bone->index)  //�� ��ȣ������ �޽��� ����
			{
				mesh->bone = bone;

				break;
			}
		}

		mesh->Binding(this);
	}
}

void Model::SetHighLowVal()
{
	UINT tempCount = 0;

	for (ModelMesh* meshData : meshes)
	{
		UINT count = meshData->vertexCount;

		for (UINT i = 0; i < count; i++)
		{
			float tempX = meshData->vertices[i].Position.x;
			float tempY = meshData->vertices[i].Position.y;
			float tempZ = meshData->vertices[i].Position.z;

			if (HighValue.x <= tempX)
				HighValue.x = tempX;
			if (HighValue.y <= tempY)
				HighValue.y = tempY;
			if (HighValue.z <= tempZ)
				HighValue.z = tempZ;

			if (LowValue.x >= tempX)
				LowValue.x = tempX;
			if (LowValue.y >= tempY)
				LowValue.y = tempY;
			if (LowValue.z >= tempZ)
				LowValue.z = tempZ;
		}
	}
}
