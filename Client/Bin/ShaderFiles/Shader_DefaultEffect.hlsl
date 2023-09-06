#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
texture2D		g_DiffuseTexture;

float4			g_vColor;
float4			g_vCamPosition;
float2			g_vOffset;

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


/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	Out.vNormal = normalize(vWorldNormal);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	return Out;
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

	TilingAndOffset_float(In.vTexUV, 1.f, g_vOffset, In.vTexUV);

	vector		vDiffuse = g_Texture.Sample(LinearSampler, In.vTexUV);
	//if (vDiffuse.r == 0.f && vDiffuse.g == 0.f && vDiffuse.b == 0.f)
	//	vDiffuse.a = 0;

	if (vDiffuse.a < 0.1f)
		discard;

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT	PS_MAIN_PICKING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor.x = g_vColor.x / 255.f;
	Out.vColor.y = g_vColor.y / 255.f;
	Out.vColor.z = g_vColor.z / 255.f;
	Out.vColor.w = 1.f;

	return Out;
}

technique11		DefaultTechnique
{
	pass Default
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

	pass Tree
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Picking
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_PICKING();
	}
}