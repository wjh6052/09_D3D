//Parameters
matrix World, View, Projection;
float3 LightDirection;
Texture2D DiffuseMap;

//Stats
SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

RasterizerState FillMode_WireFrame
{
	FillMode = WireFrame;
};

//Reder
struct VertexInput
{
	float4 Position : Position;
	float2 Uv : Uv;
	float3 Normal : Normal;
};

struct VertexOutput
{
	float4 Position : SV_Position;
	float2 Uv : Uv;
	float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
	VertexOutput output;
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
	output.Normal = mul(input.Normal, (float3x3)World);
	
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
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_Diffuse()));
	}

	pass P1
	{
		SetRasterizerState(FillMode_WireFrame);

		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS_WireFrame()));
	}

}