#include "Framework.h"
#include "SkeletalMeshAnimator.h"


SkeletalMeshAnimator::SkeletalMeshAnimator(Shader* shader)
	: shader(shader)
{
	skeletalMesh = new SkeletalMesh();

	transform = new Transform(shader);
}

SkeletalMeshAnimator::~SkeletalMeshAnimator()
{
	SafeDelete(skeletalMesh);
	SafeDelete(transform);
}

void SkeletalMeshAnimator::Update()
{
	if (bRead == true)
	{
		bRead = false;

		for (SkeletalMesh_Mesh* mesh : skeletalMesh->Meshes())
			mesh->SetShader(shader);

		UpdateTransforms();
	}
	
	for (SkeletalMesh_Mesh* mesh : skeletalMesh->Meshes())
		mesh->Update();
}

void SkeletalMeshAnimator::Render()
{
	for (SkeletalMesh_Mesh* mesh : skeletalMesh->Meshes())
	{
		mesh->SetTransform(transform); //World
		mesh->Render();
	}
}

void SkeletalMeshAnimator::ReadMesh(wstring file)
{
	skeletalMesh->ReadMesh(file);

}

void SkeletalMeshAnimator::ReadMaterial(wstring file)
{
	skeletalMesh->ReadMaterial(file);

	bRead = true;
}

void SkeletalMeshAnimator::Pass(UINT pass)
{
	for (SkeletalMesh_Mesh* mesh : skeletalMesh->Meshes())
		mesh->Pass(pass);
}

void SkeletalMeshAnimator::UpdateTransforms()
{
	for (UINT i = 0; i < skeletalMesh->BoneCount(); i++)
	{
		SkeletalMesh_Bone* bone = skeletalMesh->BoneByIndex(i);
		transforms[i] = bone->Transform();
	}

	for (SkeletalMesh_Mesh* mesh : skeletalMesh->Meshes())
		mesh->Transforms(transforms);
}
