#include "stdafx.h"
#include "Loader.h"
#include "Type.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

Loader::Loader()
{
	importer = new Assimp::Importer();
}

Loader::~Loader()
{
	SafeDelete(importer);
}

void Loader::ReadFile(wstring file)
{
	this->file = L"../../_Assets/" + file; // ���� ���� _Assets ���Ϸ�..
										   //�츮�� wstring, Assimp�� string ������ �Ѱ��ֱ� ������ �������
										   //�����Ͱ� ������ �о ���� ��������
	scene = importer->ReadFile(String::ToString(this->file),  //(��θ�, ������ �츮�� ���ϴ����·� �о���̱� ���� 
		aiProcess_ConvertToLeftHanded						  // = �츰 �޼���ǥ�踦 ����
		| aiProcess_Triangulate   // | �츰 �ﰢ������ �׷��Դ�. ������ �׷��� ���� ���µ鵵 �����Ѵ�. ���� �ﰢ������ �ٲ���!
		| aiProcess_GenUVCoords   // | UV �� ���� �� �鵵 uv����ؼ� �Ѱ���!
		| aiProcess_GenNormals   	// | �븻 ���Ͱ� ���� �� ����ؼ� �Ѱ���!
		| aiProcess_CalcTangentSpace);   	// | ���߿��ٷ�)

	assert(scene != NULL);
}

void Loader::ExportMaterial(wstring savePath, bool bOverwrite)
{
	//�Ѿ�� savePath�� ������/���ϸ�
	//���� �Ǵ� ��θ�
	savePath = L"../../_Textures/" + savePath + L".material";  //�ؽ��� ��������~ savepath + .material Ȯ���� �߰�

	ReadMaterial();
	WriteMaterial(savePath, bOverwrite);
}

void Loader::ReadMaterial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		//assimp�� ��� �ڷ������� �տ� ai�� �ٴ´�.
		aiMaterial* readMaterial = scene->mMaterials[i];  //����������, ��Ȯ���� �������� �迭

														  //�츮�� ������ �ڷ���
		AsMaterial* material = new AsMaterial();  //Type���� �츮�� ������ AsMaterial �ڷ���

		material->Name = readMaterial->GetName().C_Str();

		//����
		aiColor3D color; //���� ��� 3D
		float val; // float �����ε� ���ϵ�

		readMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);  //��ũ������ 3���� �̹� ���ǵǾ�����
		material->Ambient = D3DXCOLOR(color.r, color.g, color.b, 1.0f);  //ai_real = float3

		readMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);  //��ũ������ 3���� �̹� ���ǵǾ�����
		material->Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);  //ai_real = float3

		readMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);  //��ũ������ 3���� �̹� ���ǵǾ�����
		readMaterial->Get(AI_MATKEY_SHININESS, val);  //��ũ������ 3���� �̹� ���ǵǾ�����
		material->Specular = D3DXCOLOR(color.r, color.g, color.b, 1.0f);  //ai_real = float3

		aiString file;  //������ ��θ��� ����

		readMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();  // Diffuse �츮�� ���� �� �ؽ���

		readMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		readMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();
		//scene�� �ִ� material�� ������ �츮������ ��� �����س���

		materials.push_back(material);
		//material
		//Name, Ambient, Diffuse, Specular, Shiness, DiffuseFile, SpecularFile, NormalFile

	}
}

