#pragma once

class StaticMesh_Sphere : public StaticMesh
{
public:
	StaticMesh_Sphere(Shader* sahder, float radius, UINT stackCount = 20, UINT sliceCount = 20);
	~StaticMesh_Sphere();

private:
	void Create() override;

private:
	float radius;

	UINT stackCount;
	UINT sliceCount;
};