#include "Framework.h"
#include "Projector.h"
#include "Viewer/Fixity.h"

Projector::Projector(Shader * shader, vector<wstring>& textureNames, UINT width, UINT height)
	:shader(shader), width(width), height(height)
{
	camera = new Fixity();
	camera->RotationDegree(90, 0, 0);
	camera->Position(0, 10, 0);
	//������ �������� ���� �ʴ´� --> ���̸� �ø��� �����Ǵ� �̹����� Ŀ����. �翬�Ѱ�. ����ü�� ���簢����� �ƴϴ�.
	//projection = new Perspective(static_cast<float>(width), static_cast<float>(height), 0.1f, 100.0f, Math::PI * 0.5f);
	//�ü� �׷����� ������ �������̴�.
	projection = new Orthographic(static_cast<float>(width), static_cast<float>(height));
	//texture = new Texture(textureFile);
	textures = new TextureArray(textureNames);
	buffer = new ConstantBuffer(&desc, sizeof(Desc));

	shader->AsSRV("ProjectorMaps")->SetResource(textures->SRV());
	//sMap = shader->AsSRV("ProjectorMap");
	//sMap->SetResource(texture->SRV());

	sBuffer = shader->AsConstantBuffer("CB_Projector");

}

Projector::~Projector()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
	SafeDelete(textures);
	SafeDelete(projection);
	SafeDelete(camera);
}

void Projector::Update(Vector3& position, int& type, UINT& range, Color& color)
{
	camera->Position(position.x, 10.0f, position.z);
	if (type == 0)
		width = height = 0;
	else
	{
		width = range * 2;
		height = range * 2;
	}
	//width = width < 1 ? 1 : width;

	//ImGui::InputInt("Height", (int *)&height);
	//height = height < 1 ? 1 : height;

	desc.Color = color;

	//�ٿ�ĳ����
	(dynamic_cast<Orthographic*>(projection))->Set((float)width, (float)height);

	camera->GetMatrix(&desc.View);
	projection->GetMatrix(&desc.Projection);

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}