void Loader::WriteMaterial(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false)  //����� �ʰڴ�(false) �ϸ� �⺻���� true.
	{
		if (Path::ExistFile(savePath) == true) //���� �Ѿ�� ����� ������ �����ϸ� �״�� ��������.
			return;
	}

	string folder = String::ToString(Path::GetDirectoryName(savePath)); //��ο��� ���丮 �� �鸸 ������ �´�. ���ϸ� ����!
	string file = String::ToString(Path::GetFileName(savePath));  //���ϸ�(Ȯ���� ����)�� ������

																  //�����Ϸ��� ������ ���� ��� �������
	Path::CreateFolders(folder); //���鼭 �ش� ��ΰ� ������ �� �������.

								 // Xml �� document�κ��� �����Ѵ�. �׻�!! ����!! �κ��� �����Ѵ�.
	Xml::XMLDocument* document = new Xml::XMLDocument();

	// Xml ������ ����ϰڴ�.
	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);  //document�� �ֻ���̰� �� �Ʒ� �ٿ���.

	Xml::XMLElement* root = document->NewElement("Materials"); //��� �����ʹ� ��� ���� �����Ѵ�. ("�̸�")
															   // Element�� ������ ����°Ű�, attribute�� ���������� ����°�
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");  //��� �־�� xml���Ϸ� �д´�.
																		  //<Materials xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
	document->LinkEndChild(root);

	for (AsMaterial* material : materials)
	{
		//���⼭���� �ڽĵ�? ����
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);  //�ٷ� �� root�� �ٿ��ش�.
								   //    <Material> �Ʒ��� ���� �غ�

		Xml::XMLElement* element = NULL;  //node �Ʒ� ���� ��

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str()); //������ �̸�
		node->LinkEndChild(element);  //���Ʒ� �ٿ��ش�.

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(material->DiffuseFile, folder).c_str());
		node->LinkEndChild(element); //���Ʒ� �ٿ��ش�.

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(material->SpecularFile, folder).c_str());
		node->LinkEndChild(element); //���Ʒ� �ٿ��ش�.

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(material->NormalFile, folder).c_str());
		node->LinkEndChild(element); //���Ʒ� �ٿ��ش�.


		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r); //SetAttribute : ������ �ٴ°�~
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);  //����� �Ʒ��� �ٿ��ش�.

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);  //����� �Ʒ��� �ٿ��ش�.

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);  //����� �Ʒ��� �ٿ��ش�.

  //����� �Ʒ��� �ٿ��ش�.

  //���� ����� �Ʒ��� ���� �ֵ��� ������ �ȴ٤�������������������.!!!

  //��������� ���൵ --->  document(�ֻ��) - decl(Ư���� ��.. xml���� ����ϰڴ�)
  //                           |
  //                          root  (Materials)
  //                           |
  //                          node  (Material)  ... root�Ʒ� �پ��ִ�. ���� document�� �����̴�.
  //                           |
  //  element (Name) / element (DiffuseFile) / element (SpecularFile) / element (NormalFile) 
  //  element (Ambient)  /  element (Diffuse)  /  element (Specular)  /  element (Shininess)   ..node�Ʒ� �پ��ִ�. ���� document�� �����̴�.

		SafeDelete(material);  //�� ���Ͽ� �� �Ű�����~ ���� �������ش�.
	}

	document->SaveFile((folder + file).c_str());
}

string Loader::WriteTexture(string file, string saveFolder)
{
	if (file.length() < 1) return "";  //������ ����

	string fileName = Path::GetFileName(file);  //���ϸ� ������(Ȯ���� ����)
												// aiTexture -> assimp�� �ؽ��� ������ ������ ����
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());  //����� �ؽ���(����Ǿ��ִ� �ؽ���)

	string savePath = "";
	if (texture != NULL) //Null�� �ƴϸ� ����� �ؽ��Ĵ�. (����)
	{
		savePath = saveFolder + Path::GetFileNameWithoutExtension(file) + ".png"; //png ���Ϸθ� �޾ƿò���!

		if (texture->mHeight < 1)  // �ȼ������� �� �ֱ� ������ �״�� ���常 ���ָ� ������ �ȴ�.
		{
			FILE* fp;
			fopen_s(&fp, savePath.c_str(), "wb"); // ( �ּ�, ���ϸ�, ���̳ʸ��� ���ڴ�)
			fwrite(texture->pcData, texture->mWidth, 1, fp); // (������ ����, ������ ũ��, ������ 1���� ���´�, ���� ����Ʈ)
			fclose(fp);  //���� �������� �ݾ�.
		}
		else
		{   // ������������ ����Ǿ� �ִ� ����. ���� ������...
			ID3D11Texture2D* dest;
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = texture->pcData;


			HRESULT hr;
			hr = D3D::GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);
			Check(hr);

			D3DX11SaveTextureToFileA(D3D::GetDC(), dest, D3DX11_IFF_PNG, savePath.c_str());
		}
	}
	else  //���̸� �����̴�
	{
		string directory = Path::GetDirectoryName(String::ToString(this->file));
		string origin = directory + file;
		String::Replace(&origin, "\\", "/");  // �������ø� / �� �ٲ�

		if (Path::ExistFile(origin) == false) //���� �� ���� ������ ���ٸ� ������ ���°ɷ� �����ϰ� ���Ͻ�Ŵ
			return "";

		savePath = saveFolder + fileName;  //�Ѱܹ޾ƿ� saveFolder + ���ϸ����� ���� fileName ������
		CopyFileA(origin.c_str(), savePath.c_str(), FALSE);

		String::Replace(&savePath, "../../_Textures/", "");  //�ڿ� �ؽ��� ����
	}

	return Path::GetFileName(savePath);
}

