#include "Framework.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "ModelMesh.h"
#include "ModelClip.h"
#include "Environment\Terrain.h"

ModelAnimator::ModelAnimator(Shader * shader, Model * model)
	: model(model), drawCount(0), frameSpeed(1.0f), isEdit(false)
{
	frameBuffer = new ConstantBuffer(tweens, sizeof(TweenDesc)* MAX_MODEL_KEYFRAMES);
	instanceBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(D3DXMATRIX), 1, true);

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetShader(shader);
		mesh->FrameBuffer(frameBuffer);
	}

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		transforms[i] = new Transform();

	boneTransforms = new BoneTransform[MODEL_CLIP_NUM]; //4��׸����ϱ�
	//CreateAnimTransform(0);
	for (UINT i = 0; i < model->ClipCount(); i++)
		CreateAnimTransform(i);

	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4; //x
		desc.Height = MAX_MODEL_KEYFRAMES; //y
		desc.MipLevels = 1;
		desc.ArraySize = MODEL_CLIP_NUM;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		
		UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;  //�� ���� ũ��
		void* p = malloc(pageSize * MODEL_CLIP_NUM); //4�� ��ü ũ�� �Ҵ�
		for (UINT c = 0; c < MODEL_CLIP_NUM; c++) // memcpy�� ������ ���� �ѵ�ġ�� �����Ƿ� �ɰ��� ���ش�.
		{
			for (UINT y = 0; y < MAX_MODEL_KEYFRAMES; y++) //4���� �ึ�� �ɰ��� ���ٲ���
			{
				UINT start = c * pageSize;
				void* temp = (BYTE *)p + MAX_MODEL_TRANSFORMS * y * sizeof(Matrix) + start; //�� ���� ����

				memcpy(temp, boneTransforms[c].Transform[y], sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
			}
		}
		int a = MODEL_CLIP_NUM;
		//ArraySize�� ����ϱ� ������ �迭.. ���� �����Ҵ����� ������ �о��ش�!
		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[MODEL_CLIP_NUM];
		for (UINT c = 0; c < MODEL_CLIP_NUM; c++)
		{
			void* temp = (BYTE*)p + c * pageSize; //��� �����ּ�

			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
			subResource[c].SysMemSlicePitch = pageSize; //�� ���� ũ��
		}

		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResource, &texture));
		SafeDeleteArray(subResource);
		free(p);
	}

	//Create SRV
	{
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Format = desc.Format;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.ArraySize = MODEL_CLIP_NUM;

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv));
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->TransformsSRV(srv);
}

ModelAnimator::~ModelAnimator()
{
	//SafeDelete(transform);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		SafeDelete(transforms[i]);

	SafeRelease(texture);
	SafeRelease(srv);
}

void ModelAnimator::Update()
{
	if (model->ClipCount() > 0)
	{
		for (UINT i = 0; i < drawCount; i++)
		{

			TweenDesc& tween = tweens[i];  // ttt = 1, tweent = 0, clip = 0, curr = 1, next = 2, time = 0.9847, running = 0.0327  //next -1, ~0
			ModelClip* currClip = model->ClipByIndex(tween.Curr.Clip);

			tween.Curr.RunningTime += Time::Delta() * frameSpeed;
			if (tween.Next.Clip < 0) //���� ������ ���ٸ�
			{
				float invFrameRate = 1.0f / currClip->FrameRate();  //30
				if (tween.Curr.RunningTime > invFrameRate)
				{
					tween.Curr.RunningTime = 0.0f;

					tween.Curr.CurrFrame = (tween.Curr.CurrFrame + 1) % currClip->FrameCount(); //60  2
					tween.Curr.NextFrame = (tween.Curr.CurrFrame + 1) % currClip->FrameCount(); //3
				}

				tween.Curr.Time = tween.Curr.RunningTime / invFrameRate; //���� ���� �״�� �÷���
			}
			else //���� ������ ������
			{
				ModelClip* nextClip = model->ClipByIndex(tween.Next.Clip);

				tween.Next.RunningTime += Time::Delta();
				tween.TweenTime = tween.Next.RunningTime / tween.TakeTweenTime;
				if (tween.TweenTime > 1.0f)
				{
					tween.Curr = tween.Next;

					tween.Next.Clip = -1;
					tween.Next.CurrFrame = 0;
					tween.Next.NextFrame = 0;
					tween.Next.Time = 0;
					tween.Next.RunningTime = 0;

					tween.TweenTime = 0.0f;
				}
				else
				{
					float invFrameRate = 1.0f / nextClip->FrameRate();
					if (tween.Next.Time > invFrameRate)
					{
						tween.Next.Time = 0.0f;

						tween.Next.CurrFrame = (tween.Next.CurrFrame + 1) % nextClip->FrameCount();
						tween.Next.NextFrame = (tween.Next.CurrFrame + 1) % nextClip->FrameCount();
					}

					tween.Next.Time = tween.Next.Time / invFrameRate;
				}
			}
		}
	}
		
	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::Render()
{
	instanceBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (ModelMesh* mesh : model->Meshes())
	{
		//Depreated
		//mesh->SetTransform(transform);
		mesh->Render(drawCount);
	}
}

void ModelAnimator::Pass(UINT val)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(val);
}

