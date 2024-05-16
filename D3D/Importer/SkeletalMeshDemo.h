#pragma once
#include "Systems/IExecute.h"


class SkeletalMeshDemo : public IExecute
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
	void Tank();
	void Kachujin();
	void Tower();

private:
	Shader* shader;
	SkeletalMeshRenderer* tank = nullptr;
	SkeletalMeshRenderer* kachujin = nullptr;
	SkeletalMeshRenderer* tower = nullptr;
};
