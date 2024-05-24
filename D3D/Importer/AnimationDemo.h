#pragma once
#include "Systems/IExecute.h"


class AnimationDemo : public IExecute
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
	void Kachujin();

private:
	Shader* shader;
	SkeletalMeshAnimator* kachujin = nullptr;
};
