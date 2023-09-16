#include "Shader_Client_Defines.hlsli"

Texture2D g_Texture;
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// For Progress
float g_fPercent;

// For MiniMap
float2 g_vMiniMapSize;
float3 g_vPlayerPos;

SamplerState g_Sampler
{
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float2 vMiniMapTexUV : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
	float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MINIMAP(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
	float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);
	Out.vMiniMapTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return vColor;
}

float4 PS_MAIN_UI(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	//if (0.1f >= vColor.a)
	//    discard;

	return vColor;
}

float4 PS_MAIN_HP_PROGRESS(PS_IN In) : SV_TARGET0
{
	float4 vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	vColor = vColor * float4(0.f, 1.f, 0.f, 1.f);

	float HP = 1.f - g_fPercent;

	if (In.vTexUV.x < HP)
	{
		discard;
	}

	return vColor;
}

float4 PS_MAIN_FINISHER_PROGRESS(PS_IN In) : SV_TARGET0
{
	float4 vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	vColor = vColor * float4(0.f, 102.f / 255.f, 1.f, 1.f);

	float HP = 1.f - g_fPercent;

	if (In.vTexUV.x < HP)
	{
		discard;
	}

	return vColor;
}

float4 PS_MAIN_MINIMAP(PS_IN In) : SV_TARGET0
{
	float4 vColor;

	float2 vCenter = float2(-0.5f, 0.5f) - float2(1.f - g_vPlayerPos.x, g_vPlayerPos.z) / g_vMiniMapSize;

	float2 vMiniMapUV = In.vTexUV + vCenter;

	float2 vCircleCenter = vCenter + 0.5f;

	float fLength = length(vMiniMapUV - vCircleCenter);
	if (fLength > 0.4f)
	{
		discard;
	}

	vColor = g_Texture.Sample(g_Sampler, vMiniMapUV);
	return vColor;
}

technique11 DefaultTechnique
{
	pass BackGround
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass UI
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI();
	}

	pass HP_Progress
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_HP_PROGRESS();
	}

	pass Finisher_Progress
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_FINISHER_PROGRESS();
	}


	pass MiniMap
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_MINIMAP();
	}
}