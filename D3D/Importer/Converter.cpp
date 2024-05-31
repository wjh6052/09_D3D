#include "stdafx.h"
#include "Converter.h"
#include "Types.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"

Converter::Converter()
{
	importer = new Assimp::Importer();
}

Converter::~Converter()
{
	SafeDelete(importer);

	for (asBone* bone : bones)
		SafeDelete(bone);

	for (asMesh* mesh : meshes)
	{
		for (asMeshPart* part : mesh->MeshParts)
			SafeDelete(part);

		SafeDelete(mesh);
	}

	 for (asMaterial* materia : materials)
		 SafeDelete(materia);
}

void Converter::ReadFile(wstring file)
{
	this->file = L"../../_Assets/" +  file;

	scene = importer->ReadFile
	(
		String::ToString(this->file),
		aiProcess_ConvertToLeftHanded
		| aiProcess_Triangulate
		| aiProcess_GenUVCoords
		| aiProcess_GenNormals
		| aiProcess_CalcTangentSpace
	);

	if (scene == nullptr)
	{
		string str = importer->GetErrorString();
		
		MessageBoxA(D3D::GetDesc().Handle, str.c_str(), "Import Error", MB_OK);
		exit(-1);
	}
		
}

void Converter::ExportMesh(wstring savePath)
{
	ReadBoneData(scene->mRootNode, -1, -1);
	ReadSkinData();
	WriteMeshData(L"../../_Models/" + savePath + L".mesh");
}

void Converter::ReadBoneData(aiNode* node, int index, int parent)
{
	asBone* bone = new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();
	
	Matrix transform(node->mTransformation[0]);
	D3DXMatrixTranspose(&bone->Transform, &transform);

	Matrix matParent;
	if (parent < 0)
		D3DXMatrixIdentity(&matParent);
	else
		matParent = bones[parent]->Transform;

	bone->Transform = bone->Transform * matParent;
	bones.push_back(bone);
	
	ReadMeshData(node, index);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int index)
{
	if (node->mNumMeshes < 1) return;

	asMesh* mesh = new asMesh();
	mesh->BoneIndex = index;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		UINT startVertex = mesh->Vertices.size();
		UINT startIndex = mesh->Indices.size();

		//Read Vertices
		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			SkeletalMesh::VertexSkeletalMesh vertex;
			memcpy(&vertex.Position,  &srcMesh->mVertices[v], sizeof(Vector3));

			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));
			
			if (srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);
		}

		//Read Indices
		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
				mesh->Indices.push_back(face.mIndices[k] + startVertex);
		}

		//Read MeshParts
		asMeshPart* meshPart = new asMeshPart();

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];

		meshPart->MaterialName = material->GetName().C_Str();
		meshPart->StartVertex = startVertex;
		meshPart->StartIndex = startIndex;
		meshPart->VertexCount = srcMesh->mNumVertices;
		meshPart->IndexCount = srcMesh->mNumFaces * srcMesh->mFaces->mNumIndices;

		mesh->MeshParts.push_back(meshPart);
	}

	meshes.push_back(mesh);
}

void Converter::ReadSkinData()
{
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];

		if (aiMesh->HasBones() == false) continue;

		asMesh* mesh = meshes[i];

		vector<asBoneWeight> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeight());

		for (UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			aiBone* aiMeshBone = aiMesh->mBones[b];

			UINT boneIndex = 0;

			for (asBone* bone : bones)
			{
				if (bone->Name == (string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;
					break;
				}
			}

			for (UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				UINT vertexID = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				boneWeights[vertexID].AddWeights(boneIndex, weight);
			} //for (w)
		} //for (b)


		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[w].Normalize();

			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);

			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}

	}

}

void Converter::WriteMeshData(wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter(savePath);

	//Save Bone
	w->UInt(bones.size());
	for (asBone* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);
	}

	//Save Mesh
	w->UInt(meshes.size());
	for (asMesh* mesh : meshes)
	{
		w->Int(mesh->BoneIndex);

		w->UInt(mesh->Vertices.size());
		w->Byte(&mesh->Vertices[0], sizeof(SkeletalMesh::VertexSkeletalMesh) * mesh->Vertices.size());

		w->UInt(mesh->Indices.size());
		w->Byte(&mesh->Indices[0], sizeof(UINT) * mesh->Indices.size());

		w->UInt(mesh->MeshParts.size());
		for (asMeshPart* part : mesh->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCount);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);
		}
	}

	SafeDelete(w);
}

void Converter::ExportMatrial(wstring savePath/*Tank/Tank.material*/, bool bOverWrite)
{
	savePath = L"../../_Textures/" + savePath + L".material";

	if (bOverWrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	ReadMaterialData();
	WriteMaterialData(savePath); 
}

void Converter::ReadMaterialData()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];

		if (FoundMaterialData(srcMaterial) == false)
			continue;

		asMaterial* material = new asMaterial();

		material->Name = srcMaterial->GetName().C_Str();

		//Get Color
		aiColor3D color;

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->Ambient = Color(color.r, color.g, color.b, 1.f);

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->Diffuse = Color(color.r, color.g, color.b, 1.f);

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->Specular = Color(color.r, color.g, color.b, 1.f);

		srcMaterial->Get(AI_MATKEY_SHININESS, material->Specular.a);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->Emissive = Color(color.r, color.g, color.b, 1.f);

		//Get MapFile
		aiString file;

		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();

		materials.push_back(material);
	}
}

bool Converter::FoundMaterialData(aiMaterial* material)
{
	string materialName = material->GetName().C_Str();

	for (asMesh* mesh : meshes)
	{
		for (asMeshPart* part : mesh->MeshParts)
		{
			if (part->MaterialName == materialName)
				return true;
		}
	}

	return false;
}

