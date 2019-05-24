#pragma once
#include "Systems/IExecute.h" //�������̽�

class Main : public IExecute
{
public:
	//��Ʈ�� . �ؼ� IExecute �� �ִ� ��� ���� �����Լ� �������
	// IExecute��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Ready() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override;

private:
	void Push(IExecute* exceute);

private:
	vector<IExecute *> executes;


};