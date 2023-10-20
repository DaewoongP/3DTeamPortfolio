#include "Shader_Functions.hlsli"
#include "Shader_EngineHeader.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;
float2 g_vTiling;
float4 g_vColor;
float g_fTimeAcc;
float g_fEmissiveStrength;
bool g_isBlur;

float g_fWinSizeX = 1280.f, g_fWinSizeY = 720.f;
uint g_iSampleCnt = 19;

uint g_iWeights = 19; // 개수 통일
float g_fBlurWeights[19] =
{
	0.0561f, 0.1353f, 0.278f,
	0.3221f, 0.4868f, 0.5881f,
	0.6661f, 0.7261f, 0.9231f,
	0.999f,
	0.9231f, 0.7261f, 0.6661f,
	0.5881f, 0.4868f, 0.3221f,
	0.278f, 0.1353f, 0.0561f,
};

float g_fLowBlurWeights[19] =
{
	0.0361f, 0.0553f, 0.1280f,
	0.1721f, 0.2268f, 0.2881f,
	0.3361f, 0.3576f, 0.4031f,
	0.459f,
	0.4031f, 0.3576f, 0.3361f,
	0.2881f, 0.2268f, 0.1721f,
	0.1280f, 0.0553f, 0.0361f,
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
};

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 OffsetUV;
	float2 fTexelSize = float2(1.0 / g_fWinSizeX, 1.0 / g_fWinSizeY);
	float4 vBlur;
	TilingAndOffset_float(In.vTexUV, g_vTiling, g_vOffset, OffsetUV);

	vector vNormal = g_NormalTexture.Sample(LinearSampler, OffsetUV);
	vector vEmmisive = g_EmissiveTexture.Sample(LinearSampler, OffsetUV);

	int iValue = (g_iWeights - 1) / 2;
	float fSampleCount = (2 * iValue + 1) * (2 * iValue + 1); // 계산을 반복문 외부로 이동

	for (int i = -iValue; i <= iValue; ++i)
	{
		for (int j = -iValue; j <= iValue; ++j)
		{
			vBlur += g_EmissiveTexture.Sample(LinearSampler, In.vTexUV + float2(i, j) * fTexelSize);
		}
	}

	vBlur /= fSampleCount;
	Out.vColor = g_vColor;
	Out.vColor *= vBlur;
	Out.vColor.rgb += vEmmisive * g_fEmissiveStrength;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
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
