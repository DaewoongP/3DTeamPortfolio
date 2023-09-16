#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_AlphaTexture;
texture2D		g_GradientTexture;

float3			g_vHeadColor = float3(1.f, 1.f, 1.f);
float3			g_vTailColor = float3(1.f, 1.f, 1.f);


int g_iCurIndex;
int g_iWidthLength;
int g_iHeightLength;
int g_iClipChannel;

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

/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_FLIPBOOK_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	// ex)
	// g_iCurIndex = 6, 
	// g_iWidthLength = 4, g_iHeightLenght = 5

	int iCurWidth = g_iCurIndex % g_iWidthLength;		// 6 % 4 = 2
	int iCurHeight = int(g_iCurIndex / g_iWidthLength); // 6 / 4 = int(1)
	float fUnitWidth = 1.f / g_iWidthLength;			// 1.f / 4 = 0.25f
	float fUnitHeight = 1.f / g_iHeightLength;			// 1.f / 5 = 0.2f;
	float2 vStartUV;

	// 스프라이트의 시작 UV 좌표 계산
	vStartUV.x = iCurWidth * fUnitWidth;	// 2 * 0.25f = 0.5f
	vStartUV.y = iCurHeight * fUnitHeight;	// 1 * 0.2f = 0.2f

	// 해당 스프라이트 내에서의 UV 계산
	Out.vTexUV.x = In.vTexUV.x * fUnitWidth + vStartUV.x;	// x * 0.25f + 0.5f
	Out.vTexUV.y = In.vTexUV.y * fUnitHeight + vStartUV.y;	// y * 0.2f + 0.2f

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

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDiffuse = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	float		fGradient = g_GradientTexture.Sample(LinearSampler, In.vTexUV).r;

	Out.vColor = vDiffuse;
	Out.vColor.xyz *= lerp(g_vHeadColor, g_vTailColor, fGradient);
	Out.vColor.a = vAlpha.r;

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT	PS_FLIPBOOK_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vTexture = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.rgb = vTexture.rgb;

	if(0 == g_iClipChannel) Out.vColor.a = vTexture.r;
	else if(1 == g_iClipChannel) Out.vColor.a = vTexture.g;
	else if(2 == g_iClipChannel) Out.vColor.a = vTexture.b;
	else if(3 == g_iClipChannel) Out.vColor.a = vTexture.a;
	
	return Out;
}

technique11		DefaultTechnique
{
	pass Default
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

	pass Flipbook
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FLIPBOOK_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_FLIPBOOK_MAIN();
	}
}