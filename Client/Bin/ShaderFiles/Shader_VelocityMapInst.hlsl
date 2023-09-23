#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_PrevoWorldMatrix;
texture2D g_VelocityTexture;
texture2D g_Texture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;
	vector vVelocity;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	float4 vValue = g_VelocityTexture.Sample(LinearSampler, In.vTexUV);
	float fOffset = 1.f;
	float2 vVelocity = vValue.xy;
	uint g_iSampleCnt = 16;
	vVelocity.x = vVelocity.x * fOffset;
	vVelocity.y = vVelocity.y * fOffset;

	float2 vTexUV = In.vTexUV;

	float4 vColor = g_Texture.Sample(LinearSampler, vTexUV);

	vTexUV += vVelocity;

	for (uint i = 1; i < g_iSampleCnt; ++i)
	{
		float4 vNewColor = g_Texture.Sample(LinearSampler, vTexUV);

		vColor += vNewColor;

		vTexUV += vVelocity;
	}

	Out.vColor = vColor / g_iSampleCnt;

	return Out;
}

technique11	DefaultTechnique
{
	pass Velocity
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}