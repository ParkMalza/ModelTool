#include "stdafx.h"
#include "RTVMinimap.h"
#include "PostEffects/Default.h"
#include "PostEffects/Wiggle.h"
#include "PostEffects/Distortion.h"
#include "PostEffects/Sepia.h"
#include "PostEffects/Saturation.h"
#include "PostEffects/Sharpening.h"
#include "PostEffects/RadialBlur.h"
#include "PostEffects/Interlace.h"
#include "PostEffects/Vignette.h"
#include "PostEffects/Bloom.h"

RTVMinimap::RTVMinimap()
{
	//ȭ�� ũ��
	width = (float)D3D::GetDesc().Width;
	height = (float)D3D::GetDesc().Height;

	//RenderTarget
	{
		for (UINT i = 0; i < 10; i++)
			targets[i] = new RenderTarget((UINT)width, (UINT)height);
	}


	//������ �׷����� �ֵ�
	{
		//����
		renderTarget = new RenderTarget((UINT)width, (UINT)height);
		depthStencil = new DepthStencil((UINT)width, (UINT)height);
		viewport = new Viewport(width, height);

		mainRender2D = new Render2D();
		mainRender2D->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		mainRender2D->GetTransform()->Scale(width, height, 1);

		//�߰� �����Ǵ� ȭ��'s
		//orthoRenderTarget1 = new RenderTarget((UINT)width, (UINT)height);
		//orthoDepthStencil = new DepthStencil((UINT)width, (UINT)height);
		//orthoViewport1 = new Viewport(width, height);

		//orthoRender1 = new Render2D();
		//orthoRender1->GetTransform()->Position(width * 0.25f, height * 0.75f, 0);
		//orthoRender1->GetTransform()->Scale(width * 0.5f, height * 0.5f, 1);
	}

#ifdef ALLOW_MULTI_VIEWPORT
	minimapTarget = new RenderTarget((UINT)width, (UINT)height);
	minimapDepth = new DepthStencil((UINT)width, (UINT)height);
	minimapViewPort = new Viewport(width, height);
	minimap2D = new Render2D();
	float tempWidht = width / 6;
	float tempHeight = height / 6;
	minimap2D->GetTransform()->Position(width - tempWidht/2, tempHeight, 0);
	minimap2D->GetTransform()->Scale(tempWidht, tempHeight, 1);
#endif //  ALLOW_MULTI_VIEWPORT

	//���� ȿ��
	{
		deFault = new PostEffects::Default();
		deFault->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		deFault->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(deFault);

		wiggle = new PostEffects::Wiggle();
		wiggle->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		wiggle->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(wiggle);

		distortion = new PostEffects::Distortion();
		distortion->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		distortion->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(distortion);

		sepia = new PostEffects::Sepia();
		sepia->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		sepia->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(sepia);

		saturation = new PostEffects::Saturation();
		saturation->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		saturation->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(saturation);

		sharpening = new PostEffects::Sharpening();
		sharpening->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		sharpening->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(sharpening);

		radialBlur = new PostEffects::RadialBlur();
		radialBlur->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		radialBlur->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(radialBlur);

		interLace = new PostEffects::Interlace();
		interLace->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		interLace->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(interLace);

		vignette = new PostEffects::Vignette();
		vignette->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		vignette->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(vignette);

		bloom = new PostEffects::Bloom();
		bloom->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
		bloom->GetTransform()->Scale(width, height, 1);
		postEffects.push_back(bloom);
	}

	isMapOpen = true;
	isDivideScreen = false;
	selectEffects = 0;

	effect = "Default";
	effects.push_back(effect);
	effect = "Wiggle";
	effects.push_back(effect);
	effect = "Distortion";
	effects.push_back(effect);
	effect = "Sepia";
	effects.push_back(effect);
	effect = "Saturation";
	effects.push_back(effect);
	effect = "Sharpening";
	effects.push_back(effect);
	effect = "RadialBlur";
	effects.push_back(effect);
	effect = "interLace";
	effects.push_back(effect);
	effect = "vignette";
	effects.push_back(effect);
	effect = "Bloom";
	effects.push_back(effect);

}

RTVMinimap::~RTVMinimap()
{
	SafeRelease(finalSrv);
	SafeDelete(minimap2D);
	SafeDelete(minimapViewPort);
	SafeDelete(minimapDepth);
	SafeDelete(minimapTarget);
	for (Render2D* pEffect : postEffects)
		SafeDelete(pEffect);
	for (int i = 0; i < 10; i++)
		SafeDelete(targets[i]);
	SafeDelete(mainRender2D);
	SafeDelete(viewport);
	SafeDelete(depthStencil);
	SafeDelete(renderTarget);
}

