#pragma once

#include "stdafx.h"


struct asBone
{
	int Index;
	string Name;

	int Parent;
	Matrix Transform;
};


struct asMeshPart
{
	string MaterialName;

	UINT StartVertex;
	UINT VertexCount;

	UINT StartIndex;
	UINT IndexCount;
};


struct asMesh
{
	int BoneIndex;

	vector<SkeletalMesh::VertexSkeletalMesh> Vertices;
	vector<UINT> Indices;

	vector<asMeshPart*> MeshParts;
};


struct asMaterial
{
	string Name;

	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;
};

