#include "00_Global.fx"
#include "00_Light.fx"

float3 LightDirection;

//Reder
struct VertexInput
{
	float4 Position : Position;
	float2 Uv : Uv;
	float3 Normal : Normal;
	float3 Tangent : Tangent;
	float4 BlendIndices : BlendIndices;
	float4 BlendWeights : BlendWeights;
};

struct VertexOutput
{
	float4 Position : SV_Position;
	float2 Uv : Uv;
	float3 Normal : Normal;
};


#define MAX_BONE_COUNT 250
cbuffer CB_Bones
{
	Matrix BoneTransforms[MAX_BONE_COUNT]; //Component(Bone) Space

	uint BoneIndex;
};

struct KeyframeDesc
{
	int Clip;

	uint CurrFrame;
	uint NextFrame;

	float Time;
	float RunningTime;

	float Speed;
};

struct TweenFrame
{
	
	float TakeTime;
	float TweenTime;
	float ChangeTime;
	float Pdding;


	KeyframeDesc Curr;
	KeyframeDesc Next;
};

cbuffer CB_Animationframe
{
	TweenFrame TweenFrames;
};



Texture2DArray TransformsMap;

void SetAnimationWorld(inout matrix world, VertexInput input)
{
	float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
	float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
	
	//[0]currntClip, [1]nextClip
	int clip[2];
	int currFrame[2], nextFrame[2];
	float time[2];
	
	//currntClip
	clip[0] = TweenFrames.Curr.Clip;
	currFrame[0] = TweenFrames.Curr.CurrFrame;
	nextFrame[0] = TweenFrames.Curr.NextFrame;
	time[0] = TweenFrames.Curr.Time;
	
	//nextClip
	clip[1] = TweenFrames.Next.Clip;
	currFrame[1] = TweenFrames.Next.CurrFrame;
	nextFrame[1] = TweenFrames.Next.NextFrame;
	time[1] = TweenFrames.Next.Time;
	
	float4 c0, c1, c2, c3;
	float4 n0, n1, n2, n3;
	
	matrix transform = 0;
	matrix curr = 0, next = 0;
	matrix currAnim = 0, nextAnim = 0;
	
	[unroll(4)]
	for (int i = 0; i < 4; i++)
	{
		//------------------------------------------------------------------------------------------------
		//Current Clip
		//
		//Current Frame
		c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0)); //_11_12_13_14
		c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0)); //_21_22_23_24
		c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0)); //_31_32_33_34
		c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0)); //_41_42_43_44
		curr = matrix(c0, c1, c2, c3);
		
		//Next Frame
		n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0)); //_11_12_13_14
		n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0)); //_21_22_23_24
		n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0)); //_31_32_33_34
		n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0)); //_41_42_43_44
		next = matrix(n0, n1, n2, n3);
		
		//Inter Frame Lerp
		currAnim = lerp(curr, next, time[0]);
		//------------------------------------------------------------------------------------------------
		
		
		//------------------------------------------------------------------------------------------------
		//Next Clip
		[flatten]
		if (clip[1] > -1)
		{
			
			//Current Frame
			c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0)); //_11_12_13_14
			c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0)); //_21_22_23_24
			c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0)); //_31_32_33_34
			c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0)); //_41_42_43_44
			curr = matrix(c0, c1, c2, c3);									
																			
			//Next Frame													
			n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0)); //_11_12_13_14
			n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0)); //_21_22_23_24
			n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0)); //_31_32_33_34
			n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0)); //_41_42_43_44
			next = matrix(n0, n1, n2, n3);
			
			//Inter Frame Lerp
			nextAnim = lerp(curr, next, time[1]);
			
			//Inter Clip Lerp
			currAnim = lerp(currAnim, nextAnim, TweenFrames.TweenTime);
		}
		//------------------------------------------------------------------------------------------------
		
		
		//Add Skinning Weigths
		transform += mul(weights[i], currAnim);
	};
	
	world = mul(transform, world);
}

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	
	SetAnimationWorld(World, input);
	
	output.Position = WorldPosition(input.Position);
	output.Position = ViewProjection(output.Position);

	output.Normal = WorldNormal(input.Normal);

	output.Uv = input.Uv;

	return output;
}

float4 PS_Diffuse(VertexOutput input) : SV_Target
{
	float3 normal = normalize(input.Normal);
	float lambert = saturate(dot(normal, -LightDirection));

	float4 diffuseColor = DiffuseMap.Sample(LinearSampler, input.Uv);
	return diffuseColor * lambert;
}

float4 PS_WireFrame(VertexOutput input) : SV_Target
{
	return float4(0.5f, 0.5f, 0.5f, 1);
}


technique11 T0
{
	P_VP(P0, VS, PS_Diffuse)
	P_RS_VP(P1, FillMode_WireFrame, VS, PS_WireFrame)
}