void RTVMinimap::Update()
{
	ImGui::Checkbox("MiniMap", &isMapOpen);

	//���� ������Ʈ�� Render2D
	mainRender2D->Update();

	//���� ȿ��
	SetImgui();
	postEffects[selectEffects]->Update();
	

#ifdef  ALLOW_MULTI_VIEWPORT
	minimap2D->Update();
#endif //  ALLOW_MULTI_VIEWPORT
}

void RTVMinimap::PreRender()
{
	renderTarget->Set(depthStencil->DSV()); //����Ÿ�ٿ� depthStencil ����
	//viewport->RSSetViewport();
}

void RTVMinimap::MultiViewPreRender()
{
	//���� ȿ�� ��ó��
	if (selectEffects != 9)
	{
		targets[0]->Set(depthStencil->DSV());  //targets[0]�� depthStencil ����

		postEffects[selectEffects]->SRV(renderTarget->SRV());
		postEffects[selectEffects]->Render();

		finalSrv = targets[0]->SRV();  //Ÿ�ٿ��� ���������� �׷��� �� ��Ʈ���ϰڴ�
	}
	else
	{
		//Lumi
		targets[0]->Set(depthStencil->DSV());
		postEffects[selectEffects]->Pass(0);
		postEffects[selectEffects]->SRV(renderTarget->SRV());
		postEffects[selectEffects]->Render();

		//BlurX
		targets[1]->Set(depthStencil->DSV());

		float x = 1.0f / D3D::GetDesc().Width; // 100�� ũ��� �����ϸ�, 1�� �Ѿ�ڳ�.
		((PostEffects::Bloom *)postEffects[selectEffects])->BlurOffset(x, 0);

		postEffects[selectEffects]->Pass(1);
		postEffects[selectEffects]->SRV(renderTarget->SRV());
		postEffects[selectEffects]->Render();

		//BlurY
		targets[2]->Set(depthStencil->DSV());

		float y = 1.0f / D3D::GetDesc().Height;
		((PostEffects::Bloom *)postEffects[selectEffects])->BlurOffset(0, y);

		postEffects[selectEffects]->Pass(1);
		postEffects[selectEffects]->SRV(targets[1]->SRV());  // X �� �־������ν� �����ش�
		postEffects[selectEffects]->Render();

		//composite
		targets[3]->Set(depthStencil->DSV());
		//(PostEffects::Bloom *)
		postEffects[selectEffects]->Pass(2);
		postEffects[selectEffects]->SRV(targets[2]->SRV());  // X �� �־������ν� �����ش�
		(static_cast<PostEffects::Bloom *>(postEffects[selectEffects]))->LuminosityMap(targets[0]->SRV());
		postEffects[selectEffects]->Render();

		finalSrv = targets[3]->SRV();  //Ÿ�ٿ��� ���������� �׷��� �� ��Ʈ���ϰڴ�

	}
}

void RTVMinimap::MinimapRender()
{
#ifdef  ALLOW_MULTI_VIEWPORT 
	minimapTarget->Set(minimapDepth->DSV());
	//minimapViewPort->RSSetViewport();
#endif //  ALLOW_MULTI_VIEWPORT
}

void RTVMinimap::SetViewport()
{
	viewport->RSSetViewport();
}

void RTVMinimap::SetImgui()
{
	static const char* current_effect = NULL;

	if (ImGui::BeginCombo("##combo", current_effect))
	{
		for (UINT n = 0; n < effects.size(); n++)
		{
			bool is_selected = (current_effect == effects[n].c_str());
			if (ImGui::Selectable(effects[n].c_str(), is_selected))
			{
				current_effect = effects[n].c_str();
				selectEffects = n;
			}
		}
		ImGui::EndCombo();
	}
}

void RTVMinimap::PostRender()
{
	mainRender2D->SRV(finalSrv);  //������ �־��ٰ��̳Ŀ� ���� ȭ�鿡 ��µɰ��� ��������
	mainRender2D->Render();

	//����ȭ��
	mainRender2D->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
	mainRender2D->GetTransform()->Scale(width, height, 1);

#ifdef  ALLOW_MULTI_VIEWPORT
	if (isMapOpen == true)
	{
		//�̴ϸ�!
		minimap2D->SRV(minimapTarget->SRV());
		minimap2D->Render();
	}
#endif //  ALLOW_MULTI_VIEWPORT
}