#include "stdafx.h"
#include "AnimationDemo.h"

void AnimationDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(12, 0, 0);
	Context::Get()->GetCamera()->Position(2, 4, -12);
	shader = new Shader(L"17_Animation.fxo");

	Kachujin();
}

void AnimationDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(kachujin);
}

void AnimationDemo::Update()
{
	//Tweening Test
	static bool bBlendMode = false;
	static float blendAlpha = 0.f;

	static int clip = 0;
	static float speed = 1.f;
	static float takeTime = 0.25f;

	ImGui::Checkbox("Blend Mode", &bBlendMode);

	if (bBlendMode == false)
	{
		ImGui::InputInt("Clip", &clip);
		clip = Math::Clamp(clip, 0, 4);

		const char* clipsName[] = { "Idle", "Walk", "Run", "Slash", "Dance" };
		ImGui::Text("%s", clipsName[clip]);
		ImGui::SliderFloat("Speed", &speed, 0.1f, 5.f);
		ImGui::SliderFloat("TakeTime", &takeTime, 0.1f, 5.f);

		if (ImGui::Button("Apply"))
			kachujin->PlayTweenMode(clip, speed, takeTime);
	}
	else
	{
		ImGui::SliderFloat("Blend Alpha", &blendAlpha, 0.f, 2.f);
		kachujin->SetBlendAlpha(blendAlpha);

		if (ImGui::Button("Apply"))
			kachujin->PlayBlendMode(0, 1, 2);
	}
	

	//Light Direction Test
	static Vector3 lightDirection = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("Light Direction", lightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(lightDirection);

	//Pass Test
	static int pass = 0;
	ImGui::InputInt("Pass", &pass);
	pass %= 2;

	//Update
	if (kachujin != nullptr)
	{	
		kachujin->Pass(pass);
		kachujin->Update();
	}
}

void AnimationDemo::Render()
{
	if (kachujin != nullptr)
		kachujin->Render();
}

void AnimationDemo::Kachujin()
{
	kachujin = new SkeletalMeshAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walk");
	kachujin->ReadClip(L"Kachujin/Run");
	kachujin->ReadClip(L"Kachujin/Slash");
	kachujin->ReadClip(L"Kachujin/Uprock");

	kachujin->GetTransform()->Position(0, 0, 0);
	kachujin->GetTransform()->Scale(0.01f, 0.01f, 0.01f);
}