Matrix & ModelAnimator::AnimationWolrd(UINT instance, UINT ClipNum, UINT boneIndex)
{
	if (tweens[instance].Next.Clip < 0)
	{
		UINT currF = tweens[instance].Curr.CurrFrame;
		UINT nextCurrF = tweens[instance].Curr.NextFrame;
		float time = tweens[instance].Curr.Time;
		Matrix boneWorld = model->BoneByIndex(boneIndex)->Transform();
		Matrix boxWorld = boneWorld * boneTransforms[ClipNum].Transform[currF][boneIndex];
		Matrix boxWorld2 = boneWorld * boneTransforms[ClipNum].Transform[nextCurrF][boneIndex];
		Matrix total;

		Math::LerpMatrix(total, boxWorld, boxWorld2, time);

		return total;
	}
	else /*if (tweens[instance].Next.Clip >= 0)*/
	{ //�� �򰥷�
		UINT currF = tweens[instance].Curr.CurrFrame; //�ֱ��� �ֱ� ������
		UINT nextCurrF = tweens[instance].Curr.NextFrame; //�ֱ��� ���� ������
		float time = tweens[instance].Curr.Time; //Ÿ���� �޾ƿ��°� �³�
		UINT clipNum = tweens[instance].Curr.Clip; //���� ������ Ŭ���̴�

		Matrix boneWorld = model->BoneByIndex(boneIndex)->Transform();
		Matrix boxWorld = boneWorld * boneTransforms[clipNum].Transform[currF][boneIndex];
		Matrix boxWorld2 = boneWorld * boneTransforms[clipNum].Transform[nextCurrF][boneIndex];
		
		Matrix total;
		Math::LerpMatrix(total, boxWorld, boxWorld2, time);

		currF = tweens[instance].Next.CurrFrame; //������ �ֱ� ������
		nextCurrF = tweens[instance].Next.NextFrame;  //������ ���� ������
		float nextTime = tweens[instance].Next.Time;  //�ð�,,
		// ���� �� Ŭ���̴�
		boxWorld = boneWorld * boneTransforms[ClipNum].Transform[currF][boneIndex];
		boxWorld2 = boneWorld * boneTransforms[ClipNum].Transform[nextCurrF][boneIndex];

		Matrix nextTotal;
		Math::LerpMatrix(nextTotal, boxWorld, boxWorld2, nextTime);

		Math::LerpMatrix(total, total, nextTotal, tweens[instance].TweenTime);

		return  total;
	}
}


UINT ModelAnimator::AddTransform()
{
	drawCount++;

	return drawCount - 1;
}

Transform* ModelAnimator::GetTransform(UINT index)
{
	return transforms[index];
}

void ModelAnimator::UpdateTransform()
{
	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		worlds[i] = transforms[i]->World();


	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);
}

void ModelAnimator::PlayNextClip(UINT instance, UINT clip, float tweenTime)
{
	tweens[instance].Next.Clip = clip;
	tweens[instance].TakeTweenTime = tweenTime;
}

void ModelAnimator::CreateAnimTransform(UINT index)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(index);
	for (UINT f = 0; f < clip->FrameCount(); f++) //y
	{
		for (UINT b = 0; b < model->BoneCount(); b++) //x  �� �ϳ��� �߰��ؼ� �׽�Ʈ �غ�
		{
			ModelBone* bone = model->BoneByIndex(b); //��Ʈ������ ��

			Matrix parent;
			Matrix invGlobal = bone->Transform();
			D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);
			//����� �ϰ� �ִϸ��̼� ����ϰ� ���ؾ���

			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent); //�θ� �׳�
			else
				parent = bones[parentIndex]; //�ڽĲ��δٰ�


			Matrix animation;
			ModelKeyframe* frame = clip->Keyframe(bone->Name());
			if (frame != NULL) //Ű������ �ִٸ�
			{
				ModelKeyframeData data = frame->Transforms[f];

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T; //����?
			}
			else //Ű�������� ������ �⺻ ����Ҳ���
				D3DXMatrixIdentity(&animation);

			bones[b] = animation * parent; // �ڽ��� ����?? = ���� x �θ� ������� 
			boneTransforms[index].Transform[f][b] = invGlobal * bones[b]; //�����... 3D api���� ã�ƺ���
			//����� x �ڽ��� ����
		}
	}
}