void Loader::ExportMesh(wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Models/" + savePath + L".mesh";

	ReadBoneData(scene->mRootNode, -1, -1);  //��� �����̹Ƿ� root���� ź��. �����ε��� -1, parent -1
	ReadSkinData();
	WriteMehData(savePath, bOverwrite);
}

void Loader::ReadBoneData(aiNode * node, int index, int parent)
{
	AsBone* bone = new AsBone();
	bone->Index = index;  //���� -1
	bone->Parent = parent;  //���� -1
	bone->Name = node->mName.C_Str();  //bone�� �̸�

									   // SIMD : �ϳ��� ��ɾ�� �������� DATA�� ó�� (�� �켱)
									   // MIMD : �������� ��ɾ�� �������� DATA�� ó��
									   // MISD : �������� ��ɾ�� �Ѱ����� DATA�� ó�� (ȿ���� ����.. �̷����θ� ����)

									   //SIMD�� �����ʺ��� �����Ͱ� ä������. �� �켱.
									   //DX10 ���� SIMD(�ϳ��� ��ɾ�� �������� ���� ó��)

	D3DXMATRIX transform(node->mTransformation[0]); //FLOAT* ������ ���� �� �ִ�. ���� �����ּҸ� �־��ָ� �˾Ƽ� �������ش�.
	D3DXMatrixTranspose(&bone->Transform, &transform);  //�츮�� ���°� ��켱 �̹Ƿ� ��ġ�ؼ� �־������

	D3DXMATRIX temp;
	if (parent == -1) //�ֻ�� ��Ʈ , �θ����
		D3DXMatrixIdentity(&temp);  //�׳� 
	else
		temp = bones[parent]->Transform;  //�θ��� Ʈ������ ������

	bone->Transform = bone->Transform * temp;  //���� Ʈ������ �ٽ� ���
	//�� �߿� �ϳ��� ������ ���� �����Ƿ� �ٽ� ������ذž�

	bones.push_back(bone);
	ReadMeshData(node, index); //�ڽ��� bone�� �ε����� mesh�� �Ѱ���
	for (UINT i = 0; i < node->mNumChildren; i++)  //���ź��. �ڽĲ� ��� ������
		ReadBoneData(node->mChildren[i], bones.size(), index);  // index -> �����ڽ��� �θ��ȣ�� ������ ��ȣ��.
}

