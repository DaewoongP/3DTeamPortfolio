#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_TransformationMatrix;
texture2D		g_MaterialTexture;
//texture2D		g_ClipTexture;
int				g_iClipChannel;
float			g_fClipThreshold;

float4			g_vColor;
float4			g_vCamPosition;

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
};

void MainLogic(VS_IN In, out VS_OUT Out)
{
	matrix matTW, matTWV, matTWVP;

	matTW = mul(g_TransformationMatrix, g_WorldMatrix);
	matTWV = mul(matTW, g_ViewMatrix);
	matTWVP = mul(matTWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matTWVP);
	vector vWorldNormal = mul(vector(In.vNormal, 0.f), matTW);

	Out.vNormal = normalize(vWorldNormal);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), matTW);
}

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	MainLogic(In, Out);

	return Out;
}

VS_OUT VS_MAIN2(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	MainLogic(In, Out);
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	TilingAndOffset_float(In.vTexUV, g_vTililing, g_vOffset, In.vTexUV);

	vector		vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);
	//vector		vClipTexture = g_ClipTexture.Sample(LinearSampler, In.vTexUV);

	if (0 == g_iClipChannel)
		Out.vColor.a = vDiffuse.r;
	else if (1 == g_iClipChannel)
		Out.vColor.a = vDiffuse.g;
	else if (2 == g_iClipChannel)
		Out.vColor.a = vDiffuse.b;
	else if (3 == g_iClipChannel)
		Out.vColor.a = vDiffuse.a;

	if (Out.vColor.a < g_fClipThreshold)
	{
		discard;
	}
	
	Out.vColor = vDiffuse;
	Out.vColor *= g_vColor;
	  
	return Out;
}

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT	PS_NONBLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	TilingAndOffset_float(In.vTexUV, g_vTililing, g_vOffset, In.vTexUV);

	vector		vDiffuse = g_MaterialTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse;
	Out.vColor *= g_vColor;

	return Out;
}

technique11		DefaultTechnique
{
	pass Default_AlphaBlend
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_NonBlend
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_NONBLEND();
	}
}