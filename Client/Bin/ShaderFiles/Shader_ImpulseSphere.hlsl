#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

// Matrix
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// RawValue
vector g_vColor = vector(1.f, 1.f, 1.f, 0.5f);
vector g_vCamPos;
float g_fCamFar;
float g_fTime;
float g_fRimPower;
float g_fScale;


float PI = 3.14159265;

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

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(In.vPosition, g_WorldMatrix);
	Out.vWorldNormal = mul(In.vNormal, g_WorldMatrix);

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
	float3 vViewDir = normalize(g_vCamPos.xyz - In.vWorldNormal);
	float fFresnel;
	float fEmissive;

	Out.vColor = g_vColor;

	// 외곽선 따기
	vViewDir = normalize(g_vCamPos.xyz - In.vWorldPos);
	FresnelEffect_float(In.vWorldNormal, vViewDir, g_fRimPower, fFresnel);

	// 빛나는 부분 만들기.
	fEmissive = 1.f - fFresnel;
	Out.vColor.r += fEmissive;
	Out.vColor.g += fEmissive;
	Out.vColor.b += fEmissive;
	
	// 바깥으로 갈수록 연하게
	Out.vColor.a *= fEmissive;
	Out.vColor.a *= 0.5f;

	// 알파 테스트
	if (Out.vColor.a <= 0.01f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass ImpulseSphere
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}