void Loader::ReadMeshData(aiNode * node, int bone)
{
	if (node->mNumMeshes < 1) return;  //�޽��� �������� �־�. ������ �ȱ׸�����

	AsMesh* asMesh = new AsMesh();
	asMesh->Name = node->mName.C_Str(); //�޽��� �̸��� ���� �̸��� ����
	asMesh->BoneIndex = bone;  //���ϰ� ������ ��ȣ

	for (UINT i = 0; i < node->mNumMeshes; i++)  // ���� ������ �޽��� ������ �ϼ��� �ִ� ���̴�.
	{
		UINT index = node->mMeshes[i]; //���� �پ��ִ� �Ž��� ��ȣ
		aiMesh* mesh = scene->mMeshes[index];  //�޽� ������ �޾ƿ�

		UINT startVertex = asMesh->Vertices.size();
		UINT startIndex = asMesh->Indices.size();

		for (UINT m = 0; m < mesh->mNumVertices; m++) //�޽��ȿ� ������ ��������~
		{
			Model::ModelVertex vertex;
			memcpy(&vertex.Position, &mesh->mVertices[m], sizeof(D3DXVECTOR3));
			memcpy(&vertex.Uv, &mesh->mTextureCoords[0][m], sizeof(D3DXVECTOR2));  //2���� �迭�� ���� -->  �ؽ��� �ϳ� ���� �� �ؽ��ĸ� �ø��� ��쵵 �ֱ⶧�� (���̾� ��� �Ѵ�.)
			memcpy(&vertex.Normal, &mesh->mNormals[m], sizeof(D3DXVECTOR3));
			memcpy(&vertex.Tangent, &mesh->mTangents[m], sizeof(D3DXVECTOR3));  //ź��Ʈ ���� �ڵ����� ���´�.

			asMesh->Vertices.push_back(vertex);
		}//for(m)

		for (UINT f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				asMesh->Indices.push_back(face.mIndices[k]);
				asMesh->Indices.back() += startVertex;
			}
		}

		AsMeshPart* meshPart = new AsMeshPart();

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		meshPart->Name = mesh->mName.C_Str();
		meshPart->MaterialName = material->GetName().C_Str();
		meshPart->StartVertex = startVertex;
		meshPart->StartIndex = startIndex;
		meshPart->VertexCount = mesh->mNumVertices;
		meshPart->IndexCount = mesh->mNumFaces * mesh->mFaces->mNumIndices;
		//�츮�� ����ϴ� �ε��� ��ȣ ���ϴ� ����.. 3 x mNumIndices(�ﰢ���� ����)
		//�簢�� �ϳ����� �ε����� 6��.. 
		//face�� �ﰢ���� ������ ���� = 3��
		//mNumIndices�� �ﰢ���� ����

		asMesh->MeshParts.push_back(meshPart);
	}

	meshes.push_back(asMesh);
}

void Loader::ReadSkinData()
{
	for (UINT i = 0; i < scene->mNumMeshes; i++) //�޽� ���� 1��
	{
		aiMesh* aiMesh = scene->mMeshes[i]; //ai~ : assimp ��
		if (aiMesh->HasBones() == false) continue; //�޽��� ���� ���� ��찡 �̵���~?!
		//���� ������ �޽��� �ƴ� ��� ex) ���̶�簡 ���̶�簡 ���̶�簡
		//���߿� �߰��ɰ���
		AsMesh* mesh = meshes[i];
		//Name : Kachujin
		//BoneIndex : 66��
		//Mesh : NULL
		//Vertices : 25850��
		//Indices : 37830��
		//MeshParts : 1��

		vector<AsBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), AsBoneWeights());
		UINT num = boneWeights.size();
		//������ ����ġ vector. �̸� ������ ������ŭ �Ҵ��Ѵ�. �������� ����ġ�� 
		//��� �� ���̱� ������ �̸� Ȯ���� �д�?
		for (UINT b = 0; b < aiMesh->mNumBones; b++) //���� ������ŭ �������鼭 ����ġ ��� .. ��? -> ����ġ�� �� Matrix�� ���ؼ� ���Ǳ⶧��
		{	//ī������ Bone ������ 53����
			aiBone* aibone = aiMesh->mBones[b]; //ai ~ : Assimp �� Bone��.. mBones[0]�� "Hip", mNumWeights = 4020, mVertexId = 2635, mWeight = 0.6�� �ٻ�ġ����
			UINT boneIndex = FindBoneIndex(aibone->mName.C_Str()); //"Hip"�� �ε��� ��ȣ�� 1?(�θ��� �����̾ 1�ε�) readBone���� �� �޾ƿ�, �츮�� ������ ���� ��ȣ�� �޾ƿ��µ�

			for (UINT w = 0; w < aibone->mNumWeights; w++) //mNumWeights == 4020 -> ���� ����ġ�� � �ִ���?
			{
				//�ִ� 4����
				UINT index = aibone->mWeights[w].mVertexId; // w == 0 �� ��, index == 2653 == ����ġ�� �� ������ ��ȣ!
				float weight = aibone->mWeights[w].mWeight;  // �Ʊ� �� 0.6�� �ٻ�ġ �� �޾ƿԳ�
				//��� : 2653��(mVertexId)�� ������ȣ���ٰ� 0.6����(mWeight)�� �ٻ�ġ ���� �־��ְڴ�!
				boneWeights[index].AddWeights(boneIndex, weight); //���� �ε����� ����ġ �־���
			}
		}//for(b)  //������ ����ġ ��� �־�����

		//�츮���� �������� ����ġ�� �ִ´�??
		for (UINT w = 0; w < boneWeights.size(); w++)  //boneWeights.size == 25850
		{
			boneWeights[w].Normalize(); //��ü ����ġ�� ���� 1�̵ȴ�.
			AsBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight); //blendWeight���ٰ� ����ġ �������ش�

			mesh->Vertices[w].BlendIndices = blendWeight.Indices; //�̰� asMesh�� �츮����. �츮�����ٰ� �� �޾ƿ�
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