void Converter::WriteMaterialData(wstring savePath) //L"../../_Textures/Tank/Tank.material"
{
	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	
	Xml::XMLDeclaration* decl =	document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials"); //<Materials></Materials>
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (asMaterial* material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material"); //<Materials>
		root->LinkEndChild(node);

		Xml::XMLElement* element = nullptr;

		element = document->NewElement("Name"); //<Name>
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile"); //<DiffuseFile>
		element->SetText(WriteTexture(folder, material->DiffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile"); //<SpecularFile>
		element->SetText(WriteTexture(folder, material->SpecularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile"); //<NormalFile>
		element->SetText(WriteTexture(folder, material->NormalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient"); //<Ambient>
		element->SetAttribute("R", material->Ambient.r);
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse"); //<Diffuse>
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular"); //<Specular>
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive"); //<Emissive>
		element->SetAttribute("R", material->Emissive.r);
		element->SetAttribute("G", material->Emissive.g);
		element->SetAttribute("B", material->Emissive.b);
		element->SetAttribute("A", material->Emissive.a);
		node->LinkEndChild(element);
	}

	document->SaveFile((folder + file).c_str());
	SafeDelete(document);
}

string Converter::WriteTexture(string saveFolder, string file)
{
	if (file.length() < 1) return "";

	string fileName = Path::GetFileName(file);
	const aiTexture* embededTexture = scene->GetEmbeddedTexture(file.c_str());

	string path = "";

	if (embededTexture != nullptr)
	{
		path = saveFolder + Path::GetFileNameWithoutExtension(file) + ".png";

		BinaryWriter* w = new BinaryWriter(String::ToWString(path));
		w->Byte(embededTexture->pcData, embededTexture->mWidth);
		SafeDelete(w);
	}
	else
	{
		string directory = Path::GetDirectoryName(String::ToString(this->file));
		string origin = directory + file;
		String::Replace(&origin, "\\", "/");

		if (Path::ExistFile(origin) == false)
			return "";

		path = saveFolder + fileName;
		CopyFileA(origin.c_str(), path.c_str(), FALSE);

		String::Replace(&path, "../../_Textures/", "");
	}

	return Path::GetFileName(path);
}

void Converter::ExportAnimClip(UINT index, wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".clip";

	asClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath);
}

asClip* Converter::ReadClipData(aiAnimation* animation)
{
	asClip* clip = new asClip();
	clip->Name = animation->mName.C_Str();
	clip->FrameRate = (float)animation->mTicksPerSecond;
	clip->FrameCount = (UINT)animation->mDuration + 1;

	vector<asClipNode> aniNodeInfos;
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* aniNode = animation->mChannels[i];

		asClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName;

		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumScalingKeys); ;
		keyCount = max(keyCount, aniNode->mNumRotationKeys);

		asKeyframeData frameData;
		for (UINT k = 0; k < keyCount; k++)
		{
			bool bFound = false;
			UINT t = aniNodeInfo.keyframe.size();

			//PositionKey
			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mPositionKeys[k];
				memcpy_s(&frameData.Translation, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));
				frameData.Frame = (float)aniNode->mPositionKeys[k].mTime;

				bFound = true;
			}

			//RotationKey
			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiQuatKey key = aniNode->mRotationKeys[k];
				
				frameData.Rotation.x = key.mValue.x;
				frameData.Rotation.y = key.mValue.y;
				frameData.Rotation.z = key.mValue.z;
				frameData.Rotation.w = key.mValue.w;

				frameData.Frame = (float)aniNode->mRotationKeys[k].mTime;

				bFound = true;
			}

			//ScalingKey
			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mScalingKeys[k];
				memcpy_s(&frameData.Scale, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));
				frameData.Frame = (float)aniNode->mScalingKeys[k].mTime;

				bFound = true;
			}

			if (bFound == true)
				aniNodeInfo.keyframe.push_back(frameData);
		}//for(k)

		if (aniNodeInfo.keyframe.size() < clip->FrameCount)
		{
			UINT count = clip->FrameCount - aniNodeInfo.keyframe.size();
			asKeyframeData lastKeyframe = aniNodeInfo.keyframe.back();

			for (UINT n = 0; n < count; n++)
				aniNodeInfo.keyframe.push_back(lastKeyframe);
		}

		aniNodeInfos.push_back(aniNodeInfo);
		
	}//for(i)


	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos);

	return clip;
}

void Converter::ReadKeyframeData(asClip* clip, aiNode* node, vector<struct asClipNode>& aniNodeInfos)
{
	asKeyframe* keyframe = new asKeyframe();
	keyframe->BoneName = node->mName.C_Str();

	asClipNode* asClipNode = nullptr;

	for (UINT n = 0; n < aniNodeInfos.size(); n++)
	{
		if (aniNodeInfos[n].Name == node->mName)
		{
			asClipNode = &aniNodeInfos[n];
			break;
		}
	}

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		asKeyframeData frameData;

		if (asClipNode == nullptr)
		{
			frameData.Frame = (float)i;

			Matrix transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform);

			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform);
		}
		else
		{
			frameData = asClipNode->keyframe[i];
		}

		keyframe->Transforms.push_back(frameData);
	}

	clip->Keyframes.push_back(keyframe);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadKeyframeData(clip, node->mChildren[i], aniNodeInfos);
}

void Converter::WriteClipData(asClip* clip, wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter(savePath);

	w->String(clip->Name);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);
	
	w->UInt(clip->Keyframes.size());
	for (asKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(asKeyframeData) * keyframe->Transforms.size());

		SafeDelete(keyframe);
	}

	SafeDelete(w);
}
