#pragma once


class SkeletalMeshAnimator
{
public:
	SkeletalMeshAnimator(Shader* shader);
	~SkeletalMeshAnimator();

	void Update();
	void Render();


public:
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	void ReadClip(wstring file);

	void SetShader(Shader* shader, bool CalledByUpdate = false);
	void Pass(UINT pass);

	Transform* GetTransform() { return transform; }
	SkeletalMesh* GetSkeletalMesh() { return skeletalMesh; }


private:
	void CreateTexture();
	void CreateClipTransform(UINT index);


private:
	struct ClipTransform
	{
		Matrix** Transform;

		ClipTransform()
		{
			Transform = new Matrix* [MAX_KEYFRAME_COUNT];

			for (UINT i = 0; i < MAX_KEYFRAME_COUNT; i++)
			{
				Transform[i] = new Matrix [MAX_BONE_COUNT];
			}
		}
		
		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_KEYFRAME_COUNT; i++)
				SafeDeleteArray(Transform[i]);

			SafeDeleteArray(Transform);
		}
	};
	ClipTransform* clipTransforms;


private:
	ID3D11Texture2D* texture = nullptr;			//Texture2DArray 보낼때, Texture3D 읽을때
	ID3D11ShaderResourceView* transformsSRV;
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;


private:
	struct KeyframeDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0;
		float RunningTime = 0.0f;
	
		float Speed = 1.0f;

		Vector2 Padding;
	} keyframeDesc;
	
	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;



private:
	Shader* shader;
	SkeletalMesh* skeletalMesh;

	Transform* transform;						//World
};