UINT Loader::FindBoneIndex(string name)  //"Hip"�� �Ѿ�Գ�
{
	for (AsBone* bone : bones) //as~ : �츮�� �̹� �� �޾Ƴ��� Bone��. bones.size() == 67��. �Ƹ��� �� ���� �θ���� ���� ������ BoneIndex 66�� + �θ��� 1�� �ȵ�
	{
		if (bone->Name == name)
			return bone->Index; //"Hip" �϶� �θ��� �����̴ϱ� 1 �Ѱ��ֳ׿�
	}

	return 0;
}

void Loader::WriteMehData(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false)  //����� �ʰڴ� �ϸ� �⺻���� true.
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter * w = new BinaryWriter();  //������ writer, �������� reader

	w->Open(savePath);  //������ ���� ����

	w->UInt(bones.size());

	//AsBone�� �Ȱ���.. �������Ѽ�
	for (AsBone* bone : bones)  //���� ������ŭ ���鼭 ��������
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		SafeDelete(bone);
	}

	w->UInt(meshes.size());
	for (AsMesh* meshData : meshes)
	{
		//�Ž�
		w->String(meshData->Name);
		w->Int(meshData->BoneIndex);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::ModelVertex) * meshData->Vertices.size());
		//����Ʈ ������ �����ϰڴ�

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());

		//�Ž� ��Ʈ
		w->UInt(meshData->MeshParts.size());
		for (AsMeshPart* part : meshData->MeshParts)
		{
			w->String(part->Name);
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCount);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);

			SafeDelete(part);
		}

		SafeDelete(meshData);
	}

	w->Close();
	SafeDelete(w);
}

//�� Ŭ���� ���� �ϳ�
void Loader::GetClipList(vector<wstring>* list)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		list->push_back(String::ToWString(anim->mName.C_Str()));

	}
}

void Loader::ExportAnimClip(UINT index, wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Models/" + savePath + L".clip"; // Kachujin/Idle
	//scene->mAnimations 
	//mName = mixamo.com
	//mDuration = 59
	//mTicksperSecond = 30
	//mNumChannels = 42
	AsClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath, bOverwrite);
}

