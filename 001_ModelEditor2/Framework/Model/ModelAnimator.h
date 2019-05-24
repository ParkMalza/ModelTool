#pragma once
#define MAX_MODEL_KEYFRAMES 250
#define MODEL_CLIP_NUM 20

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader, Model* model);
	~ModelAnimator();

	void Update();
	void Render();

	void Pass(UINT val);
	int Clip(UINT instance) { return tweens[instance].Curr.Clip; }
	void RunningTime(UINT& currFrame) { tweens[0].Curr.CurrFrame = currFrame; }
	void RunningTime(OUT UINT* currFrame) { *currFrame = tweens[0].Curr.CurrFrame; }
	void IsEdit(bool& isEdit) { this->isEdit = isEdit; }
	Matrix& AnimationWolrd(UINT instance, UINT ClipNum, UINT boneIndex);

	
	void FrameSpeed(float& val) { frameSpeed = val; }

	UINT GetDrawCount() { return drawCount; }

	UINT AddTransform();
	Transform* GetTransform(UINT index);
	D3DXMATRIX& Worlds(UINT i) { return worlds[i]; }

	void UpdateTransform();

	void PlayNextClip(UINT instance, UINT clip, float tweenTime = 0.18f);


//private:
	void CreateAnimTransform(UINT index);

private:
	Model* model; //�ܺο��� �� �޾ƿͼ� ����
	UINT drawCount;
	Transform* transforms[MAX_MODEL_INSTANCE];
	D3DXMATRIX worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer;

private:
		struct BoneTransform
		{
			Matrix** Transform; //�� �뷮�� �Ѿ�� �����÷ο찡 �Ͼ�Ƿ� ���� �Ҵ��� �Ұ���

			BoneTransform()
			{
				Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

				for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
					Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS]; //������� �ؾ�.. 2���� �迭 �����Ҵ�..
			}
			

			~BoneTransform()
			{
				for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
					SafeDeleteArray(Transform[i]);

				SafeDeleteArray(Transform);
			}
		};

	BoneTransform* boneTransforms;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;

	vector<int> saveDeleteIndexes;
	
private:
	struct KeyframeDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Padding0;
		float Padding1;
		float Padding2;
	};

	struct TweenDesc
	{
		float TakeTweenTime = 1.0f; //�󸶸�ŭ�� �ð����� ��ȭ�ɰ��̳�
		float TweenTime = 0.0f;
		float Padding[2];

		KeyframeDesc Curr;
		KeyframeDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}
	};

	TweenDesc tweens[MAX_MODEL_KEYFRAMES]; // ������ �ٸ� ���������� �÷��� �Ǿ�� �ϹǷ�
	ConstantBuffer* frameBuffer;

	float frameSpeed;
	bool isEdit;
//private:
//	struct InstanceDesc
//	{
//		UINT index;
//		Transform* Transform;
//	};
//	vector<InstanceDesc> updateTransforms;
};