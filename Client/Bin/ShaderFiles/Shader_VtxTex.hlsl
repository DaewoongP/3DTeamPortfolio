#include "Shader_Client_Defines.hlsli"

Texture2D g_Texture;
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_AlphaTexture;
Texture2D g_DissolveTexture;

// For Progress
float g_fPercent;

// For MiniMap
float2 g_vMiniMapSize;
float3 g_vPlayerPos;

// For Rotation
float g_fRadian;

// For Interaction
bool g_isClicked;
bool g_isOnCollision;

// For Glitter
float		g_fDissolveAmount; // 0~1��\

// For Cool
float		g_fCoolTime;

// For Dynamic_Back
float		g_fAlphaRatio = 1.f;

// For SkillBack
float4		g_vBackColor;

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

float4 PS_MAIN_UI_REMOVE_BLACK(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	if (vColor.r < 0.2 && vColor.g < 0.2 && vColor.b < 0.2)
	{
		discard;
	}

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

float4 PS_MAIN_HP_PROGRESS_RED(PS_IN In) : SV_TARGET0
{
	float4 vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	vColor = vColor * float4(1.f, 0.f, 0.f, 1.f);

	float HP = g_fPercent;

	if (In.vTexUV.x > HP)
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

float4 PS_MAIN_CURSOR(PS_IN In) : SV_TARGET0
{
	float4 vColor = g_Texture.Sample(g_Sampler, In.vTexUV);

	float fCos = cos(g_fRadian);
	float fSin = sin(g_fRadian);

	float2x2 fRot = float2x2(fCos, -fSin, fSin, fCos);

	float2 fCenter = float2(0.5f, 0.5f);
	float fLength = length(In.vTexUV - fCenter);

	if (fLength < 0.38f)
	{
		In.vTexUV -= 0.5f;
		In.vTexUV = mul(In.vTexUV, fRot);
		In.vTexUV += 0.5f;
		vColor = g_Texture.Sample(g_Sampler, In.vTexUV);
	}
	else
	{
		In.vTexUV -= 0.5f;
		In.vTexUV = mul(In.vTexUV, float2x2(fCos, fSin, -fSin, fCos));
		In.vTexUV += 0.5f;
		vColor = g_Texture.Sample(g_Sampler, In.vTexUV);
	}

	if (vColor.r < 0.3 && vColor.g < 0.3 && vColor.b < 0.3)
	{
		discard;
	}
	else
	{
		vColor.rgb = float3(1.f, 1.f, 1.f);
	}


	return vColor;
}

float4 PS_MAIN_TEXT(PS_IN In) : SV_TARGET0
{
	float4 vColor = g_Texture.Sample(PointSampler, In.vTexUV);

	if (g_isClicked)
	{
		if (vColor.a > 0.1f)
		{
			vColor = float4(1.f, 1.f, 1.f, 1.f);
		}
	}
	else if (g_isOnCollision)
	{
		if (vColor.a > 0.1f)
		{
			vColor = vColor * float4(153.f / 255.f, 153.f / 255.f, 153.f / 255.f, 1.f);
		}
	}
	else
	{
		if (vColor.a > 0.1f)
		{
			vColor = vColor * float4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.f);
		}
	}

	return vColor;
}

float4 PS_MAIN_UI_ALPHA(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;
	float4 vAlpha = (float4) 0;

	vAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	vColor.a *= vAlpha.r;

	return vColor;
}


float4 PS_MAIN_UI_DISSOLVE(PS_IN In) : SV_TARGET0
{
	vector vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	float fDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexUV).r;

	float fEdgeThickness = 0.07f;

	vector vRed = float4(0.f, 0.f, 0.0f, 1.0f);
	vector vYellow = float4(1.0f, 0.6471f, 0.0f, 1.0f);
	vector vWhite = float4(1.0f, 0.2706f, 0.0f, 1.0f);

	if (fDissolve > g_fDissolveAmount + fEdgeThickness)
	{
		discard;
	}
	else if (fDissolve > g_fDissolveAmount)
	{
		float blendFactor = (fDissolve - g_fDissolveAmount) / fEdgeThickness;
		vColor.rgb = lerp(lerp(vRed.rgb, vYellow.rgb, blendFactor), vWhite.rgb, blendFactor);
	}

	return vColor;
}

float4	PS_MAIN_UI_COOLTIME(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	if (vColor.a < 0.1f)
		discard;

	if (In.vTexUV.y < 1.f - g_fCoolTime)
	{
		vColor.rgb = 0.f;
	}

	return vColor;
}

float4	PS_MAIN_UI_GRAYCOOL(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	if (vColor.a < 0.1f)
		discard;

	if (In.vTexUV.y < 1.f - g_fCoolTime)
	{
		vColor.rgb = 0.3f;
	}

	return vColor;

}

float4 PS_MAIN_DYNAMIC_BACK(PS_IN In) :SV_TARGET0
{
	vector vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	vColor.a *= g_fAlphaRatio;

	return vColor;
}

float4 PS_MAIN_UI_SKILLBACK(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float Bright = 1.1f;
	vColor.rgb *= g_vBackColor.rgb * Bright;

	return vColor;
}

float4 PS_MAIN_UI_MAINSKILL(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	float Bright = 1.1f;
	vColor.rgb *= g_vBackColor.rgb * Bright;

	if (In.vTexUV.y < 1.f - g_fCoolTime)
	{
		vColor.rgb = 0.f;
	}

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

	pass UI_Remove_Black
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_REMOVE_BLACK();
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

	pass HP_ProgressRED
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_HP_PROGRESS_RED();
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

	pass Cursor
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_CURSOR();
	}

	pass Text
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_TEXT();
	}

	pass Alpha
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_ALPHA();
	}

	pass Dissolve
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_DISSOLVE();
	}

	pass Cool
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_COOLTIME();
	}

	pass Dynamic_Back
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_DYNAMIC_BACK();
	}

	pass GrayCool
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_GRAYCOOL();
	}

	pass SkillBack
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_SKILLBACK();
	}

	pass MainSkill
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN_UI_MAINSKILL();
	}
}