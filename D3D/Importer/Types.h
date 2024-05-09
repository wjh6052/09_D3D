#pragma once

#include "stdafx.h"

struct asBone
{
	int Index;
	string Name;

	int Parent;
	Matrix Transform;
};

struct asMesh
{
	int BoneIndex;

	vector<SkeletalMesh::VertexSkeletalMesh> Vertices;
	vector<UINT> Indices;

};