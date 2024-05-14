#pragma once
#include "Systems/IExecute.h"


class SkeletalMeshDemo : public IExecute
{
public:
	virtual void Initialize() override; // �����ڿ�Ȱ
	virtual void Destroy() override; // �����Ҷ�
	virtual void Update() override;
	virtual void Render() override;
	virtual void PreRender() override {};
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Tank();
	void Kachujin();

private:
	Shader* shader;
	SkeletalMeshRenderer* tank = nullptr;
	SkeletalMeshRenderer* kachujin = nullptr;
};