AsClip * Loader::ReadClipData(aiAnimation * animation)
{
	AsClip* clip = new AsClip();
	clip->Name = animation->mName.C_Str(); // mixamo.com
	clip->FrameRate = (float)animation->mTicksPerSecond; //30
	clip->FrameCount = (UINT)animation->mDuration + 1; //������ �ø� �ع���, 60

	vector<AsClipNode> aniNodeInfos;//�ֻ� ��Ʈ���� �Ʒ��� �������鼭 ����ϴ� ����� ��� �� ���̴�. �����(Forward Kinematics)���
									//�����(Inverse Kinematics)�� ���� : ���� ��κ� ������ IK�� ����Ѵ�.
									//ĳ���Ͱ� �갰������ �ö󰥶� ������� ���� ��Ȯ�ϰ� ���� ���� �� �ִ�. ���������� ��Ʈ�� ����ؼ� �ö󰣴�.

	// tweening : �ѵ��ۿ��� ������������ ���� ������ �ϰ� �Ѿ�� �ϱ� ����
	// blending : �ִϸ��̼ǳ��� ��� ����Ѵ�. �ȴ°Ͱ� ���� ���� ������ ���ÿ� �� �� �ִ�. 2���� ä���� ���ÿ� ����
	//ä���� Blending�� ���� ���ȴ�.
	for (UINT i = 0; i < animation->mNumChannels/*42��*/; i++) 
	{													
		aiNodeAnim* aniNode = animation->mChannels[i]; //0�� "Head" , ��3�� Ű ��� 60��

		AsClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName; //�� �̸� �����صα� , 0�� "Head" �̸� ������ Ű������ �����Ұ��Ӥ�

		//Ű ���� �ƽ��� ������ ����, �� Ű���� �������� ���ڸ��� ä���������̴�. �׻� Ű���� �����ϴ°ɷ�!
		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumRotationKeys); //60
		keyCount = max(keyCount, aniNode->mNumScalingKeys);

		AsKeyframeData frameData;
		for (UINT k = 0; k < keyCount; k++)
		{
			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size(); //0
			//fabsf : ���밪
			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON) // �������� �� Epsilon���� �� ���� �۴ٸ� Ű ���� �����Ѵٰ� ����
			{
				aiVectorKey posKey = aniNode->mPositionKeys[k]; //Ű �޾ƿ�
				memcpy_s(&frameData.Translation, sizeof(Vector3), &posKey.mValue, sizeof(aiVector3D));
				frameData.Time = (float)aniNode->mPositionKeys[k].mTime;

				bFound = true;
			}

			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON) //
			{
				aiQuatKey Key = aniNode->mRotationKeys[k];
				//���ʹϿ��� �����ϸ� �ȵȴ�.. ������ ��.. ã�ƺ���
				frameData.Rotation.x = Key.mValue.x;  //Ű�� �޾ƿ������Դϴ�...
				frameData.Rotation.y = Key.mValue.y;
				frameData.Rotation.z = Key.mValue.z;
				frameData.Rotation.w = Key.mValue.w;

				frameData.Time = (float)aniNode->mRotationKeys[k].mTime;  //0

				bFound = true;
			}

			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON) //
			{
				aiVectorKey posKey = aniNode->mScalingKeys[k];
				memcpy_s(&frameData.Scale, sizeof(Vector3), &posKey.mValue, sizeof(aiVector3D));
				frameData.Time = (float)aniNode->mScalingKeys[k].mTime;

				bFound = true;
			}
			if (bFound == true)
				aniNodeInfo.Keyframe.push_back(frameData);
		}
		if (aniNodeInfo.Keyframe.size() < clip->FrameCount)  //��ü������ ������ ������ �� ��ä�����ž�.. �׷��� ä�ﲨ��
		{
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();
			AsKeyframeData keyframe = aniNodeInfo.Keyframe.back();
			
			for (UINT i = 0; i < count; i++)
				aniNodeInfo.Keyframe.push_back(keyframe);  //������ keyframe���� ���� count��ŭ ä���ִ´�
			
		}
		clip->Duration = max(clip->Duration, aniNodeInfo.Keyframe.back().Time);

		aniNodeInfos.push_back(aniNodeInfo);  //��庰�� Ű������ ���� ������ �ִ°��̴�
	}
	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos); //���! �츮���� �������

	return clip;
}

void Loader::ReadKeyframeData(AsClip * clip, aiNode * node, vector<struct AsClipNode>& aiNodeInfos)
{
	AsKeyframe* keyframe = new AsKeyframe();
	keyframe->BoneName = node->mName.C_Str();  // "RootNode" ���� �����ϳ�, ���� "Hips"

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		AsClipNode* aniNode = FindClipNode(aiNodeInfos, node->mName);

		AsKeyframeData frameData;
		if (aniNode != NULL)
		{
			frameData = aniNode->Keyframe[i];
		}
		else
		{
			Matrix transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform); //�츮�� �� �켱�̶�

			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform); //����
			frameData.Time = (float)i;
		}
		keyframe->Transforms.push_back(frameData);; //���� ����� ����Ÿ?
	}

	clip->Keyframes.push_back(keyframe);

	for (UINT i = 0; i < node->mNumChildren; i++) //mNumChildren : �� �о��Ŀ� ���� ��� �ٸ�
		ReadKeyframeData(clip, node->mChildren[i], aiNodeInfos);
}

AsClipNode * Loader::FindClipNode(vector<AsClipNode>& aniNodeInfos, aiString name)
{
	for (UINT i = 0; i < aniNodeInfos.size(); i++)
	{
		if (aniNodeInfos[i].Name == name)
			return &aniNodeInfos[i];
	}
	return NULL;
}

void Loader::WriteClipData(AsClip * clip, wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}


	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (AsKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(AsKeyframeData) * keyframe->Transforms.size());

		SafeDelete(keyframe);
	}

	w->Close();
	SafeDelete(w);
}
