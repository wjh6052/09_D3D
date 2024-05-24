#pragma once
#include "Systems/IExecute.h"


class AnimationDemo : public IExecute
{
public:
	virtual void Initialize() override; // 생성자역활
	virtual void Destroy() override; // 종료할때
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
