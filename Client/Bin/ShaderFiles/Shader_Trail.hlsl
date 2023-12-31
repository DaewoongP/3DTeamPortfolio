#include "Shader_EngineHeader.hlsli"
#include "Shader_Functions.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
// 모양을 잡기 위함
texture2D		g_AlphaTexture;
// 그라디언트 색상을 잡기 위함.
texture2D		g_GradientTexture;
// 그라디언트 색상잡기용 
float g_fClipThreshold = 0.00f;

float3			g_vHeadColor = float3(1.f, 1.f, 1.f);
float3			g_vTailColor = float3(1.f, 1.f, 1.f);
int				g_iClipChannel;
bool			g_isUseTextureColor = false;
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
	
	Out.vColor = vector(1.f, 1.f, 1.f, 1.f);
	if (true == g_isUseTextureColor)
	{
		Out.vColor *= vDiffuse;
	}
	
    Out.vColor.xyz *= lerp(g_vHeadColor, g_vTailColor, fGradient).xyz;


	
	Out.vColor.a = vAlpha.r;

	if (0 == g_iClipChannel)
		Out.vColor.a = vAlpha.r;
	else if (1 == g_iClipChannel)
		Out.vColor.a = vAlpha.g;
	else if (2 == g_iClipChannel)
		Out.vColor.a = vAlpha.b;
	else if (3 == g_iClipChannel)
		Out.vColor.a = vAlpha.a;

    if (Out.vColor.a < g_fClipThreshold)
		discard;

	return Out;
}

//콘프링코는 자체 그라디언트를 씁니다.
PS_OUT PS_CONFRINGO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
    vector vDiffuse = g_AlphaTexture.Sample(LinearSampler, In.vTexUV);
	
    float2 vGradientUV = float2((1-vAlpha.r), 0);
    float4 vGradientColor = g_GradientTexture.Sample(LinearSampler, vGradientUV);
	
    Out.vColor = vDiffuse;
    Out.vColor.xyz *= vGradientColor.xyz;
    Out.vColor.a = vAlpha.r;

    if (Out.vColor.a < 0.1f)
        discard;

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

    pass Confringo
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_CONFRINGO();
    }
}