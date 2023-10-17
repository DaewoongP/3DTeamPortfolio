#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

// Matrix
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// RawValue
vector g_vColor = vector(1.f, 1.f, 1.f, 0.5f);
float3 g_vCamPos;
float g_fCamFar;
float g_fFresnelPower;
float g_fScale;
float g_fEmissivePower = 1.f;
float g_fTimeAcc;

float PI = 3.14159265;

texture2D g_NoiseTexture;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float3 vWorldNormal : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	matrix matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix).xyz;
	float3x3 matWorld3x3 = (float3x3)g_WorldMatrix;
	Out.vWorldNormal = mul(In.vNormal, matWorld3x3);

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
	float3 vWorldNormal : TEXCOORD2;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	float3 vViewDir = (float3)0;
	float fFresnel = 0.f;
	float fEmissive = 0.f;
	float2 vOffsetUV;
	Out.vColor = g_vColor;

	// 외곽선 따기
	vViewDir = normalize(g_vCamPos - In.vWorldPos);
	FresnelEffect_float(In.vWorldNormal, vViewDir, g_fFresnelPower, fFresnel);
	fEmissive = 1.f - fFresnel;
	
	// 일렁이는 효과 추가
	TilingAndOffset_float(In.vTexUV, 1.f, float2(0.f, g_fTimeAcc), vOffsetUV);
	float4 vNoiseValue = g_NoiseTexture.Sample(LinearSampler, vOffsetUV);
	float remappedValue = lerp(0.8f, 1.2f, vNoiseValue.g);
	
	// 바깥으로 갈수록 연하게
	Out.vColor.r += (fEmissive * g_fEmissivePower) * remappedValue;
	Out.vColor.g += (fEmissive * g_fEmissivePower) * remappedValue;
	Out.vColor.b += (fEmissive * g_fEmissivePower) * remappedValue;

	Out.vColor.a = fEmissive;

	// 알파 테스트
	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass ImpulseSphere
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}